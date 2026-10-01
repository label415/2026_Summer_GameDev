#include "../../../Manager/ResourceManager.h"
#include "../../../Utility/AsoUtility.h"
#include "SkyDome.h"

SkyDome::SkyDome(void){}

SkyDome::~SkyDome(void){}

void SkyDome::Update(void)
{
	// スカイドームをゆっくり回転させる
	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-0.01f), AsoUtility::AXIS_Y));

	// モデル制御更新
	transform_.Update();
}

void SkyDome::Draw(void)
{
	// ライティングを無効化して描画
	SetUseLighting(FALSE);
	MV1DrawModel(transform_.modelId);
	SetUseLighting(TRUE);
}

void SkyDome::InitLoad(void)
{
	// スカイドームモデルのロード
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::MODEL_SKY_DOME));
}

void SkyDome::InitTransform(void)
{
	// スカイドームの大きさ、回転、座標の初期化
	transform_.scl = { 100.0f, 100.0f, 100.0f };
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));
	transform_.pos = { 0.0f, 0.0f, 0.0f };
	transform_.Update();
}

void SkyDome::InitCollider(void){}

void SkyDome::InitAnimation(void){}

void SkyDome::InitPost(void)
{
	// Zバッファ無効(突き抜け対策)
	MV1SetUseZBuffer(transform_.modelId, false);
	MV1SetWriteZBuffer(transform_.modelId, false);
}
