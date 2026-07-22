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
		始める,		// 続ける
		ゲーム終了, // ゲームの終了
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

	//ポーズフォント
	int pauseFont_;

	//ポーズリストの選択
	int selectIndex_;

	int titleImg_;

	int selectImg_;

	float selectImgX_ = 0.0f;
	float selectImgY_ = 0.0f;

	bool isHovered = false;

	//選択リスト
	std::wstring pasueList_[LIST_MAX] =
	{
		L"GAME STATE",// 続ける
		L"	QUIT",   // ゲームの終了
	};

};
