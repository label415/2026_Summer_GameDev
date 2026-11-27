#pragma once
#include <DxLib.h>
#include "ColliderBase.h"

class ColliderModel;

class ColliderSphere : public ColliderBase
{
public:

    // コンストラクタ
    ColliderSphere(
        TAG tag,
        const std::vector<TAG>& targetTags,
        const Transform* follow,
        const VECTOR& localPos,
        float radius,
        int partTag = 0);

    // デストラクタ
	~ColliderSphere(void)override = default;

    // 親Transformからの相対位置を取得
    const VECTOR& GetLocalPos(void) const { return localPos_; }

    // 親Transformからの相対位置をセット
    void SetLocalPos(const VECTOR& localPos) { localPos_ = localPos; }

    // ワールド座標を取得
    VECTOR GetPos(void) const { return GetRotPos(localPos_); }

    // 半径
    float GetRadius(void) const { return radius_; }

	// 半径の設定
    void SetRadius(float radius) { radius_ = radius; }

protected:

    // デバッグ用描画
    void DrawDebug(int color) override { DrawSphere3D(GetPos(), GetRadius(), DIV_NUM, color, color, false); }

private:

    // デバッグ表示の球体ポリゴン分割数
    static constexpr int DIV_NUM = 6;

    // 親Transformからの相対位置
    VECTOR localPos_;

    // 半径
    float radius_;

};

