#include <algorithm>
#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Object/Common/Collider/ColliderModel.h"
#include "../Object/Common/Collider/ColliderLine.h"
#include "../Object/Common/Collider/ColliderSphere.h"
#include "../Object/Common/Transform.h"
#include "CollisionManager.h"

void CollisionManager::AddCollider(std::shared_ptr<ColliderBase> col, CollisionCallback callback)
{
	colliders_.push_back({ col, callback });
}

void CollisionManager::Release(void)
{
	colliders_.clear();
}

void CollisionManager::DrawDebug(void)
{
	for (auto& entry : colliders_)
	{
		if (entry.collider)
		{
			entry.collider->Draw();
		}
	}
}

void CollisionManager::Update(void)
{
	if (colliders_.empty()) return;

	// îjä¸çœÇ›ÇÃéQè∆ÇçÌèú
	colliders_.erase(
		std::remove_if(colliders_.begin(), colliders_.end(),
			[](const ColliderEntry& e) {
				return !e.collider || e.collider->GetFollow().expired();
			}),
		colliders_.end()
	);

	// ëçìñÇΩÇËîªíË
	for (size_t i = 0; i < colliders_.size(); ++i)
	{
		for (size_t j = i + 1; j < colliders_.size(); ++j)
		{
			if (!IsCheckColliderTag(
				colliders_[i].collider->GetTag(),
				colliders_[j].collider->GetTag()))
			{
				continue;
			}

			ResolveCollision(colliders_[i], colliders_[j]);
		}
	}
}

void CollisionManager::ResolveCollision(ColliderEntry& entryA, ColliderEntry& entryB)
{
	auto& colA = entryA.collider;
	auto& colB = entryB.collider;

	auto shapeA = colA->GetShape();
	auto shapeB = colB->GetShape();

	if (shapeA == ColliderBase::SHAPE::CAPSULE && shapeB == ColliderBase::SHAPE::CAPSULE)
	{
		auto capA = std::static_pointer_cast<ColliderCapsule>(colA);
		auto capB = std::static_pointer_cast<ColliderCapsule>(colB);

		if (CollisionUtility::IsHit(*capA, *capB))
		{
			// ÉLÉÉÉâìØémÇÃâüÇµçáÇ¢
			VECTOR pushA = CollisionUtility::CalcPushCapsuleCapsule(*capA, *capB);
			if (entryA.onCollision) entryA.onCollision(colA, colB, { true, pushA });
			if (entryB.onCollision) entryB.onCollision(colB, colA, { true, VScale(pushA, -1.0f) });
		}
		return;
	}

	auto HandleCapsuleModel = [](ColliderEntry& capEnt, ColliderEntry& mdlEnt) {
		auto cap = std::static_pointer_cast<ColliderCapsule>(capEnt.collider);
		auto mdl = std::static_pointer_cast<ColliderModel>(mdlEnt.collider);

		MV1_COLL_RESULT_POLY_DIM hits;
		if (CollisionUtility::IsHit(*cap, *mdl, hits, true, false))
		{
			VECTOR push = CollisionUtility::CalcPushCapsuleModel(*cap, *mdl, hits, 20, 1.0f, true, false);
			if (capEnt.onCollision)
			{
				capEnt.onCollision(capEnt.collider, mdlEnt.collider, { true, push });
			}
		}
		MV1CollResultPolyDimTerminate(hits);
		};

	if (shapeA == ColliderBase::SHAPE::CAPSULE && shapeB == ColliderBase::SHAPE::MODEL)
	{
		HandleCapsuleModel(entryA, entryB);
		return;
	}
	if (shapeB == ColliderBase::SHAPE::CAPSULE && shapeA == ColliderBase::SHAPE::MODEL)
	{
		HandleCapsuleModel(entryB, entryA);
		return;
	}

	auto HandleLineModel = [](ColliderEntry& lineEnt, ColliderEntry& mdlEnt) {
		auto line = std::static_pointer_cast<ColliderLine>(lineEnt.collider);
		auto mdl = std::static_pointer_cast<ColliderModel>(mdlEnt.collider);

		MV1_COLL_RESULT_POLY_DIM hits;
		if (CollisionUtility::IsHit(*line, *mdl, hits, false, true))
		{
			float currentY = line->GetFollow().lock() ? line->GetFollow().lock()->pos.y : 0.0f;
			VECTOR pushUp = CollisionUtility::CalcPushUpLineModel(currentY, *mdl, hits, 2.0f, false, true);
			if (lineEnt.onCollision)
			{
				lineEnt.onCollision(lineEnt.collider, mdlEnt.collider, { true, pushUp });
			}
		}
		MV1CollResultPolyDimTerminate(hits);
		};

	if (shapeA == ColliderBase::SHAPE::LINE && shapeB == ColliderBase::SHAPE::MODEL)
	{
		HandleLineModel(entryA, entryB);
		return;
	}
	if (shapeB == ColliderBase::SHAPE::LINE && shapeA == ColliderBase::SHAPE::MODEL)
	{
		HandleLineModel(entryB, entryA);
		return;
	}

	if ((shapeA == ColliderBase::SHAPE::SPHERE && shapeB == ColliderBase::SHAPE::CAPSULE) ||
		(shapeB == ColliderBase::SHAPE::SPHERE && shapeA == ColliderBase::SHAPE::CAPSULE))
	{
		auto& sphEnt = (shapeA == ColliderBase::SHAPE::SPHERE) ? entryA : entryB;
		auto& capEnt = (shapeA == ColliderBase::SHAPE::SPHERE) ? entryB : entryA;

		auto sph = std::static_pointer_cast<ColliderSphere>(sphEnt.collider);
		auto cap = std::static_pointer_cast<ColliderCapsule>(capEnt.collider);

		if (HitCheck_Sphere_Capsule(
			sph->GetPos(), sph->GetRadius(),
			cap->GetPosTop(), cap->GetPosDown(), cap->GetRadius()))
		{
			if (capEnt.onCollision) capEnt.onCollision(capEnt.collider, sphEnt.collider, { true, {0,0,0} });
			if (sphEnt.onCollision) sphEnt.onCollision(sphEnt.collider, capEnt.collider, { true, {0,0,0} });
		}
		return;
	}
}

