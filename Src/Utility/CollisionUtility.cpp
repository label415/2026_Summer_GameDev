#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Object/Common/Collider/ColliderSphere.h"
#include "../Object/Common/Collider/ColliderLine.h"
#include "../Object/Common/Collider/ColliderModel.h"
#include "../Object/Common/Transform.h"
#include "AsoUtility.h"
#include "CollisionUtility.h"

bool CollisionUtility::IsHit(
	const ColliderCapsule& capsule1,
	const ColliderCapsule& capsule2)
{
	// カプセル同士の衝突を判定
	return HitCheck_Capsule_Capsule(
		capsule2.GetPosTop(), capsule2.GetPosDown(),
		capsule2.GetRadius(),
		capsule1.GetPosTop(), capsule1.GetPosDown(),
		capsule1.GetRadius());
}

bool CollisionUtility::IsHit(
	const ColliderCapsule& capsule,
	const ColliderSphere& sphere)
{
	// 球体とカプセルの衝突を判定
	return HitCheck_Sphere_Capsule(
		sphere.GetPos(), sphere.GetRadius(),
		capsule.GetPosTop(), capsule.GetPosDown(),
		capsule.GetRadius());
}

bool CollisionUtility::IsHit(
	const ColliderCapsule& capsule,
	const ColliderModel& model,
	bool isExclude, bool isTarget)
{
	// モデルの追従先を取得
	auto modelFollow = model.GetFollow();
	if (!modelFollow) return false;

	// ポリゴンとカプセルを判定
	auto hits = MV1CollCheck_Capsule(
		modelFollow->modelId, -1,
		capsule.GetPosTop(), capsule.GetPosDown(),
		capsule.GetRadius());

	// 衝突処理
	bool isHit = false;
	for (int i = 0; i < hits.HitNum; i++)
	{
		// ポリゴンが有効かどうかを判定
		if (!IsValidPoly(
			model, hits.Dim[i].FrameIndex,
			isExclude, isTarget))continue;

		// 有効なポリゴンに衝突している
		isHit = true;
		break;
	}

	// メモリ解放
	MV1CollResultPolyDimTerminate(hits);

	return isHit;
}

bool CollisionUtility::IsHit(
	const ColliderSphere& sphere,
	const ColliderModel& model,
	bool isExclude, bool isTarget)
{
	// モデルの追従先を取得
	auto modelFollow = model.GetFollow();
	if (!modelFollow) return false;

	// 球体とポリゴンを判定
	auto hits = MV1CollCheck_Sphere(
		modelFollow->modelId, -1,
		sphere.GetPos(), sphere.GetRadius());

	// 衝突処理
	bool isHit = false;
	for (int i = 0; i < hits.HitNum; i++)
	{
		// ポリゴンが有効かどうかを判定
		if (!IsValidPoly(
			model, hits.Dim[i].FrameIndex,
			isExclude, isTarget))continue;

		// 有効なポリゴンに衝突している
		isHit = true;
		break;
	}

	// メモリ解放
	MV1CollResultPolyDimTerminate(hits);

	return isHit;
}

bool CollisionUtility::IsHit(
	const ColliderLine& line,
	const ColliderModel& model,
	bool isExclude, bool isTarget)
{
	// モデルの追従先を取得
	auto modelFollow = model.GetFollow();
	if (!modelFollow) return false;

	// 線分とポリゴンの判定
	auto hits = MV1CollCheck_LineDim(
		model.GetFollow()->modelId, -1,
		line.GetPosStart(), line.GetPosEnd());

	// 衝突処理
	bool isHit = false;
	for (int i = 0; i < hits.HitNum; i++)
	{
		// ポリゴンが有効かどうかを判定
		if (!IsValidPoly(
			model, hits.Dim[i].FrameIndex,
			isExclude, isTarget))continue;

		// 有効なポリゴンと交差している
		isHit = true;
		break;
	}

	// メモリ解放
	MV1CollResultPolyDimTerminate(hits);

	return isHit;
}

bool CollisionUtility::IsValidPoly(
	const ColliderModel& model,
	int frameIdx, bool isExclude, bool isTarget)
{
	// 除外フレームと対象フレームの対象の時は処理しない
	if (isExclude && model.IsExcludeFrame(frameIdx)
		|| isTarget && !model.IsTargetFrame(frameIdx)) return false;

	return true;
}

