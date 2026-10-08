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
    void AddCollider(std::shared_ptr<ColliderBase> col);

    // 判定更新
    void Update(void);

    // デバッグ描画
    void DrawDebug(void);

    // 解放処理
    void Release(void);

    // 登録されているコライダーから特定のタグを持つコライダーを取得する
    std::shared_ptr<ColliderBase> GetColliderByTag(ColliderBase::TAG tag) const
    {
        for (const auto& col : colliders_)
        {
            if (col && col->GetTag() == tag)
            {
                return col;
            }
        }
        return nullptr;
    }
private:
    // コライダー配列
	std::vector<std::shared_ptr<ColliderBase>> colliders_;

	// 2者間の判定・押し戻し計算および HitInfo 生成
	void ResolveCollision(
		std::shared_ptr<ColliderBase> colA,
		std::shared_ptr<ColliderBase> colB);

	// どちらか一方でも相手を対象としているか判定
	bool CanCollide(
		const ColliderBase& a,
		const ColliderBase& b) const;

	// 壁による遮蔽判定（武器 vs プレイヤー等の遮蔽チェック用）
	bool CheckWallOcclusion(const VECTOR& start, const VECTOR& end) const;
};
