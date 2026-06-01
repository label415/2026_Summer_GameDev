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
		ASOSOLES,
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

	//ポーズフォント
	int pauseFont_;

	//ポーズリストの選択
	int selectIndex_;

	//選択リスト
	std::wstring pasueList_[LIST_MAX] =
	{
		L" ASO SOLES",
		L"  始める",		// 続ける
		L"   設定",		// タイトルに戻る
		L"ゲーム終了",   // ゲームの終了
	};

};
