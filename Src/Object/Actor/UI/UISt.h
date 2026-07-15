#pragma once
#include "UIBase.h"

class Vector2;

class UISt : public UIBase
{
public:

    //スタミナ最大値
    static constexpr float MAX_ST = 100.0f;
    //スタミナ最小値
    static constexpr float MIN_ST = -15.0f;

    // コンストラクタ
    // maxHp は省略時 100.0f
    UISt(void);
    // デストラクタ
    ~UISt(void) override;
    // 更新
    void Update(void) override;
    // 描画
    void Draw(void) override;

    // ダメージ
    void SetSt(float delta);

    //回復
    void SetHpAbsolute(float hp);

    const float GetSt(void)const { return st_; }

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
    static constexpr int IMG_SIZE_X = 866;
    static constexpr int IMG_SIZE_Y = 288;

    int stUi1_;
    int stUi2_;

    Vector2 pos_;

    float st_;
};


