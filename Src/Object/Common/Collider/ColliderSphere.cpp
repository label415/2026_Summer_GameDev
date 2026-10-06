#include "../../Common/Transform.h"
#include "ColliderModel.h"
#include "ColliderSphere.h"

ColliderSphere::ColliderSphere(
	TAG tag, std::weak_ptr<const Transform> follow,
	const VECTOR& localPos, float radius, int patrTag)
	:
	ColliderBase(SHAPE::SPHERE, tag, follow, patrTag),
	localPos_(localPos),
	radius_(radius)
{
}

ColliderSphere::~ColliderSphere(void){}

VECTOR ColliderSphere::GetPosPushBackAlongNormal(
	const MV1_COLL_RESULT_POLY& hitColPoly, int maxTryCnt, float pushDistance) const
{
	// コピー生成
	auto tmpTransform = std::make_shared<Transform>(*follow_.lock());
	ColliderSphere tmpCapsule = *this;
	tmpCapsule.SetFollow(tmpTransform);
	// 衝突補正処理
	int tryCnt = 0;
	while (tryCnt < maxTryCnt)
	{
		// カプセルと三角形の当たり判定
		if (!HitCheck_Sphere_Triangle(
			tmpCapsule.GetPos(),
			tmpCapsule.GetRadius(),
			hitColPoly.Position[0], hitColPoly.Position[1],
			hitColPoly.Position[2]))
		{
			break;
		}
		// 衝突していたら法線方向に押し戻し
		tmpTransform->pos =
			VAdd(tmpTransform->pos, VScale(hitColPoly.Normal, pushDistance));
		tryCnt++;
	}
	return tmpTransform->pos;
}

bool ColliderSphere::GetHitSpher_Model(
	std::weak_ptr<const ColliderModel> colliderModel, bool isExclude, bool isTarget) const
{
	bool ret = false;

	// モデルとカプセルの衝突判定
	auto hits = MV1CollCheck_Sphere(
		colliderModel.lock()->GetFollow().lock()->modelId, -1,
		GetPos(), GetRadius());

	// 衝突した複数のポリゴンと衝突回避するまで、位置を移動させる
	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hitPoly = hits.Dim[i];
		// 除外フレームは無視する
		if (isExclude 
			&& colliderModel.lock()->IsExcludeFrame(hitPoly.FrameIndex))
		{
			continue;
		}
		// 対象フレーム以外は無視する
		if (isTarget 
			&& !colliderModel.lock()->IsTargetFrame(hitPoly.FrameIndex))
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

void ColliderSphere::DrawDebug(int color)
{
	// デバッグ用に球体を描画
	DrawSphere3D(GetPos(), GetRadius(), DIV_NUM, color, color, false);
}
