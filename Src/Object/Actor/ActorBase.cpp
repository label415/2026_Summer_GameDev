#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
	: 
	resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	transform_()
{
}

ActorBase::~ActorBase(void)
{
}

void ActorBase::Init(void)
{

	// リソースロード
	InitLoad();

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
#endif // _DEBUG

	if (transform_.modelId != -1)
	{
		MV1DrawModel(transform_.modelId);
	}
}

void ActorBase::Release(void)
{
	transform_.Release();

	for (const auto& own : ownColliders_)
	{
		for (const auto& collider : own.second) {
			delete collider;
		}
	}
	ownColliders_.clear();
}

const Transform& ActorBase::GetTransform(void) const
{
	return transform_;
}

const std::vector<ColliderBase*> ActorBase::GetOwnCollider(int key) const
{
	if (ownColliders_.count(key) == 0)
	{
		return std::vector<ColliderBase*>();
	}
	return ownColliders_.at(key);
}

void ActorBase::AddHitCollider(int shape, const std::vector<ColliderBase*> hitCollider)
{
	for (const auto& c : hitColliders_)
	{
			if (c.second == hitCollider)
			{
				return;
			}
	}
	hitColliders_.emplace(shape, hitCollider);
}

void ActorBase::ClearHitCollider(void)
{
	hitColliders_.clear();
}