VECTOR CollisionUtility::CalcPushCapsuleCapsule(
	const ColliderCapsule& a,
	const ColliderCapsule& b,
	float distanc)
{
	// カプセルbの対象位置を取得
	VECTOR targetPos =
		b.GetFollow() ? b.GetFollow()->pos : b.GetPosDown();
	
	// カプセルaの軸線上でtargetPosに最も近い点を取得
	VECTOR p1 = GetNearestPointOnSegment(
		a.GetPosTop(), a.GetPosDown(), targetPos);
	
	// カプセルbの軸線上でp1に最も近い点を取得
	VECTOR p2 = GetNearestPointOnSegment(
		b.GetPosTop(), b.GetPosDown(), p1);

	// p2からp1への方向ベクトルを計算
	VECTOR vBA = VSub(p1, p2);
	float distance = VSize(vBA);
	
	// 2つのカプセルの合計半径を計算
	float totalRadius = a.GetRadius() + b.GetRadius();

	// 接触していない場合
	if (distance >= totalRadius) { return AsoUtility::VECTOR_ZERO; }

	// 距離がほぼ0の場合
	if (distance < AsoUtility::EPSILON)
	{
		// デフォルト方向を右向きにして、距離を最小値に設定
		vBA = AsoUtility::DIR_R;
		distance = AsoUtility::EPSILON;
	}

	// 押し出し量を計算
	float overlap = (totalRadius - distance) * distanc;
	
	// 方向ベクトルを正規化し、Y成分を0にして水平方向のみ処理
	VECTOR pushDir = VNorm(vBA);
	pushDir.y = 0.0f;

	// 押し出し方向と量からベクトルを生成して返す
	return VScale(pushDir, overlap);
}

VECTOR CollisionUtility::CalcPushCapsuleTriangle(
	const ColliderCapsule& capsule,
	const MV1_COLL_RESULT_POLY& poly,
	const VECTOR& pos,
	int maxTryCnt, float pushDistance)
{
	// 初期値を引数のposに設定
	VECTOR pushedPos = pos;

	// カプセルの元の位置から現在位置への移動差分を計算
	VECTOR diff = VSub(pos, capsule.GetFollow()->pos);
	
	// カプセルの上下端の座標を、移動差分に基づいて更新
	VECTOR topPos = VAdd(capsule.GetPosTop(), diff);
	VECTOR downPos = VAdd(capsule.GetPosDown(), diff);
	float radius = capsule.GetRadius();

	// ポリゴンの法線方向に指定距離だけ移動するベクトルを計算
	VECTOR pushVec = VScale(poly.Normal, pushDistance);

	// 最大試行回数まで段階的に押し戻す
	int tryCnt = 0;
	while (tryCnt < maxTryCnt)
	{
		// カプセルとポリゴンの衝突判定
		if (!HitCheck_Capsule_Triangle(
			topPos, downPos, radius,
			poly.Position[TRIANGLE_VERTEX_0],
			poly.Position[TRIANGLE_VERTEX_1],
			poly.Position[TRIANGLE_VERTEX_2]))break;

		// 衝突していたら法線方向に押し戻す
		pushedPos = VAdd(pushedPos, pushVec);
		topPos = VAdd(topPos, pushVec);
		downPos = VAdd(downPos, pushVec);

		// 試行回数を更新
		tryCnt++;
	}

	// 押し戻された位置を返す
	return pushedPos;
}

VECTOR CollisionUtility::CalcPushSphereTriangle(
	const ColliderSphere& sphere,
	const MV1_COLL_RESULT_POLY& poly,
	const VECTOR& pos,
	int maxTryCnt, float pushDistance)
{
	// 初期値を設定
	VECTOR pushedPos = pos;
	float radius = sphere.GetRadius();

	// ポリゴンの法線方向に指定距離だけ移動するベクトルを計算
	VECTOR pushVec = VScale(poly.Normal, pushDistance);

	// 最大試行回数まで段階的に押し戻す
	int tryCnt = 0;
	while (tryCnt < maxTryCnt)
	{
		// 球体とポリゴンの衝突判定
		if (!HitCheck_Sphere_Triangle(
			pushedPos, radius,
			poly.Position[TRIANGLE_VERTEX_0],
			poly.Position[TRIANGLE_VERTEX_1],
			poly.Position[TRIANGLE_VERTEX_2]))break;

		// 衝突していたら法線方向に押し戻す
		pushedPos = VAdd(pushedPos, pushVec);
		tryCnt++;
	}

	// 最終的に押し戻された位置を返す
	return pushedPos;
}