bool CollisionManager::IsCheckColliderTag(ColliderBase::TAG tagA, ColliderBase::TAG tagB) const
{
	using TAG = ColliderBase::TAG;

	if ((tagA == TAG::PLAYER && tagB == TAG::ENEMY)|| (tagA == TAG::ENEMY && tagB == TAG::PLAYER)) return true;
	if ((tagA == TAG::PLAYER && tagB == TAG::STAGE) || (tagA == TAG::STAGE && tagB == TAG::PLAYER)) return true;
	if ((tagA == TAG::ENEMY && tagB == TAG::STAGE) || (tagA == TAG::STAGE && tagB == TAG::ENEMY)) return true;
	if ((tagA == TAG::GROUND && tagB == TAG::STAGE) || (tagA == TAG::STAGE && tagB == TAG::GROUND)) return true;
	if ((tagA == TAG::CAMERA && tagB == TAG::STAGE) || (tagA == TAG::STAGE && tagB == TAG::CAMERA)) return true;
	if ((tagA == TAG::ENEMY_WEPON && tagB == TAG::STAGE) || (tagA == TAG::STAGE && tagB == TAG::ENEMY_WEPON)) return true;
	if ((tagA == TAG::PLAYER && tagB == TAG::ENEMY_WEPON) || (tagA == TAG::ENEMY_WEPON && tagB == TAG::PLAYER)) return true;
	if ((tagA == TAG::ENEMY && tagB == TAG::PLAYER_WEPON) || (tagA == TAG::PLAYER_WEPON && tagB == TAG::ENEMY)) return true;

	return false;
}