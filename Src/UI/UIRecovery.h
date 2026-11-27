#pragma once
#include <functional>
#include <memory>
#include <string>
#include "ImgUI.h"

class UIRecovery : public ImgUI
{
public:

	// コンストラクタ
	UIRecovery(int initCnt = 0);

	// デストラクタ
	~UIRecovery(void) override = default;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

	// モデル側の個数取得関数をバインドする
	void BindBottleCnt(std::function<int(void)> countGetter);

	// 表示用の回復瓶個数を設定する（表示専用）
	void SetBottleCnt(int cnt);

private:

	// アイテムボックスの大きさ
	static constexpr Vector2F BOX_SIZE = { 0.8f, 0.8f };

	// 回復瓶の大きさ
	static constexpr Vector2F BOTTLE_SIZE = { 0.4f, 0.4f };

	// UIの座標調整値
	static constexpr Vector2F UI_POS_ADJ = { 8.0f, 1.5f };

	// フォント座標調整値
	static constexpr Vector2F FONT_POS_ADJ = { 6.5f, 1.5f };

	// フォントの色
	static constexpr int FONT_COLOR = 0xffffff;

	// フォントサイズ
	static constexpr int FONT_SIZE = 50;

	// 表示中の回復瓶個数
	int displayCount_;

	// 回復瓶の数を表示する文字列
	std::wstring cntfont_;

	// 回復瓶用画像UI
	std::unique_ptr<ImgUI> bottleImg_;

	// 回復瓶カウント用フォントハンドル
	int font_;

	// モデル参照用コールバック
	std::function<int(void)> countGetter_;

};
