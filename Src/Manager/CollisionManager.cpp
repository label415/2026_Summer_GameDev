#include <algorithm>
#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Object/Common/Collider/ColliderModel.h"
#include "../Object/Common/Collider/ColliderLine.h"
#include "../Object/Common/Collider/ColliderSphere.h"
#include "../Object/Common/Transform.h"
#include "CollisionManager.h"

void CollisionManager::AddCollider(std::shared_ptr<ColliderBase> col)
{
	colliders_.push_back(col);
}

void CollisionManager::Release(void)
{
	colliders_.clear();
}

void CollisionManager::DrawDebug(void)
{
#ifdef _DEBUG
	for (auto& collider : colliders_)
	{
		if (collider && collider->GetIsCollier())
		{
			collider->Draw();
		}
	}
#endif
}

void CollisionManager::Update(void)
{
	if (colliders_.empty()) return;

	// 破棄済みの参照を削除
	colliders_.erase(
		std::remove_if(colliders_.begin(), colliders_.end(),
			[](const std::shared_ptr<ColliderBase>& e) {
				return !e || e->GetFollow().expired();
			}),
		colliders_.end()
	);

	// 前回の衝突結果を全コライダークリア
	for (auto& col : colliders_)
	{
		col->ClearCollisionResults();
	}

	// 総当たり判定
	for (size_t i = 0; i < colliders_.size(); ++i)
	{
		for (size_t j = i + 1; j < colliders_.size(); ++j)
		{
			auto& colA = colliders_[i];
			auto& colB = colliders_[j];

			// コライダーが無効化されている場合はスキップ
			if (!colA->GetIsCollier() 
				|| !colB->GetIsCollier()) continue;

			// お互いが対象タグに含まれているか確認
			if (!CanCollide(*colA, *colB))
			{
				continue;
			}

			// 判定および結果の格納
			ResolveCollision(colA, colB);
		}
	}
}

bool CollisionManager::CanCollide(
	const ColliderBase& a,
	const ColliderBase& b) const
{
	// どちらか一方が相手のタグをターゲットとしている場合に判定を実行
	return a.IsTargetTag(b.GetTag()) || b.IsTargetTag(a.GetTag());
}

