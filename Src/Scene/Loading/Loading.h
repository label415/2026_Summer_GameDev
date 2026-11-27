#pragma once
#include <DxLib.h>
#include "../../Template/Vector2Template.h"

class Loading
{
public:

	// コンストラクタ
	Loading(void);

	//デストラクタ
	~Loading(void) = default;

	// ロード
	void Load(void){}

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	//解放
	void Release(void){}

	// 非同期ロードの開始
	void StartAsyncLoad(void);

	// 非同期ロードの終了
	void EndAsyncLoad(void);

	// ロード中かを返す。
	bool GetIsLoading(void) { return isLoading_; }

private:

	//表示するフォント
	static constexpr const wchar_t* LOADING_TEXT = L"Loading";

	// フォント座標調整値
	static constexpr Vector2F FONT_ADJUST = { 200.0f, 70.0f };

	// フォントの色
	static constexpr int FONT_COLOR = 0xffffff;

	// フォントサイズ・太さ
	static constexpr int FONT_SIZE = 40;
	static constexpr int FONT_THICKNESS = 20;

	// 最低でもロード画面を表示する時間
	static constexpr int MIN_LOAD_TIME = 70;

	// ロード中ドットアニメーション間隔・周期
	static constexpr int DOT_ANIM_INTERVAL = 20;
	static constexpr int DOT_ANIM_CYCLE = 4;

	// 最低でもロード画面を表示する時間の範囲
	int loadTimer_;

	// ロード中の判定用
	bool isLoading_;

};

