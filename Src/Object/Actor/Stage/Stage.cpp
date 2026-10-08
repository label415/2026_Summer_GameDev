#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/CollisionManager.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../../Common/Collider/ColliderSphere.h"
#include "Stage.h"

Stage::Stage(void){}

Stage::~Stage(void){}

void Stage::Update(void)
{
	// 衝突判定
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
	MV1SetupCollInfo(transform_.modelId);

	// ステージモデルが反応する対象タグ
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::PLAYER,
		ColliderBase::TAG::ENEMY,
		ColliderBase::TAG::CAMERA,
		ColliderBase::TAG::GROUND,
		ColliderBase::TAG::PLAYER_WEPON,
		ColliderBase::TAG::ENEMY_WEPON
	};

	colModel_ = std::make_shared<ColliderModel>(
		ColliderBase::TAG::STAGE,
		targetTags,
		&transform_
	);

	for (const std::wstring& name : EXCLUDE_FRAME_NAMES)
	{
		colModel_->AddExcludeFrameIds(name);
	}

	for (const std::wstring& name : TARGET_FRAME_NAMES)
	{
		colModel_->AddTargetFrameIds(name);
	}

	ownColliders_[static_cast<int>(ColliderBase::SHAPE::MODEL)].push_back(colModel_);
	CollisionManager::GetInstance().AddCollider(colModel_);
}

void Stage::InitAnimation(void){}

void Stage::InitPost(void){}

void Stage::UpdateHitCollider(void)
{
	// 前フレームで半透明化したフレームの不透明度をリセット
	for (auto& frameIdx : frameOpacityRate_) {
		MV1SetFrameOpacityRate(transform_.modelId, frameIdx, 1.0f);
	}
	frameOpacityRate_.clear();

	if (!colModel_ || !colModel_->GetIsCollier()) return;

	// カメラ等と接触した遮蔽フレームの半透明化処理
	for (const auto& hit : colModel_->GetCollisionResults())
	{
		if (hit.isHit_ && hit.targetTag_ == ColliderBase::TAG::CAMERA)
		{
		}
	}
}

void Stage::RateFrameIds(const std::wstring& name)
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

bool Stage::IsRateFrame(int frameIdx) const
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
