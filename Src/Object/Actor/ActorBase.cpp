#include <algorithm>
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
	: 
	resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	transform_()
{
	isAlive_ = true;
}

ActorBase::~ActorBase(void){}

void ActorBase::Load(void)
{
	// リソースロード
	InitLoad();
}

void ActorBase::Init(void)
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
#ifdef _DEBUG
	// 所有しているコライダの描画
	for (const auto& own : ownColliders_)
	{
		for (const auto& collider : own.second) {
			collider->Draw();
		}
	}
#endif

	if (transform_.modelId != -1)
	{
		MV1DrawModel(transform_.modelId);
	}
}

void ActorBase::Release(void)
{
	transform_.Release();
}

const Transform& ActorBase::GetTransform(void) const
{
	return transform_;
}

void ActorBase::Collision(void)
{
	UpdateHitCollider();
}
