#pragma once
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

	void Load(void);		// 読み込み
	void Update(void);		// 更新
	void Draw(void);		// 描画
	void Release(void);		// 解放

	void StartAsyncLoad(void);	// 非同期ロードの開始
	void EndAsyncLoad(void);	// 非同期ロードの終了

	// ロード中かを返す。
	bool IsEnd(void) const { return (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_TIME); }

private:

	//ポーズフォント
	int pauseFont_;

	std::wstring dots;
};

