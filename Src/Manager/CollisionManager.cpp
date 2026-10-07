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
		collider->Draw();
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
				return !e || e.get()->GetFollow().expired();
			}),
		colliders_.end()
	);

	// 総当たり判定
	for (size_t i = 0; i < colliders_.size(); ++i)
	{
		for (size_t j = i + 1; j < colliders_.size(); ++j)
		{
			if (!IsCheckColliderTag(
				colliders_[i]->GetTag(),
				colliders_[j]->GetTag()))
			{
				continue;
			}

			// 衝突判定実行
			ResolveCollision(colliders_[i], colliders_[j]);
		}
	}
}

void CollisionManager::ResolveCollision(
	std::weak_ptr<ColliderBase> colliderA,
	std::weak_ptr<ColliderBase> colliderB)
{
	auto colA = colliderA.lock();
	auto colB = colliderB.lock();

	if (colA->GetShape() == ColliderBase::SHAPE::CAPSULE 
		&& colB->GetShape() == ColliderBase::SHAPE::CAPSULE)
	{
		auto capA = std::static_pointer_cast<ColliderCapsule>(colA);
		auto capB = std::static_pointer_cast<ColliderCapsule>(colB);

		if (CollisionUtility::IsHit(*capA, *capB))
		{
			if (capA->GetTag() == ColliderBase::TAG::PLAYER
				&& capB->GetTag() == ColliderBase::TAG::ENEMY)
			{

			}
		}
	}

	if (colA->GetShape() == ColliderBase::SHAPE::CAPSULE 
		&& colB->GetShape() == ColliderBase::SHAPE::SPHERE)
	{
		auto capA = std::static_pointer_cast<ColliderCapsule>(colA);
		auto capB = std::static_pointer_cast<ColliderSphere>(colB);

		if (CollisionUtility::IsHit(*capA, *capB))
		{

		}
	}

	if (colA->GetShape() == ColliderBase::SHAPE::CAPSULE 
		&& colB->GetShape() == ColliderBase::SHAPE::MODEL)
	{
		auto capA = std::static_pointer_cast<ColliderCapsule>(colA);
		auto capB = std::static_pointer_cast<ColliderSphere>(colB);

		if (CollisionUtility::IsHit(*capA, *capB))
		{

		}
	}

	if (colA->GetShape() == ColliderBase::SHAPE::MODEL 
		&& colB->GetShape() == ColliderBase::SHAPE::SPHERE)
	{
		auto capA = std::static_pointer_cast<ColliderModel>(colA);
		auto capB = std::static_pointer_cast<ColliderSphere>(colB);

		if (CollisionUtility::IsHit(*capB, *capA))
		{

		}
	}

	if (colA->GetShape() == ColliderBase::SHAPE::MODEL 
		&& colB->GetShape() == ColliderBase::SHAPE::LINE)
	{
		auto capA = std::static_pointer_cast<ColliderModel>(colA);
		auto capB = std::static_pointer_cast<ColliderLine>(colB);

		if (CollisionUtility::IsHit(*capB, *capA))
		{

		}
	}
}

bool CollisionManager::IsCheckColliderTag(
	ColliderBase::TAG tagA, ColliderBase::TAG tagB) const
{
	using TAG = ColliderBase::TAG;

	// 順不同で一致するか判定するラムダ式
	auto match = [tagA, tagB](TAG a, TAG b) {
		return (tagA == a && tagB == b) || (tagA == b && tagB == a);
		};

	if (match(TAG::PLAYER, TAG::ENEMY)) return true;
	if (match(TAG::PLAYER, TAG::STAGE)) return true;
	if (match(TAG::ENEMY, TAG::STAGE)) return true;
	if (match(TAG::GROUND, TAG::STAGE)) return true;
	if (match(TAG::CAMERA, TAG::STAGE)) return true;
	if (match(TAG::ENEMY_WEPON, TAG::STAGE)) return true;
	if (match(TAG::PLAYER, TAG::ENEMY_WEPON)) return true;
	if (match(TAG::ENEMY, TAG::PLAYER_WEPON)) return true;

	return false;
}
