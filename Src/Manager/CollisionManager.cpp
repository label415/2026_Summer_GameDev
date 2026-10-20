#include <algorithm>
#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Object/Common/Collider/ColliderModel.h"
#include "../Object/Common/Collider/ColliderLine.h"
#include "../Object/Common/Collider/ColliderSphere.h"
#include "../Object/Common/Transform.h"
#include "CollisionManager.h"

void CollisionManager::AddCollider(const std::shared_ptr<ColliderBase>& col)
{
	colliders_.push_back(col);
}

void CollisionManager::Release(void)
{
	colliders_.clear();
}

void CollisionManager::DrawDebug(void)
{
#ifdef _DEBUG
	for (auto& collider : colliders_)
	{
		if (collider.lock() && collider.lock()->GetIsCollier())
		{
			collider.lock()->Draw();
		}
	}
#endif
}

void CollisionManager::Update(void)
{
	// 有効期限切れコライダーを配列から除外する
	colliders_.erase(
		std::remove_if(colliders_.begin(), colliders_.end(),
			[](const std::weak_ptr<ColliderBase>& wp) {
				return wp.expired();
			}),
		colliders_.end()
	);

	if (colliders_.empty()) return;

	// 前回の衝突結果を全コライダークリア
	for (auto& col : colliders_) { col.lock()->ClearHitCollider(); }

	// 総当たり判定
	for (size_t i = 0; i < colliders_.size(); ++i)
	{
		for (size_t j = i + 1; j < colliders_.size(); ++j)
		{
			auto colA = colliders_[i].lock();
			auto colB = colliders_[j].lock();

			// コライダーが無効化されている場合はスキップ
			if (!colA->GetIsCollier()
				|| !colB->GetIsCollier()) continue;

			// お互いが対象タグに含まれているか確認
			if (!CanCollide(*colA, *colB))continue;

			// 判定および結果の格納
			ResolveCollision(colA, colB);
		}
	}
}

bool CollisionManager::CanCollide(
	const ColliderBase& a,
	const ColliderBase& b) const
{
	// どちらかの衝突対象となっているとき実行
	return a.IsTargetTag(b.GetTag())
		|| b.IsTargetTag(a.GetTag());
}

void CollisionManager::ResolveCollision(
	const std::shared_ptr<ColliderBase>& argA,
	const std::shared_ptr<ColliderBase>& argB)
{
	auto colA = argA;
	auto colB = argB;

	// 形状によるソート（スワップ）
	if (colA->GetShape() > colB->GetShape())
	{
		std::swap(colA, colB);
	}

	auto shape1 = colA->GetShape();
	auto shape2 = colB->GetShape();
	bool isHit = false;

	// カプセル vs カプセル
	if (shape1 == ColliderBase::SHAPE::CAPSULE
		&& shape2 == ColliderBase::SHAPE::CAPSULE)
	{
		auto cap1 = std::static_pointer_cast<ColliderCapsule>(colA);
		auto cap2 = std::static_pointer_cast<ColliderCapsule>(colB);
		isHit = CollisionUtility::IsHit(*cap1, *cap2);
	}
	// スフィア vs カプセル
	else if (shape1 == ColliderBase::SHAPE::SPHERE
		&& shape2 == ColliderBase::SHAPE::CAPSULE)
	{
		auto sphere = std::static_pointer_cast<ColliderSphere>(colA);
		auto cap = std::static_pointer_cast<ColliderCapsule>(colB);
		isHit = CollisionUtility::IsHit(*cap, *sphere);
	}
	// カプセル vs モデル
	else if (shape1 == ColliderBase::SHAPE::CAPSULE
		&& shape2 == ColliderBase::SHAPE::MODEL)
	{
		auto cap = std::static_pointer_cast<ColliderCapsule>(colA);
		auto model = std::static_pointer_cast<ColliderModel>(colB);
		isHit = CollisionUtility::IsHit(*cap, *model);
	}
	// スフィア vs モデル
	else if (shape1 == ColliderBase::SHAPE::SPHERE
		&& shape2 == ColliderBase::SHAPE::MODEL)
	{
		auto sphere = std::static_pointer_cast<ColliderSphere>(colA);
		auto model = std::static_pointer_cast<ColliderModel>(colB);
		isHit = CollisionUtility::IsHit(*sphere, *model);
	}
	// ライン vs モデル
	else if (shape1 == ColliderBase::SHAPE::LINE
		&& shape2 == ColliderBase::SHAPE::MODEL)
	{
		auto line = std::static_pointer_cast<ColliderLine>(colA);
		auto model = std::static_pointer_cast<ColliderModel>(colB);
		isHit = CollisionUtility::IsHit(*line, *model);
	}

	if (isHit)
	{
		// 対象タグに含まれていたら格納
		if (colA->IsTargetTag(colB->GetTag())) {
			colA->SetHitCollider(colB);
		}

		// 対象タグに含まれていたら格納
		if (colB->IsTargetTag(colA->GetTag())) {
			colB->SetHitCollider(colA);
		}
	}
}

bool CollisionManager::CheckWallOcclusion(const VECTOR& start, const VECTOR& end)
{
	// プレイヤーと敵の間の直線を結ぶ一時的な線コライダーを生成
	std::vector<ColliderBase::TAG> targetTags = { ColliderBase::TAG::STAGE };

	ColliderLine tempLine(
		ColliderBase::TAG::NONE,
		targetTags,
		nullptr,
		start,
		end
	);

	// ステージコライダーを探して線コライダーとの交差を判定
	for (const auto& col : colliders_)
	{
		if (col.lock()->GetShape() != ColliderBase::SHAPE::MODEL
			&& col.lock()->GetTag() != ColliderBase::TAG::STAGE)continue;

		auto modelCol = std::static_pointer_cast<ColliderModel>(col.lock());
		if (!modelCol || !modelCol->GetIsCollier()) continue;

		// CollisionUtility の線分 vs モデル交差判定
		if (CollisionUtility::IsHit(
			tempLine, *modelCol))return true;
	}
	return false;
}