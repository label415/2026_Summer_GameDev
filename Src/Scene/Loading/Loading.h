#pragma once
#include <string>
#include "../../Common/Vector2.h"

class Loading
{
private:
	// 最低でもロード画面を表示する時間の範囲
	int loadTimer_;
public:

	// 最低でもロード画面を表示する時間
	static constexpr int MIN_LOAD_TIME = 300;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	Loading();
	~Loading();

	// 読み込み
	void Load(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);

	// 非同期ロードの開始
	void StartAsyncLoad(void);

	// 非同期ロードの終了
	void EndAsyncLoad(void);

	// ロード中かを返す。
	bool IsEnd(void) const { return (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_TIME); }
private:
	//ポーズフォント
	int pauseFont_;

	//ドットの文字列
	std::wstring dots;
};