bool CollisionUtility::TryGetNearestHitPolyLineModel(
	const ColliderModel& model, const VECTOR& startPos,
	const VECTOR& endPos, MV1_COLL_RESULT_POLY& outHitPoly,
	bool isExclude, bool isTarget)
{
	// 追従先が存在しない場合は処理を行わない
	const Transform* modelFollow = model.GetFollow();
	if (modelFollow == nullptr) return false;

	// 線分がモデルと交差するポリゴンを取得
	auto hits = MV1CollCheck_LineDim(
		modelFollow->modelId,
		-1,
		endPos,
		startPos
	);

	// 衝突したポリゴンフラグ
	bool isCollision = false;
	double minDist = DBL_MAX;

	// 衝突処理
	for (int i = 0; i < hits.HitNum; i++)
	{
		// ポリゴンが有効かどうかを判定
		const auto& poly = hits.Dim[i];
		if (!IsValidPoly(
			model, poly.FrameIndex, isExclude, isTarget))continue;

		// 有効なポリゴン
		isCollision = true;

		// startPosから衝突点までの距離を計算
		double dist = AsoUtility::Distance(startPos, poly.HitPosition);
		
		// startPosに最も近い衝突点を保持
		if (minDist > dist)
		{
			minDist = dist;
			outHitPoly = poly;
		}
	}

	// メモリ解放
	MV1CollResultPolyDimTerminate(hits);

	// 衝突したポリゴンが存在する場合はtrueを返す
	return isCollision;
}

VECTOR CollisionUtility::CalcPushCapsuleModel(
	const ColliderCapsule& capsule,
	const ColliderModel& model,
	const VECTOR& pos,
	int maxTryCnt, float pushDistance,
	bool isExclude, bool isTarget)
{
	// 初期位置を設定
	VECTOR pushedPos = pos;

	// モデルとカプセルのすべての衝突を取得
	auto hits = MV1CollCheck_Capsule(
		model.GetFollow()->modelId, -1,
		capsule.GetPosTop(),
		capsule.GetPosDown(),
		capsule.GetRadius());

	// 衝突処理
	for (int i = 0; i < hits.HitNum; i++)
	{
		// 衝突したポリゴン情報を取得
		auto hitPoly = hits.Dim[i];
		
		// ポリゴンが有効かどうかを判定
		if (!IsValidPoly(
			model, hitPoly.FrameIndex,
			isExclude, isTarget)) continue;

		// 前のポリゴンで押し戻された座標を引き継いで渡す
		// 複数のポリゴンから受ける押し出しを段階的に累積させる
		pushedPos = CalcPushCapsuleTriangle(
			capsule, hitPoly, pushedPos, maxTryCnt, pushDistance);
	}

	// メモリ解放
	MV1CollResultPolyDimTerminate(hits);

	// 押し戻された位置を返す
	return pushedPos;
}

VECTOR CollisionUtility::CalcPushSphereModel(
	const ColliderSphere& sphere,
	const ColliderModel& model,
	const VECTOR& pos,
	int maxTryCnt, float pushDistance,
	bool isExclude, bool isTarget)
{
	// 初期位置を設定
	VECTOR pushedPos = pos;

	// モデルと球体のすべての衝突を取得
	auto hits = MV1CollCheck_Sphere(
		model.GetFollow()->modelId, -1,
		sphere.GetPos(),
		sphere.GetRadius());

	// 衝突処理
	for (int i = 0; i < hits.HitNum; i++)
	{
		// 衝突したポリゴン情報を取得
		auto hitPoly = hits.Dim[i];
		
		// ポリゴンが有効かどうかを判定
		if (!IsValidPoly(
			model, hitPoly.FrameIndex,
			isExclude, isTarget)) continue;

		// 前のポリゴンで押し戻された座標を引き継いで渡す
		// 複数のポリゴンから受ける押し出しを段階的に累積させる
		pushedPos = CalcPushSphereTriangle(
			sphere, hitPoly, pushedPos, maxTryCnt, pushDistance);
	}

	// メモリ解放
	MV1CollResultPolyDimTerminate(hits);

	// 押し戻された位置を返す
	return pushedPos;
}

