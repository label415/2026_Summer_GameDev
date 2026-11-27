#include "../../../Manager/Common/ResourceManager.h"
#include "../../../Manager/Common/SceneManager.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../../Common/Collider/ColliderSphere.h"
#include "Stage.h"

void Stage::Update(void)
{
	// ステージの衝突応答処理を実行
	Collision();
}

void Stage::Draw(void)
{
#ifdef _DEBUG
	// 所有しているコライダの描画
	for (const auto& own : ownColliders_){
		for (const auto& collider : own.second){
			if (collider)
			{
				collider->Draw();
			}
		}
	}
#endif // _DEBUG

	// モデル描画
	if (transform_.modelId != -1)
	{
		MV1DrawModel(transform_.modelId);
	}
}

void Stage::InitLoad(void)
{
	// モデルのロード
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::MODEL_MAIN_STAGE));
}

void Stage::InitTransform(void)
{
	// 大きさ、回転、座標の初期化
	transform_.scl = STAGE_SCALE;
	transform_.pos = STAGE_POS;
	transform_.Update();
}

void Stage::InitCollider(void)
{
	// DxLib側のモデル衝突判定情報を構築
	MV1SetupCollInfo(transform_.modelId);

	// ステージモデルが反応する対象タグ
	std::vector<ColliderBase::TAG> targetTags = { ColliderBase::TAG::NONE };

	// モデルコライダーを生成
	auto colModel_ = std::make_shared<ColliderModel>(
		ColliderBase::TAG::STAGE,
		targetTags,
		&transform_
	);

	// 衝突判定から除外するフレーム名を登録
	for (const std::wstring& name : EXCLUDE_FRAME_NAMES)
	{
		colModel_->AddExcludeFrameId(name);
	}

	// 衝突判定対象とするフレーム名を登録
	for (const std::wstring& name : TARGET_FRAME_NAMES)
	{
		colModel_->AddTargetFrameId(name);
	}

	// モデル形状として所有コライダーへ格納
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::MODEL)].push_back(colModel_);
}

void Stage::AddOpacityFrameIds(const std::wstring& name)
{
	// フレーム数を取得
	int num = MV1GetFrameNum(transform_.modelId);
	for (int i = 0; i < num; i++)
	{
		// フレーム名称を取得
		std::wstring frameName = MV1GetFrameName(transform_.modelId, i);
		if (frameName.find(name) != std::string::npos)
		{
			// 除外フレームに追加
			frameOpacityRate_.emplace_back(i);
		}
	}
}

bool Stage::IsOpacityFrame(int frameIdx) const
{
	// 除外判定
	if (std::find(
		frameOpacityRate_.begin(),
		frameOpacityRate_.end(),
		frameIdx) != frameOpacityRate_.end())
	{
		// 除外に該当する
		return true;
	}
	return false;
}
