#pragma once  
#include "../Object/Common/Collider2D/ColliderBox2D.h" 
#include "../Application.h"
#include "SceneBase.h"  

class SkyDome;

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
	// タイトル画像サイズ
	static constexpr float TITLE_IMG_SIZE = 1.0f;
	// タイトル画像Y座標調整値
	static constexpr int TITLE_IMG_ADJUST_Y = 100.0f;

	//　使用するフォント
	std::wstring fontName_ = L"KazukiReiwa";
	// フォント座標調整値
	static constexpr float FONT_ADJUST_X = 2.0f;
	static constexpr float FONT_ADJUST_Y = 90.0f;
	// フォント同士のY座標間隔
	static constexpr float FONT_INTERVAL = 70.0f;
	// フォントのサイズ
	static constexpr float FONT_SIZE = 30.0f;
	// フォントの太さ
	static constexpr float FONT_THICKNESS = 30.0f;
	// 選択リストの最大値
	static constexpr int LIST_MAX = static_cast<int>(LIST::MAX);
	// フォントの色
	static constexpr int FONT_COLOR = 0xffffff;

	// ボックスコライダ-座標調整値
	static constexpr float BOX_ADJUST_X = 150.0f;
	static constexpr float BOX_ADJUST_Y = 75.0f;
	// ボックスコライダーの高さ
	static constexpr float BOX_HEIGHT = 60.0f;
	// ボックスコライダーの幅
	static constexpr float BOX_WIDTH = 300.0f;
	// ボックスコライダー同士のY座標間隔
	static constexpr float BOX_Y_INTERVAL = 70.0f;

	// UIカーソル画像サイズ
	static constexpr float SELECT_IMG_SIZE = 0.2f;

	// コントローラのデットゾーン
	static constexpr float STICK_DEAD_ZONE = 0.5f;

	// BGMの音量
	static constexpr int BGM_VOLUME = 50;

	// ボックスコライダーの配列
	ColliderBox2D* uiBoxs_[LIST_MAX];

	// フォントハンドル
	int pauseFont_;

	//ポーズリストの選択  
	int selectIndex_;

	// タイトル画像
	int titleImg_;

	// UIカーソル画像
	int selectImg_;

	// 選択されているかの判定
	bool isHovered;

	// スティック入力の判定
	bool isStickInput_;

	// 選択リスト  
	std::wstring pasueList_[LIST_MAX] =
	{
		L"GAME START",	// 続ける
		L"QUIT",		// ゲームの終了  
	};
};
