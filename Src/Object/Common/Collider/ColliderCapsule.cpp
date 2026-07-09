#include <DxLib.h>
#include "../../Common/Transform.h"
#include "../../../Utility/AsoUtility.h"
#include "ColliderCapsule.h"
#include "ColliderModel.h"


ColliderCapsule::ColliderCapsule(
	TAG tag, const Transform * follow,
	const VECTOR & localPosTop, const VECTOR & localPosDown, float radius, int patrTag)
	:
	ColliderBase(SHAPE::CAPSULE, tag, follow, patrTag),
	localPosTop_(localPosTop),
	localPosDown_(localPosDown),
	radius_(radius),
	center_(AsoUtility::VECTOR_ZERO)
{
}

ColliderCapsule::~ColliderCapsule(void)
{
}
const VECTOR& ColliderCapsule::GetLocalPosTop(void) const
{
	return localPosTop_;
}
const VECTOR& ColliderCapsule::GetLocalPosDown(void) const
{
	return localPosDown_;
}
void ColliderCapsule::SetLocalPosTop(const VECTOR& pos)
{
	localPosTop_ = pos;
}
void ColliderCapsule::SetLocalPosDown(const VECTOR& pos)
{
	localPosDown_ = pos;
}
VECTOR ColliderCapsule::GetPosTop(void) const
{
	return GetRotPos(localPosTop_);
}
VECTOR ColliderCapsule::GetPosDown(void) const
{
	return GetRotPos(localPosDown_);
}
float ColliderCapsule::GetRadius(void) const
{
	return radius_;
}
void ColliderCapsule::SetRadius(float radius)
{
	radius_ = radius;
}
float ColliderCapsule::GetHeight(void) const
{
	return localPosTop_.y;
}

VECTOR& ColliderCapsule::GetCenter(void)
{
	VECTOR top = GetPosTop();
	VECTOR down = GetPosDown();
	VECTOR diff = VSub(top, down);
	center_ = VAdd(down, VScale(diff, 0.5f));
	return center_;
}

VECTOR ColliderCapsule::GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly, int maxTryCnt, float pushDistance) const
{
	// コピー生成
	Transform tmpTransform = *follow_;
	ColliderCapsule tmpCapsule = *this;
	tmpCapsule.SetFollow(&tmpTransform);
	// 衝突補正処理
	int tryCnt = 0;
	while (tryCnt < maxTryCnt)
	{
		// カプセルと三角形の当たり判定
		if (!HitCheck_Capsule_Triangle(
			tmpCapsule.GetPosTop(), tmpCapsule.GetPosDown(),
			tmpCapsule.GetRadius(),
			hitColPoly.Position[0], hitColPoly.Position[1],
			hitColPoly.Position[2]))
		{
			break;
		}
			// 衝突していたら法線方向に押し戻し
			tmpTransform.pos =
			VAdd(tmpTransform.pos, VScale(hitColPoly.Normal, pushDistance));
		tryCnt++;
	}
	return tmpTransform.pos;
}

void ColliderCapsule::PushBackAlongNormal(const ColliderModel* colliderModel, Transform& transform, int maxTryCnt, float pushDistance, bool isExclude, bool isTarget) const
{
	// モデルとカプセルの衝突判定
	auto hits = MV1CollCheck_Capsule(
		colliderModel->GetFollow()->modelId, -1,
		GetPosTop(), GetPosDown(), GetRadius());

	// 衝突した複数のポリゴンと衝突回避するまで、位置を移動させる
	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hitPoly = hits.Dim[i];
		// 除外フレームは無視する
		if (isExclude && colliderModel->IsExcludeFrame(hitPoly.FrameIndex))
		{
			continue;
		}
		// 対象フレーム以外は無視する
		if (isTarget && !colliderModel->IsTargetFrame(hitPoly.FrameIndex))
		{
			continue;
		}
		// 指定された回数と距離で三角形の法線方向に押し戻す
		transform.pos =
			GetPosPushBackAlongNormal(hitPoly, maxTryCnt, pushDistance);
	}
	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
}

