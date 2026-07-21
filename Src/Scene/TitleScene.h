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
		//設定,		// タイトルに戻る
		//ゲーム終了,   // ゲームの終了
		MAX
	};

	//リスト
	static constexpr int LIST_MAX = static_cast<int>(LIST::MAX);

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	//読み込み
	void Load(void)override;

	//読み込み後の初期化
	void LoadEnd(void)override;

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

	//選択リスト
	std::wstring pasueList_[LIST_MAX] =
	{
		L"ASO SOLES",
		L"GAME STATE : KEY : SPACE or PAD : A",		// 続ける
		//L"    SYSTEM",		// タイトルに戻る
		//L"      QUIT",   // ゲームの終了
	};

};
