#pragma once
#include <DxLib.h>
#include "../../Template/Vector2Template.h"

class Loading
{
public:

	// コンストラクタ
	Loading();

	//デストラクタ
	~Loading() {}

	// ロード
	void Load(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	//解放
	void Release(void);

	// 非同期ロードの開始
	void StartAsyncLoad(void);

	// 非同期ロードの終了
	void EndAsyncLoad(void);

	// ロード中かを返す。
	bool GetIsLoading(void) { return isLoading_; }

private:

	// フォント名
	static constexpr const wchar_t* FONT_NAME = L"KazukiReiwa";

	//表示するフォント
	static constexpr const wchar_t* LOADING_TEXT = L"Loading";

	// フォント座標調整値
	static constexpr float FONT_ADJUST_X = 200.0f;
	static constexpr float FONT_ADJUST_Y = 70.0f;

	// フォントのサイズ
	static constexpr float FONT_SIZE = 40.0f;

	// フォントの太さ
	static constexpr float FONT_THICKNESS = 20.0f;

	// フォントの色
	static constexpr int FONT_COLOR = 0xffffff;

	// 最低でもロード画面を表示する時間
	static constexpr int MIN_LOAD_TIME = 70;

	// ロード中ドットアニメーション間隔・周期
	static constexpr int DOT_ANIM_INTERVAL = 20;
	static constexpr int DOT_ANIM_CYCLE = 4;

	// 最低でもロード画面を表示する時間の範囲
	int loadTimer_;

	//ポーズフォント
	int loadingFont_;

	// ロード中の判定用
	bool isLoading_;

};

