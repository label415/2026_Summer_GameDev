#pragma once
#include <string>
#include "SceneBase.h"

class PauseScene :public SceneBase
{
public:

	enum class LIST
	{
		ゲームに戻る,
		タイトルに戻る,
		ゲーム終了,
		MAX
	};

	//リスト
	static constexpr int LIST_MAX = static_cast<int>(LIST::MAX);

	// コンストラクタ
	PauseScene(void);

	// デストラクタ
	~PauseScene(void) override;

	//読み込み
	void Load(void)override{}

	//読み込み後の初期化
	void LoadEnd(void)override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }

	bool GetIsAlive(void) const { return isAlive_; }

private:

	Vector2 prevMousePos_;

	//ポーズフォント
	int pauseFont_;

	//ポーズリストの選択
	int selectIndex_;

	int titleImg_;

	int selectImg_;

	float selectImgX_;
	float selectImgY_;

	bool isHovered;

	bool isStickInput_;

	bool isAlive_;

	//選択リスト
	std::wstring pasueList_[LIST_MAX] =
	{
		L"ゲームに戻る",
		L"タイトルに戻る",
		L"ゲーム終了",
	};

};

