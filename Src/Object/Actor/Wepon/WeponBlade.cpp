#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "WeponBlade.h"
WeponBlade::WeponBlade(const Transform& followTransform, int followFrameId)
	:
	WeponBase(followTransform, followFrameId)
{
}
WeponBlade::~WeponBlade(void)
{
}
void WeponBlade::Update(void)
{
	WeponBase::Update();
}
void WeponBlade::InitLoad(void)
{
	// ÉÇÉfÉãÇÃÉçÅ[Éh
	transform_.SetModel(
		resMng_.Load(ResourceManager::SRC::WEAPON_BLADE).handleId_);
}
void WeponBlade::InitTransform(void)
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
void WeponBlade::InitCollider(void)
{
}
void WeponBlade::InitAnimation(void)
{
}
void WeponBlade::InitPost(void)
{
}