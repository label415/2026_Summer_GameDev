#pragma once
#include <string>
#include "UIBase.h"
class UIRecovery : public UIBase
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

	//回復瓶の数を取得
    const int GetBottlcCnt(void)const{ return bottleCount_; }
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
    //　使用するフォント
    std::wstring fontName_ = L"KazukiReiwa";
    // フォントのサイズ
    static constexpr int FONT_SIZE = 50;

    // アイテムボックスの大きさ
	static constexpr float BOX_SIZE = 0.8f;

	// 回復瓶の大きさ
	static constexpr float BOTTLE_SIZE = 0.4f;

	// UIの座標調整値
	static constexpr float UI_POS_ADJ_X = 8.0f;
    static constexpr float UI_POS_ADJ_Y = 1.5f;

    // フォント座標調整値
	static constexpr float FONT_POS_ADJ_X = 6.5f;
    static constexpr float FONT_POS_ADJ_Y = 1.5f;

	// フォントの色
	static constexpr int FONT_COLOR = 0xffffff;

    // 回復瓶の数
    int bottleCount_;

	// 回復瓶の数を表示するフォント
    std::wstring cntfont_;

    // アイテムボックスハンドル
    int imgBox_;

	// 回復瓶ハンドル
    int imgBottle_;

    // フォント
    int font_;
};

