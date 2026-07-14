#pragma once
#include "UIBase.h"

class Vector2;

class UIHp : public UIBase
{
public:
    // コンストラクタ
    // maxHp は省略時 100.0f
    UIHp(float pos1X, float pos1Y, float pos2X, float pos2Y, float imgSize, float maxHp = 100.0f);
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

    // 最大HPを設定
    void SetMaxHp(float maxHp);

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

    //画像サイズ
    static constexpr float IMG_SIZE_X = 866.0f;
    static constexpr float IMG_SIZE_Y = 288.0f;

    int hpUi1_;
    int hpUi2_;

    Vector2 pos1_;
    Vector2 pos2_;
    float imgSize_;

    float maxHp_;
    float currentHp_;
};

