#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Object/Common/Collider/ColliderSphere.h"
#include "../Object/Common/Collider/ColliderLine.h"
#include "../Object/Common/Collider/ColliderModel.h"
#include "../Object/Common/Transform.h"
#include "AsoUtility.h"
#include "ColliderUtility.h"

bool CollisionUtility::IsHit(
	const ColliderCapsule& capsule1,
	const ColliderCapsule& capsule2)
{
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
	auto modelFollow = model.GetFollow();
	if (!modelFollow) return false;

	auto hits = MV1CollCheck_Capsule(
		modelFollow->modelId, -1,
		capsule.GetPosTop(), capsule.GetPosDown(),
		capsule.GetRadius());

	bool isHit = false;
	for (int i = 0; i < hits.HitNum; i++)
	{
		if (!IsValidPoly(
			model, hits.Dim[i].FrameIndex,
			isExclude, isTarget))continue;

		isHit = true;
		break;
	}
	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
	return isHit;
}

bool CollisionUtility::IsHit(
	const ColliderSphere& sphere,
	const ColliderModel& model,
	bool isExclude, bool isTarget)
{
	auto modelFollow = model.GetFollow();
	if (!modelFollow) return false;

	auto hits = MV1CollCheck_Sphere(
		modelFollow->modelId, -1,
		sphere.GetPos(), sphere.GetRadius());

	bool isHit = false;
	for (int i = 0; i < hits.HitNum; i++)
	{
		if (!IsValidPoly(
			model, hits.Dim[i].FrameIndex,
			isExclude, isTarget))continue;

		isHit = true;
		break;
	}
	MV1CollResultPolyDimTerminate(hits);
	return isHit;
}

bool CollisionUtility::IsHit(
	const ColliderLine& line,
	const ColliderModel& model,
	bool isExclude, bool isTarget)
{
	auto modelFollow = model.GetFollow();
	if (!modelFollow) return false;

	// ステージモデル(地面)との衝突
	auto hits = MV1CollCheck_LineDim(
		model.GetFollow()->modelId, -1,
		line.GetPosStart(), line.GetPosEnd());

	bool isHit = false;
	for (int i = 0; i < hits.HitNum; i++)
	{
		if (!IsValidPoly(
			model, hits.Dim[i].FrameIndex,
			isExclude, isTarget))continue;

		isHit = true;
		break;
	}
	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
	return isHit;
}

bool CollisionUtility::IsValidPoly(
	const ColliderModel& model,
	int frameIdx, bool isExclude, bool isTarget)
{
	if (isExclude && model.IsExcludeFrame(frameIdx)
		|| isTarget && !model.IsTargetFrame(frameIdx)) return false;

	return true;
}

VECTOR CollisionUtility::CalcPushCapsuleCapsule(
	const ColliderCapsule& a,
	const ColliderCapsule& b,
	float distanc)
{
	VECTOR targetPos =
		b.GetFollow() ? b.GetFollow()->pos : b.GetPosDown();
	VECTOR p1 = GetNearestPointOnSegment(
		a.GetPosTop(), a.GetPosDown(), targetPos);
	VECTOR p2 = GetNearestPointOnSegment(
		b.GetPosTop(), b.GetPosDown(), p1);

	VECTOR vBA = VSub(p1, p2);
	float distance = VSize(vBA);
	float totalRadius = a.GetRadius() + b.GetRadius();

	if (distance >= totalRadius)
	{
		return AsoUtility::VECTOR_ZERO;
	}

	if (distance < 1e-6f)
	{
		vBA = AsoUtility::DIR_R;
		distance = 1e-6f;
	}

	float overlap = (totalRadius - distance) * distanc;
	VECTOR pushDir = VNorm(vBA);
	pushDir.y = 0.0f;

	return VScale(pushDir, overlap);
}

