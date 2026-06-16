#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "WeponBracelet.h"
WeponBracelet::WeponBracelet(const Transform& followTransform, int followFrameId)
	:
	WeponBase(followTransform, followFrameId)
{
	isAlive_ = false;
}

WeponBracelet::~WeponBracelet(void)
{
}

void WeponBracelet::Update(void)
{
	WeponBase::Update();
}

void WeponBracelet::InitLoad(void)
{
	// ÉÇÉfÉãÇÃÉçÅ[Éh
	transform_.SetModel(
		resMng_.Load(ResourceManager::SRC::WEAPON_BLADE).handleId_);
}

void WeponBracelet::InitTransform(void)
{
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, SCALE);
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal = Quaternion();
	transform_.pos = AsoUtility::VECTOR_ZERO;
	localPos_ = { -2.0f, 0.0f, -3.0f };
	localRot_ = {
	AsoUtility::Deg2RadF(0.0f),
	AsoUtility::Deg2RadF(0.0f),
	AsoUtility::Deg2RadF(-90.0f)
	};
}

void WeponBracelet::InitCollider(void)
{

}

void WeponBracelet::InitAnimation(void)
{
}

void WeponBracelet::InitPost(void)
{
}

void WeponBracelet::SetCollider(void)
{
	isAlive_ = true;
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::PLAYER_WEPON, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);

	std::vector<ColliderBase*> colCapsules;
	colCapsules.push_back(colCapsule);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsules);
}

void WeponBracelet::ClearCollider(void)
{
	isAlive_ = false;
	ownColliders_.erase(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
}
