#pragma once
#include <string>
#include "../Object/Common/Collider2D/ColliderBox2D.h" 
#include "../Application.h"
#include "SceneBase.h"

class UiManager;

class PauseScene :public SceneBase
{
public:

	// 選択リスト
	enum class LIST
	{
		ゲームに戻る,
		タイトルに戻る,
		ゲーム終了,
		MAX
	};

	// コンストラクタ
	PauseScene(void);

	// デストラクタ
	~PauseScene(void) override = default;

	// 読み込み
	void Load(void)override {}

	// 読み込み後の初期化
	void LoadEnd(void)override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	// フォント座標調整値
	static constexpr Vector2F FONT_ADJUST = { 2.0f, 90.0f };

	// フォント同士のY座標間隔
	static constexpr float FONT_INTERVAL = 70.0f;

	// ポーズ画面の高さ
	static constexpr float PAUSE_HEIGHT = 200.0f;

	// ポーズ画面の大きさ
	static constexpr float PAUSE_SIZE = 1.0f;

	//ポーズ画面枠大きさ
	static constexpr float PAUSE_FRAME_SIZE = 3.0f;

	// ボックスコライダー座標調整値
	static constexpr Vector2F BOX_ADJUST = { 150.0f, 105.0f };

	// ボックスコライダーのサイズ
	static constexpr Vector2F BOX_SIZE = { 300.0f, 60.0f };

	// ボックスコライダー同士のY座標間隔
	static constexpr float BOX_Y_INTERVAL = 70.0f;

	// UIカーソル画像サイズ
	static constexpr Vector2F SELECT_IMG_SIZE = { 0.2f, 0.2f };

	// UIカーソル画像Y座標調整値
	static constexpr float SELECT_IMG_ADJUST_Y = 0.5f;

	// コントローラのデットゾーン
	static constexpr float STICK_DEAD_ZONE = 0.5f;

	// ポーズ画面枠色
	static constexpr int PAUSE_FRAME_COLOR = 0xffffff;

	// ポーズ画面のブレンドパラメータ
	static constexpr int PAUSE_BLENDPARAM = 168;

	// ポーズ画面の色
	static constexpr int PAUSE_COLOR = 0x000000;

	// 選択リストの最大値
	static constexpr int LIST_MAX = static_cast<int>(LIST::MAX);

	// フォントの色
	static constexpr int FONT_COLOR = 0xffffff;

	// フォントサイズ・太さ
	static constexpr int FONT_SIZE = 30;
	static constexpr int FONT_THICKNESS = 30;

	// UI識別ID
	static constexpr int UI_ID_CURSOR = 0;

	// ボックスコライダーの配列
	std::unique_ptr<ColliderBox2D> uiBoxs_[LIST_MAX];

	// UI管理
	std::unique_ptr<UiManager> uiManager_;

	// 選択されているかの判定
	bool isHovered;

	// スティック入力の判定
	bool isStickInput_;

	//ポーズリストの選択  
	int selectIndex_;

	// ポーズフォント
	int pauseFont_;

	// 選択リスト
	std::wstring pauseList_[LIST_MAX] =
	{
		L"ゲームに戻る",
		L"タイトルに戻る",
		L"ゲーム終了",
	};

};

