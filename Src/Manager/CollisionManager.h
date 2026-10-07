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
private:
    // コライダー配列
    std::vector<std::shared_ptr<ColliderBase>> colliders_;

    // 各ペアごとの判定・計算・結果反映
    void ResolveCollision(
        std::weak_ptr<ColliderBase> collderA, std::weak_ptr<ColliderBase> collderB);

    // タグ間の判定要否チェック
    bool IsCheckColliderTag(ColliderBase::TAG tagA, ColliderBase::TAG tagB) const;
};