void ColliderCapsule::PushBackAlongNormal(const ColliderCapsule* colliderCapsule, Transform& transform, int maxTryCnt, bool isExclude, bool isTarget) const
{
	auto hits = HitCheck_Capsule_Capsule(
		colliderCapsule->GetPosTop(), colliderCapsule->GetPosDown(), colliderCapsule->GetRadius(),
		GetPosTop(), GetPosDown(), GetRadius());

	if (!hits)return;

	int tryCnt = 0;
	if (tryCnt < maxTryCnt) {

		// プレイヤーの軸上で、敵の中心に一番近い点(p1)を求める
		VECTOR p1 = AsoUtility::GetMinHitPos(GetPosTop(), GetPosDown(), colliderCapsule->GetFollow()->pos);

		// 敵の軸上で、上記で求めたp1に一番近い点(p2)を求める（※ここをp1基準にすると精度が上がります）
		VECTOR p2 = AsoUtility::GetMinHitPos(colliderCapsule->GetPosTop(), colliderCapsule->GetPosDown(), p1);

		// 敵(p2)からプレイヤー(p1)へ向かうベクトルにする
		VECTOR vBA = VSub(p1, p2);
		float distance = VSize(vBA);
		float totalRadius = GetRadius() + colliderCapsule->GetRadius();

		// めり込み判定
		if (distance < totalRadius) {
			if (distance < 1e-6) {
				vBA = AsoUtility::DIR_R; // 重なりすぎている場合は前方に逃げる
				distance = 1e-6;
			}

			// めり込んでいる距離（侵入深さ）
			float overlap = totalRadius - distance;

			// 押し出す方向（敵からプレイヤーへの正規化ベクトル）
			VECTOR pushDir = VNorm(vBA);
			pushDir.y = 0.0f;

			// プレイヤーだけを動かす場合は overlap をそのまま使います
			transform.pos = VAdd(transform.pos, VScale(pushDir, overlap));
		}
	}

}

bool ColliderCapsule::IsHit(const ColliderModel* colliderModel, bool isExclude, bool isTarget) const
{
	bool ret = false;

	// モデルとカプセルの衝突判定
	auto hits = MV1CollCheck_Capsule(
		colliderModel->GetFollow()->modelId, -1,
		GetPosTop(), GetPosDown(), GetRadius());

	// 衝突した複数のポリゴンと衝突回避するまで、位置を移動させる
	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hitPoly = hits.Dim[i];
		// 除外フレームは無視する
		if (isExclude && colliderModel->IsExcludeFrame(hitPoly.FrameIndex))
		{
			continue;
		}
		// 対象フレーム以外は無視する
		if (isTarget && !colliderModel->IsTargetFrame(hitPoly.FrameIndex))
		{
			continue;
		}
		
		ret = true;
		break;
	}
	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);

	return ret;
}



void ColliderCapsule::DrawDebug(int color)
{
	// 上の球体
	VECTOR pos1 = GetPosTop();
	DrawSphere3D(pos1, radius_, 5, color, color, false);
	// 下の球体
	VECTOR pos2 = GetPosDown();
	DrawSphere3D(pos2, radius_, 5, color, color, false);
	VECTOR dir;
	VECTOR s;
	VECTOR e;
	// 球体を繋ぐ線(X+)
	dir = follow_->GetRight();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);
	// 球体を繋ぐ線(X-)
	dir = follow_->GetLeft();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);
	// 球体を繋ぐ線(Z+)
	dir = follow_->GetForward();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);
	// 球体を繋ぐ線(Z-)
	dir = follow_->GetBack();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);
	// カプセルの中心
	DrawSphere3D(GetCenter(), 5.0f, 10, color, color, true);
}