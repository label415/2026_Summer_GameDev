#pragma once
#include "../Object/Common/Transform.h"
#include "../Object/Common/AnimationController.h"
#include "SceneBase.h"
class GameClearScene :
    public SceneBase
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
	GameClearScene(void);

	// デストラクタ
	~GameClearScene(void) override;

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
		L"GAME CLEAR",
		L"KEY : SPACE or PAD : A",		// 続ける
		//L"    SYSTEM",		// タイトルに戻る
		//L"      QUIT",   // ゲームの終了
	};

};