VECTOR CollisionUtility::CalcPushCapsuleTriangle(
	const ColliderCapsule& capsule,
	const MV1_COLL_RESULT_POLY& poly,
	const VECTOR& pos,
	int maxTryCnt, float pushDistance)
{
	// 初期値を現在の座標にする
	VECTOR pushedPos = pos;

	// 元の座標から現在のposまでの移動差分をカプセル座標にも反映する
	VECTOR diff = VSub(pos, capsule.GetFollow()->pos);
	VECTOR topPos = VAdd(capsule.GetPosTop(), diff);
	VECTOR downPos = VAdd(capsule.GetPosDown(), diff);
	float radius = capsule.GetRadius();

	// 1回あたりの押し戻しベクトル
	VECTOR pushVec = VScale(poly.Normal, pushDistance);

	int tryCnt = 0;
	while (tryCnt < maxTryCnt)
	{
		// 押し戻し後のカプセル座標で再判定する
		if (!HitCheck_Capsule_Triangle(
			topPos, downPos, radius,
			poly.Position[0],
			poly.Position[1],
			poly.Position[2]))
		{
			break;
		}

		// 衝突していたら法線方向に押し戻す
		pushedPos = VAdd(pushedPos, pushVec);
		topPos = VAdd(topPos, pushVec);
		downPos = VAdd(downPos, pushVec);

		tryCnt++;
	}

	return pushedPos;
}

VECTOR CollisionUtility::CalcPushSphereTriangle(
	const ColliderSphere& sphere,
	const MV1_COLL_RESULT_POLY& poly,
	const VECTOR& pos,
	int maxTryCnt, float pushDistance)
{
	VECTOR pushedPos = pos;
	float radius = sphere.GetRadius();

	// 1回あたりの押し戻しベクトル
	VECTOR pushVec = VScale(poly.Normal, pushDistance);

	int tryCnt = 0;
	while (tryCnt < maxTryCnt)
	{
		// diff ではなく、現在の球体中心座標である pushedPos を直接渡す
		if (!HitCheck_Sphere_Triangle(
			pushedPos, radius,
			poly.Position[0],
			poly.Position[1],
			poly.Position[2]))
		{
			break; // 衝突しなくなったらループを抜ける
		}

		// 衝突していたら法線方向に押し戻す
		pushedPos = VAdd(pushedPos, pushVec);
		tryCnt++;
	}

	return pushedPos;
}

bool CollisionUtility::TryGetNearestHitPolyLineModel(const ColliderModel& model, const VECTOR& startPos, const VECTOR& endPos, MV1_COLL_RESULT_POLY& outHitPoly, bool isExclude, bool isTarget)
{
	const Transform* modelFollow = model.GetFollow();
	if (modelFollow == nullptr) return false;

	// 線分とモデルの衝突判定を実行
	auto hits = MV1CollCheck_LineDim(
		modelFollow->modelId,
		-1,
		endPos,
		startPos
	);

	bool isCollision = false;
	double minDist = DBL_MAX;

	for (int i = 0; i < hits.HitNum; i++)
	{
		const auto& poly = hits.Dim[i];

		// 有効なフレーム（対象フレーム・除外フレーム）か確認
		if (!IsValidPoly(model, poly.FrameIndex, isExclude, isTarget))
		{
			continue;
		}

		isCollision = true;

		// 追従対象（startPos）に一番近い衝突点を優先して記録する
		double dist = AsoUtility::Distance(startPos, poly.HitPosition);
		if (minDist > dist)
		{
			minDist = dist;
			outHitPoly = poly;
		}
	}

	// 検出したポリゴン情報のメモリ解放（後始末）
	MV1CollResultPolyDimTerminate(hits);

	return isCollision;
}

VECTOR CollisionUtility::CalcPushCapsuleModel(
	const ColliderCapsule& capsule,
	const ColliderModel& model,
	const VECTOR& pos,
	int maxTryCnt, float pushDistance,
	bool isExclude, bool isTarget)
{
	VECTOR pushedPos = pos;

	// モデルとカプセルの衝突判定
	auto hits = MV1CollCheck_Capsule(
		model.GetFollow()->modelId, -1,
		capsule.GetPosTop(),
		capsule.GetPosDown(),
		capsule.GetRadius());

	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hitPoly = hits.Dim[i];
		if (!IsValidPoly(
			model, hitPoly.FrameIndex,
			isExclude, isTarget)) continue;

		// 前のポリゴンで押し戻された座標を引き継いで渡す
		pushedPos = CalcPushCapsuleTriangle(
			capsule, hitPoly, pushedPos, maxTryCnt, pushDistance);
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);

	return pushedPos;
}

