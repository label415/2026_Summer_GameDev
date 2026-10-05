#pragma once
#include "../../Template/Vector2Template.h"

class Loading
{
private:
	// 最低でもロード画面を表示する時間
	static constexpr int MIN_LOAD_TIME = 60;
public:
	// コンストラクタ
	Loading();

	//デストラクタ
	~Loading();

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
	bool IsLoading(void) { return GetASyncLoadNum() != 0 && loadTimer_ <= MIN_LOAD_TIME; }
private:
	// フォント名
	std::wstring FONT_NAME = L"KazukiReiwa";
	//表示するフォント
	std::wstring LOADING = L"Loading";
	// フォント座標調整値
	static constexpr float FONT_ADJUST_X = 200.0f;
	static constexpr float FONT_ADJUST_Y = 70.0f;
	// フォントのサイズ
	static constexpr float FONT_SIZE = 56.0f;
	// フォントの太さ
	static constexpr float FONT_THICKNESS = 20.0f;
	// フォントの色
	static constexpr int FONT_COLOR = 0xffffff;

	// ロード中追加するフォント
	std::wstring dots_;

	// 最低でもロード画面を表示する時間の範囲
	int loadTimer_;

	//ポーズフォント
	int pauseFont_;
};

