#include "../../../Manager/ResourceManager.h"
#include "../../../Utility/AsoUtility.h"
#include "SkyDome.h"

SkyDome::SkyDome(void)
{
}

SkyDome::~SkyDome(void)
{
}

void SkyDome::Update(void)
{
	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-0.01f), AsoUtility::AXIS_Y));

	transform_.Update();
}

void SkyDome::Draw(void)
{
	SetUseLighting(FALSE);
	MV1DrawModel(transform_.modelId);
	SetUseLighting(TRUE);
}

void SkyDome::InitLoad(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::SKY_DOME));
}

void SkyDome::InitTransform(void)
{
	transform_.scl = { 100.0f, 100.0f, 100.0f };
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));
	transform_.pos = { 0.0f, 0.0f, 0.0f };
	transform_.Update();
}

void SkyDome::InitCollider(void)
{
}

void SkyDome::InitAnimation(void)
{
}

void SkyDome::InitPost(void)
{
	// Zバッファ無効(突き抜け対策)
	MV1SetUseZBuffer(transform_.modelId, false);
	MV1SetWriteZBuffer(transform_.modelId, false);
}