VECTOR CollisionUtility::CalcPushSphereModel(
	const ColliderSphere& sphere,
	const ColliderModel& model,
	const VECTOR& pos,
	int maxTryCnt, float pushDistance,
	bool isExclude, bool isTarget)
{
	VECTOR pushedPos = pos;

	// モデルとカプセルの衝突判定
	auto hits = MV1CollCheck_Sphere(
		model.GetFollow()->modelId, -1,
		sphere.GetPos(),
		sphere.GetRadius());

	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hitPoly = hits.Dim[i];
		if (!IsValidPoly(
			model, hitPoly.FrameIndex,
			isExclude, isTarget)) continue;

		// 前のポリゴンで押し戻された座標を引き継いで渡す
		pushedPos = CalcPushSphereTriangle(
			sphere, hitPoly, pushedPos, maxTryCnt, pushDistance);
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);

	return pushedPos;
}

VECTOR CollisionUtility::CalcPushUpLineModel(
	const ColliderLine& line,
	const ColliderModel& model,
	const float& currentPosY,
	float pushDistance, bool isExclude, bool isTarget)
{
	VECTOR PushUpPos = { 0.0f, 0.0f, 0.0f };

	// ステージモデル(地面)との衝突
	auto hits = MV1CollCheck_LineDim(
		model.GetFollow()->modelId, -1,
		line.GetPosStart(), line.GetPosEnd());

	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hit = hits.Dim[i];
		if (!IsValidPoly(
			model, hit.FrameIndex,
			isExclude, isTarget)) continue;

		// 衝突地点から、少し上に移動
		if (currentPosY < hit.HitPosition.y)
		{
			// 衝突物より、下側にいる場合のみ、位置を修正する
			PushUpPos =
				VAdd(hit.HitPosition, VScale(AsoUtility::DIR_U, pushDistance));
		}
	}
	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
	return PushUpPos;
}

VECTOR CollisionUtility::CalcCameraPushBack(const ColliderSphere& sphere, const ColliderModel& model, const VECTOR& startPos, const VECTOR& cameraPos, int maxTryCnt, float pushDistance, bool isExclude, bool isTarget)
{
	MV1_COLL_RESULT_POLY hitPoly{};

	bool outIsLineHit = false;
	// プレイヤーとカメラを結ぶ線分で壁の遮蔽をチェック
	outIsLineHit = TryGetNearestHitPolyLineModel(
		model, startPos, cameraPos, hitPoly, isExclude, isTarget);

	if (outIsLineHit)
	{
		// 遮蔽されていた場合：カメラ位置から追従対象（startPos）への方向ベクトルを算出
		VECTOR dirToFollow = VNorm(VSub(startPos, cameraPos));

		// 衝突点から追従対象の方向へ少し手前にカメラを移動させる
		VECTOR pushedPos = VAdd(
			hitPoly.HitPosition,
			VScale(dirToFollow, pushDistance)
		);

		// さらに球体コライダーの半径分、壁の法線方向に押し戻してめり込みを防ぐ
		return CalcPushSphereTriangle(
			sphere, hitPoly, pushedPos, maxTryCnt, pushDistance);
	}

	// 線分が遮蔽されていない場合：通常の球体とモデルの押し戻しのみ行う
	return CalcPushSphereModel(
		sphere, model, cameraPos, maxTryCnt, pushDistance, isExclude, isTarget);
}

VECTOR CollisionUtility::GetNearestPointOnSegment(const VECTOR& statePos, const VECTOR& endPos, const VECTOR& targetPos)
{
	VECTOR segmentVec = VSub(endPos, statePos);
	VECTOR toTargetVec = VSub(targetPos, statePos);

	float lenSquare = static_cast<float>(VSquareSize(segmentVec));

	if (lenSquare < 1e-6)
	{
		return statePos;
	}

	float segmentRatio = VDot(toTargetVec, segmentVec) / lenSquare;

	if (segmentRatio < 0.0f) { segmentRatio = 0.0f; }
	if (segmentRatio > 1.0f) { segmentRatio = 1.0f; }

	VECTOR nearestPos = VAdd(statePos, VScale(segmentVec, segmentRatio));

	return nearestPos;
}