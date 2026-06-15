#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/AnimationController.h"

class SkyDome;

class TitleScene : public SceneBase
{

public:

	enum class LIST
	{
		ASOSEOUL,
		始める,		// 続ける
		設定,		// タイトルに戻る
		ゲーム終了,   // ゲームの終了
		MAX
	};

	//リスト
	static constexpr int LIST_MAX = static_cast<int>(LIST::MAX);

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	//タイトルフォント
	int titleFont_;

	//ポーズフォント
	int pauseFont_;

	//ポーズリストの選択
	int selectIndex_;

	int i;

	//選択リスト
	std::wstring pasueList_[LIST_MAX] =
	{
		L"ASO SOLES",
		L"GAME STATE",		// 続ける
		L"    SYSTEM",		// タイトルに戻る
		L"      QUIT",   // ゲームの終了
	};

};
