#pragma once  
#include "../Object/Common/Collider2D/ColliderBox2D.h" 
#include "../Application.h"
#include "SceneBase.h"  

class SkyDome;
class UiManager;

class TitleScene : public SceneBase
{
public:

	// 選択リスト
	enum class LIST
	{
		始める,
		ゲーム終了,
		MAX
	};

	// コンストラクタ  
	TitleScene(void);

	// デストラクタ  
	~TitleScene(void) override = default;

	// 読み込み  
	void Load(void)override;

	// 読み込み後の初期化  
	void LoadEnd(void)override;

	// 更新  
	void Update(void) override;

	// 描画  
	void Draw(void) override;

	// 解放  
	void Release(void) override;

private:

	// タイトル画像サイズ
	static constexpr Vector2F TITLE_IMG_SIZE = { 1.0f, 1.0f };

	// タイトル画像Y座標調整値
	static constexpr float TITLE_IMG_ADJUST_Y = 100.0f;

	// フォント座標調整値
	static constexpr Vector2F FONT_ADJUST = { 2.0f, 90.0f };

	// フォント同士のY座標間隔
	static constexpr float FONT_INTERVAL = 70.0f;

	// ボックスコライダー座標調整値
	static constexpr Vector2F BOX_ADJUST = { 150.0f, 75.0f };

	// ボックスコライダーのサイズ
	static constexpr Vector2F BOX_SIZE = { 300.0f, 60.0f };

	// ボックスコライダー同士のY座標間隔
	static constexpr float BOX_Y_INTERVAL = 70.0f;

	// UIカーソル画像サイズ
	static constexpr Vector2F SELECT_IMG_SIZE = { 0.2f, 0.2f };

	//UIカーソル画像Y座標調整値
	static constexpr float SELECT_IMG_ADJUST_Y = 0.5f;

	// コントローラのデットゾーン
	static constexpr float STICK_DEAD_ZONE = 0.5f;

	// 選択リストの最大値
	static constexpr int LIST_MAX = static_cast<int>(LIST::MAX);

	// フォントの色
	static constexpr int FONT_COLOR = 0xffffff;

	// フォントサイズ・太さ
	static constexpr int FONT_SIZE = 30;
	static constexpr int FONT_THICKNESS = 30;

	// 選択リスト  
	std::wstring titleList_[LIST_MAX] =
	{
		L"GAME START",	// 続ける
		L"QUIT",		// ゲームの終了  
	};

	// UI識別ID
	enum class UI_ID
	{
		TITLE_LOGO,
		CURSOR
	};

	// ボックスコライダーの配列
	std::unique_ptr<ColliderBox2D> uiBoxs_[LIST_MAX];

	// UI管理
	std::unique_ptr<UiManager> uiManager_;

	//ポーズリストの選択  
	int selectIndex_;

	// タイトルフォント
	int titleFont_;

	// 選択されているかの判定
	bool isHovered;

	// スティック入力の判定
	bool isStickInput_;

};
