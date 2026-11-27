#include <algorithm>
#include "../../Manager/Common/ResourceManager.h"
#include "../../Manager/Common/SceneManager.h"
#include "../Common/Collider/ColliderBase.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
	: resMng_(ResourceManager::GetInstance())
	, scnMng_(SceneManager::GetInstance())
	, transform_(Transform())
	, isAlive_(true)
{
	// 所有コライダーマップを空に初期化
	ownColliders_.clear();
}

void ActorBase::LoadEnd(void)
{
	// Transform初期化
	InitTransform();

	// 衝突判定の初期化
	InitCollider();

	// アニメーションの初期化
	InitAnimation();

	// 初期化後の個別処理
	InitPost();
}

void ActorBase::Draw(void)
{
	// モデルハンドルがあるとき描画
	if (transform_.modelId != -1)
	{
		MV1DrawModel(transform_.modelId);
	}
}

const std::vector<std::shared_ptr<ColliderBase>> ActorBase::GetOwnCollider(int key) const
{
	// 自身のコライダーが存在しているとき、
	// コライダーを取得
	if (ownColliders_.count(key) == 0)
	{
		return std::vector<std::shared_ptr<ColliderBase>>();
	}
	return ownColliders_.at(key);
}

void ActorBase::Load(void)
{
	// 派生クラスごとのリソース読み込みを実行
	InitLoad();
}

void ActorBase::Release(void)
{
	// 保持しているモデルリソースを解放
	transform_.Release();
}

void ActorBase::Collision(void)
{
	// 衝突したコライダーの結果を更新
	UpdateHitCollider();
}
