#pragma once
#include "UIBase.h"

class Vector2;

class UIHp : public UIBase
{
public:

    // コンストラクタ
    UIHp(float posX, float posY,float flameScl = 1.0f,
        float hpSclX = 1.0f, float hpSclY = 1.0f, float hp = MAX_HP);
    // デストラクタ
    ~UIHp(void) override;
    // 更新
    void Update(void) override;
    // 描画
    void Draw(void) override;
    // ダメージ
    void SetHp(float delta);
    //回復
    void SetHpAbsolute(float hp);

protected:
    // リソースロード
    void InitLoad(void) override;
    // 大きさ、回転、座標の初期化
    void InitTransform(void) override;
    // 衝突判定の初期化
    void InitCollider(void) override {}
    // アニメーションの初期化
    void InitAnimation(void) override {}
    // 初期化後の個別処理
    void InitPost(void) override;
private:

    //最大HP
    static constexpr float MAX_HP = 100.0f;

    //画像サイズ
    static constexpr float IMG_SIZE_X = 866.0f;
    static constexpr float IMG_SIZE_Y = 288.0f;

    int hpUi1_;
    int hpUi2_;

    Vector2 pos_;

    float hp_;

    float flameScl_;
    float hpSclX_;
    float hpSclY_;
};

