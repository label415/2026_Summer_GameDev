#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../../Common/Collider/ColliderSphere.h"
#include "Stage.h"

Stage::Stage(void)
{
}

Stage::~Stage(void)
{
}

void Stage::Update(void)
{
	Collision();
}

void Stage::Draw(void)
{
#ifdef _DEBUG
	// 所有しているコライダの描画
	for (const auto& own : ownColliders_)
	{
		own.second->Draw();
	}
#endif // _DEBUG

	if (transform_.modelId != -1)
	{
		MV1DrawModel(transform_.modelId);
	}
}

void Stage::InitLoad(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::MAIN_STAGE));
}

void Stage::InitTransform(void)
{
	transform_.scl = { 1.0f, 1.0f, 1.0f };
	transform_.pos = { 0.0f, -100.0f, 0.0f };
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
	for (const std::string& name : EXCLUDE_FRAME_NAMES)
	{
		colModel->AddExcludeFrameIds(name);
	}

	//対象フレーム格納処理
	for (const std::string& name : TARGET_FRAME_NAMES)
	{
		colModel->AddTargetFrameIds(name);
	}

	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::MODEL), colModel);
}

void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
}

void Stage::Collision(void)
{
	for (auto& frameIdx : frameOpacityRate_) {
		MV1SetFrameOpacityRate(transform_.modelId, frameIdx, 1.0f);
	}

	frameOpacityRate_.clear();

	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外は処理を飛ばす
		if (hitCol->GetShape() != ColliderBase::SHAPE::SPHERE) continue;

		//派生クラスへキャスト
		const ColliderSphere* colliderSphere =
			dynamic_cast<const ColliderSphere*>(hitCol);

		if (colliderSphere == nullptr)continue;

		auto hits = MV1CollCheck_Sphere(
			transform_.modelId,
			-1,
			colliderSphere->GetPos(),
			colliderSphere->GetRadius());

		for (int i = 0; i < hits.HitNum; i++)
		{
			const auto& hit = hits.Dim[i];

			for (const std::string& name : TARGET_FRAME_NAMES)
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

void Stage::RateFrameIds(const std::string& name)
{
	// フレーム数を取得
	int num = MV1GetFrameNum(transform_.modelId);
	for (int i = 0; i < num; i++)
	{
		// フレーム名称を取得
		std::string frameName = MV1GetFrameName(transform_.modelId, i);
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
