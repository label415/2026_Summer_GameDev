#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
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
	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダ
	ColliderModel* colModel =
		new ColliderModel(ColliderBase::TAG::STAGE, &transform_);

	//除外フレーム格納処理
	for (const std::wstring& name : EXCLUDE_FRAME_NAMES)
	{
		colModel->AddExcludeFrameIds(name);
	}

	//対象フレーム格納処理
	for (const std::wstring& name : TARGET_FRAME_NAMES)
	{
		colModel->AddTargetFrameIds(name);
	}

	// 自身のコライダに登録
	std::vector<ColliderBase*> colModels;
	colModels.push_back(colModel);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::MODEL), colModels);
}

void Stage::InitAnimation(void){}

void Stage::InitPost(void){}

void Stage::Collision(void)
{
	// 対象フレームの不透明度率をリセット
	for (auto& frameIdx : frameOpacityRate_) {
		MV1SetFrameOpacityRate(transform_.modelId, frameIdx, 1.0f);
	}

	// 対象フレームの不透明度率をクリア
	frameOpacityRate_.clear();

	// 衝突判定
	for (const auto& hitCol : hitColliders_)
	{
		for(const auto& i : hitCol.second)
		{
			// モデル以外は処理を飛ばす
			if (i->GetShape() != ColliderBase::SHAPE::SPHERE) continue;

			//派生クラスへキャスト
			const ColliderSphere* colliderSphere =
				dynamic_cast<const ColliderSphere*>(i);

			if (colliderSphere == nullptr)continue;

			auto hits = MV1CollCheck_Sphere(
				transform_.modelId,
				-1,
				colliderSphere->GetPos(),
				colliderSphere->GetRadius());

			// 検出した地面ポリゴン情報の数だけループ
			for (int i = 0; i < hits.HitNum; i++)
			{
				const auto& hit = hits.Dim[i];

				for (const std::wstring& name : TARGET_FRAME_NAMES)
				{
					RateFrameIds(name);
				}

				if (IsRateFrame(hit.FrameIndex))continue;

				MV1SetFrameOpacityRate(transform_.modelId, hit.FrameIndex, 0.3f);

				frameOpacityRate_.emplace_back(hit.FrameIndex);

			}
			// 検出した地面ポリゴン情報の後始末
			MV1CollResultPolyDimTerminate(hits);
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
