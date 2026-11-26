#include <algorithm>
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../Common/Collider/ColliderBase.h"
#include "../Common/EffectController.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
	: resMng_(ResourceManager::GetInstance())
	, scnMng_(SceneManager::GetInstance())
	, transform_(Transform())
	, isAlive_(true)
{
	ownColliders_.clear();
}

ActorBase::~ActorBase(void){}

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
	InitLoad();
}

void ActorBase::Release(void)
{
	transform_.Release();
}

void ActorBase::Collision(void)
{
	UpdateHitCollider();
}
