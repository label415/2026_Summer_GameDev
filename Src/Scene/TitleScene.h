#pragma once  
#include <memory>  
#include "SceneBase.h"  
#include "../Object/Common/Transform.h"  
#include "../Object/Common/AnimationController.h"  
#include "../Object/Common/Collider2D/ColliderBox2D.h"  

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

	//リスト  
	static constexpr int LIST_MAX = static_cast<int>(LIST::MAX);
	//BGMの音量
	static constexpr int BGM_VOLUME = 50;
	//フォントのサイズ
	static constexpr float FONT_SIZE = 30.0f;
	//フォントの太さ
	static constexpr float FONT_THICKNESS = 30.0f;

	//ボックスコライダーの配列
	std::unique_ptr<ColliderBox2D> uiBoxs_[LIST_MAX];

	//ポーズフォント  
	int pauseFont_;

	//ポーズリストの選択  
	int selectIndex_;

	//タイトル画像
	int titleImg_;

	//選択画像
	int selectImg_;

	//選択されているかの判定
	bool isHovered;

	//スティック入力の判定
	bool isStickInput_;

	//選択リスト  
	std::wstring pasueList_[LIST_MAX] =
	{
		L"GAME START",	// 続ける  
		L"QUIT",		// ゲームの終了  
	};

};