VECTOR CollisionUtility::CalcPushUpLineModel(
	const ColliderLine& line,
	const ColliderModel& model,
	const float& currentPosY,
	float pushDistance, bool isExclude, bool isTarget)
{
	// 押し上げ位置を初期化
	VECTOR PushUpPos = { 0.0f, 0.0f, 0.0f };

	// モデルとラインのすべての衝突を取得
	auto hits = MV1CollCheck_LineDim(
		model.GetFollow()->modelId, -1,
		line.GetPosStart(), line.GetPosEnd());

	// 衝突処理
	for (int i = 0; i < hits.HitNum; i++)
	{
		// 衝突したポリゴン情報を取得
		auto hit = hits.Dim[i];
		
		// ポリゴンが有効かどうかを判定
		if (!IsValidPoly(
			model, hit.FrameIndex,
			isExclude, isTarget)) continue;

		// 衝突地点から、少し上に移動した位置を計算
		// currentPosYがポリゴンより下側にいる場合のみ位置を修正する
		if (currentPosY < hit.HitPosition.y)
		{
			// 衝突物より、下側にいる場合のみ、位置を修正する
			PushUpPos =
				VAdd(hit.HitPosition, VScale(AsoUtility::DIR_U, pushDistance));
		}
	}

	// メモリ解放
	MV1CollResultPolyDimTerminate(hits);

	// 押し上げ位置を返す
	return PushUpPos;
}

VECTOR CollisionUtility::CalcCameraPushBack(
	const ColliderSphere& sphere, const ColliderModel& model,
	const VECTOR& startPos, const VECTOR& cameraPos,
	int maxTryCnt, float pushDistance, bool isExclude, bool isTarget)
{
	// 衝突したポリゴン情報と衝突フラグを初期化
	MV1_COLL_RESULT_POLY hitPoly{};
	bool outIsLineHit = false;

	// プレイヤーとカメラを結ぶ線分が壁に遮蔽されているかを判定
	outIsLineHit = TryGetNearestHitPolyLineModel(
		model, startPos, cameraPos, hitPoly, isExclude, isTarget);

	if (outIsLineHit)
	{
		// カメラ位置からプレイヤーへの方向ベクトルを計算
		VECTOR dirToFollow = VNorm(VSub(startPos, cameraPos));

		// 衝突点からプレイヤー方向へ指定距離だけ手前にカメラを移動
		VECTOR pushedPos = VAdd(
			hitPoly.HitPosition,
			VScale(dirToFollow, pushDistance)
		);

		// さらに球体コライダーの半径分、壁の法線方向に押し戻してめり込みを防ぐ
		return CalcPushSphereTriangle(
			sphere, hitPoly, pushedPos, maxTryCnt, pushDistance);
	}

	// 線分が遮蔽されていない場合：
	// 通常の球体とモデルの押し戻し処理のみを行う
	return CalcPushSphereModel(
		sphere, model, cameraPos, maxTryCnt, pushDistance, isExclude, isTarget);
}

VECTOR CollisionUtility::GetNearestPointOnSegment(
	const VECTOR& startPos, const VECTOR& endPos, const VECTOR& targetPos)
{
	// 線分の始点から終点へのベクトルを計算
	VECTOR segmentVec = VSub(endPos, startPos);
	
	// 線分の始点からターゲットへのベクトルを計算
	VECTOR toTargetVec = VSub(targetPos, startPos);

	// 線分の長さの二乗を計算
	float lenSquare = static_cast<float>(VSquareSize(segmentVec));
	
	// 線分の長さがほぼ0の場合
	if (lenSquare < AsoUtility::EPSILON)
	{
		// 始点をそのまま返す
		return startPos;
	}
	
	// 線分上の最近接点の割合を計算
	// ベクトルの内積と線分の長さから、最近接点がどこにあるかを計算
	float segmentRatio = VDot(toTargetVec, segmentVec) / lenSquare;
	
	// 線分の範囲外の場合は、範囲内に制限する
	// (0.0以下なら始点、1.0以上なら終点に設定)
	if (segmentRatio < 0.0f) { segmentRatio = 0.0f; }
	if (segmentRatio > 1.0f) { segmentRatio = 1.0f; }

	// 線分上の最近接点を計算して返す
	VECTOR nearestPos = VAdd(startPos, VScale(segmentVec, segmentRatio));

	// 最近接点を返す
	return nearestPos;
}