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
		RESUME,		// 続ける
		TITLE,		// タイトルに戻る
		GAME_END,   // ゲームの終了
		MAX
	};

	//リスト
	static constexpr int LIST_MAX = static_cast<int>(LIST::MAX);

	enum class ANIM_TYPE
	{
		RUN,
	};

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

	//スカイドーム
	SkyDome* skydome_;

	//アニメーションコントローラ
	AnimationController* anim_;

	// 惑星
	Transform bigPlanet_;
	Transform spherPlanet_;

	//プレイヤー
	Transform player_;

	//ポーズフォント
	int pauseFont_;

	//ポーズリストの選択
	int selectIndex_;

	//選択リスト
	std::wstring pasueList_[LIST_MAX] =
	{
		L"つづける",
		L"タイトルへ戻る",
		L"ゲーム終了"
	};

	// 画像読み込み
	int imgTitle_;
	int imgPushSpace_;

};
