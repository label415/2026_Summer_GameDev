#pragma once
#include <DxLib.h>
#include "ColliderBase.h"

class ColliderModel;

class ColliderSphere : public ColliderBase
{
public:
    // コンストラクタ
    ColliderSphere(
        TAG tag, const std::vector<TAG>& targetTags, std::weak_ptr<const Transform> follow,
        const VECTOR& localPos, float radius, int patrTag = 0);

    // デストラクタ
    ~ColliderSphere(void);

    // 親Transformからの相対位置を取得
    const VECTOR& GetLocalPos(void) const { return localPos_; }

    // 親Transformからの相対位置をセット
    void SetLocalPos(const VECTOR& localPos) { localPos_ = localPos; }

    // ワールド座標を取得
    VECTOR GetPos(void) const { return GetRotPos(localPos_); }

    // 半径
    float GetRadius(void) const { return radius_; }

    void SetRadius(float radius) { radius_ = radius; }
protected:
    // デバッグ用描画
    void DrawDebug(int color) override;
private:
    // デバッグ表示の球体ポリゴン分割数
    static constexpr int DIV_NUM = 6;

    // 親Transformからの相対位置(下側)
    VECTOR localPos_;

    // 半径
    float radius_;
};

