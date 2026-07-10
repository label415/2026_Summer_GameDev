#pragma once
#include <string>
#include "UIBase.h"
class UIRecovery :
    public UIBase
{
public:
    // コンストラクタ
    UIRecovery(int cnt);
    // デストラクタ
    ~UIRecovery(void) override;
    // 更新
    void Update(void) override;
    // 描画
    void Draw(void) override;
    // 描画とイージング開始
    void Start(void);

    //回復瓶の数を調整
    void SetBottleCnt(int cnt);
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
    // 演出総時間
    static constexpr float DURATION = 2.0f;
    // バウンス演出時間
    static constexpr float TIME_BOUNCE = 0.6f;
    // バウンス強度
    static constexpr float POW_BOUNCE = 9.0f;
   
    int cnt_;

    std::wstring cntfont_;

    int imgBox_;
    int imgBottle_;

    //タイトルフォント
    int titleFont_;
};