void CollisionManager::ResolveCollision(
	std::shared_ptr<ColliderBase> colA,
	std::shared_ptr<ColliderBase> colB)
{
	using SHAPE = ColliderBase::SHAPE;

	bool isHit = false;
	VECTOR pushA = AsoUtility::VECTOR_ZERO;
	VECTOR pushB = AsoUtility::VECTOR_ZERO;

	if (colA->GetShape() == SHAPE::CAPSULE
		&& colB->GetShape() == SHAPE::CAPSULE)
	{
		auto capA = std::static_pointer_cast<ColliderCapsule>(colA);
		auto capB = std::static_pointer_cast<ColliderCapsule>(colB);

		if (CollisionUtility::IsHit(*capA, *capB))
		{
			isHit = true;
			// 押し出しベクトルの計算
			pushA = CollisionUtility::CalcPushCapsuleCapsule(*capA, *capB);
			pushB = VScale(pushA, -1.0f);
		}
	}
	else if ((colA->GetShape() == SHAPE::CAPSULE 
		&& colB->GetShape() == SHAPE::SPHERE) 
		|| (colA->GetShape() == SHAPE::SPHERE 
			&& colB->GetShape() == SHAPE::CAPSULE))
	{
		auto cap = (colA->GetShape() == SHAPE::CAPSULE)
			? std::static_pointer_cast<ColliderCapsule>(colA)
			: std::static_pointer_cast<ColliderCapsule>(colB);
		auto sph = (colA->GetShape() == SHAPE::SPHERE)
			? std::static_pointer_cast<ColliderSphere>(colA)
			: std::static_pointer_cast<ColliderSphere>(colB);

		if (CollisionUtility::IsHit(*cap, *sph))
		{
			isHit = true;
		}
	}
	else if ((colA->GetShape() == SHAPE::CAPSULE
		&& colB->GetShape() == SHAPE::MODEL)
		|| (colA->GetShape() == SHAPE::MODEL
			&& colB->GetShape() == SHAPE::CAPSULE))
	{
		auto cap = (colA->GetShape() == SHAPE::CAPSULE)
			? std::static_pointer_cast<ColliderCapsule>(colA)
			: std::static_pointer_cast<ColliderCapsule>(colB);
		auto mdl = (colA->GetShape() == SHAPE::MODEL)
			? std::static_pointer_cast<ColliderModel>(colA)
			: std::static_pointer_cast<ColliderModel>(colB);

		if (CollisionUtility::IsHit(*cap, *mdl))
		{
			isHit = true;
		}
	}
	else if ((colA->GetShape() == SHAPE::SPHERE
		&& colB->GetShape() == SHAPE::MODEL)
		|| (colA->GetShape() == SHAPE::MODEL
			&& colB->GetShape() == SHAPE::SPHERE))
	{
		auto sph = (colA->GetShape() == SHAPE::SPHERE)
			? std::static_pointer_cast<ColliderSphere>(colA)
			: std::static_pointer_cast<ColliderSphere>(colB);
		auto mdl = (colA->GetShape() == SHAPE::MODEL)
			? std::static_pointer_cast<ColliderModel>(colA)
			: std::static_pointer_cast<ColliderModel>(colB);

		if (CollisionUtility::IsHit(*sph, *mdl))
		{
			isHit = true;
		}
	}
	else if ((colA->GetShape() == SHAPE::LINE
		&& colB->GetShape() == SHAPE::MODEL)
		|| (colA->GetShape() == SHAPE::MODEL
			&& colB->GetShape() == SHAPE::LINE))
	{
		auto line = (colA->GetShape() == SHAPE::LINE)
			? std::static_pointer_cast<ColliderLine>(colA)
			: std::static_pointer_cast<ColliderLine>(colB);
		auto mdl = (colA->GetShape() == SHAPE::MODEL)
			? std::static_pointer_cast<ColliderModel>(colA)
			: std::static_pointer_cast<ColliderModel>(colB);

		if (CollisionUtility::IsHit(*line, *mdl))
		{
			isHit = true;
		}
	}

	if (isHit)
	{
		// 壁遮蔽フラグの算出
		bool blocked = false;
		auto followA = colA->GetFollow().lock();
		auto followB = colB->GetFollow().lock();
		if (followA && followB)
		{
			blocked = CheckWallOcclusion(followA->pos, followB->rot);
		}

		// AがBを判定対象としている場合、Aに結果を格納
		if (colA->IsTargetTag(colB->GetTag()))
		{
			ColliderBase::HitInfo infoA;
			infoA.isHit_ = true;
			infoA.targetTag_ = colB->GetTag();
			infoA.pushVector_ = pushA;
			infoA.pushVectorDir_ =
				!AsoUtility::EqualsVZero(pushA) ? VNorm(pushA) : AsoUtility::VECTOR_ZERO;
			infoA.isBlocked = blocked;
			colA->AddCollisionResult(infoA);
		}

		// BがAを判定対象としている場合、Bに結果を格納
		if (colB->IsTargetTag(colA->GetTag()))
		{
			ColliderBase::HitInfo infoB;
			infoB.isHit_ = true;
			infoB.targetTag_ = colA->GetTag();
			infoB.pushVector_ = pushB;
			infoB.pushVectorDir_ =
				!AsoUtility::EqualsVZero(pushB) ? VNorm(pushB) : AsoUtility::VECTOR_ZERO;
			infoB.isBlocked = blocked;
			colB->AddCollisionResult(infoB);
		}
	}
}

bool CollisionManager::CheckWallOcclusion(const VECTOR& start, const VECTOR& end) const
{
	// プレイヤーと敵の間の直線を結ぶ一時的な線コライダーを生成
	std::vector<ColliderBase::TAG> targetTags = { ColliderBase::TAG::STAGE };

	ColliderLine tempLine(
		ColliderBase::TAG::NONE,
		targetTags,
		std::weak_ptr<const Transform>(),
		start,
		end
	);

	// ステージコライダーを探して線コライダーとの交差を判定
	for (const auto& col : colliders_)
	{
		if (col->GetShape() == ColliderBase::SHAPE::MODEL
			&& col->GetTag() == ColliderBase::TAG::STAGE)
		{
			auto modelCol = std::static_pointer_cast<ColliderModel>(col);
			if (!modelCol || !modelCol->GetIsCollier()) continue;

			// CollisionUtility の線分 vs モデル交差判定
			// isExclude = false, isTarget = true
			if (CollisionUtility::IsHit(tempLine, *modelCol, false, true))
			{
				return true;
			}
		}
	}
	return false;
}