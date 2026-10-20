#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "../Template/Singleton.h"
#include "../Object/Common/Collider/ColliderBase.h"
#include "../Utility/ColliderUtility.h"

class ActorBase;

class CollisionManager : public Singleton<CollisionManager>
{
	friend class Singleton<CollisionManager>;
public:
	// コライダー登録
	void AddCollider(const std::shared_ptr<ColliderBase>& col);

	// 判定更新
	void Update(void);

	// デバッグ描画
	void DrawDebug(void);

	// 解放処理
	void Release(void);

	// 壁による遮蔽判定（武器 vs プレイヤー等の遮蔽チェック用）
	bool CheckWallOcclusion(
		const VECTOR& start, const VECTOR& end);
private:
	// コライダー配列
	std::vector<std::weak_ptr<ColliderBase>> colliders_;

	// 2者間の判定・押し戻し計算および HitInfo 生成
	void ResolveCollision(
		const std::shared_ptr<ColliderBase>& argA,
		const std::shared_ptr<ColliderBase>& argB);

	// どちらか一方でも相手を対象としているか判定
	bool CanCollide(
		const ColliderBase& a,
		const ColliderBase& b) const;
};
