#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "../Template/Singleton.h"
#include "../Object/Common/Collider/ColliderBase.h"
#include "../Utility/ColliderUtility.h"

class ActorBase;

// 衝突結果データ
struct CollisionResult
{
    bool isHit = false;
    VECTOR pushOffset = { 0.0f, 0.0f, 0.0f }; // 押し戻し・押し上げベクトル
};

// 衝突通知コールバックの型
using CollisionCallback = std::function<void(const std::shared_ptr<ColliderBase>& self,
    const std::shared_ptr<ColliderBase>& opponent,
    const CollisionResult& result)>;

class CollisionManager : public Singleton<CollisionManager>
{
    friend class Singleton<CollisionManager>;
public:
    struct ColliderEntry
    {
        std::shared_ptr<ColliderBase> collider;
        CollisionCallback onCollision;
    };

    // コライダー登録 (コールバック指定)
    void AddCollider(std::shared_ptr<ColliderBase> col, CollisionCallback callback = nullptr);

    // 判定更新
    void Update(void);

    // デバッグ描画
    void DrawDebug(void);

    // 解放処理
    void Release(void);
private:
    std::vector<ColliderEntry> colliders_;

    // 各ペアごとの判定・計算・結果反映
    void ResolveCollision(ColliderEntry& entryA, ColliderEntry& entryB);

    // タグ間の判定要否チェック
    bool IsCheckColliderTag(ColliderBase::TAG tagA, ColliderBase::TAG tagB) const;
};
