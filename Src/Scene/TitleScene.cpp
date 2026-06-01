#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/FontManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage/SkyDome.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	SceneBase()
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	resMng_.GetInstance().Load(ResourceManager::SRC::FONT);

	// フォントハンドルの作成
	pauseFont_ = fontMng_.GetInstance().CreateMyFont(L"はなぞめフォント", 56, 20);

}

void TitleScene::Update(void)
{

	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}

}

void TitleScene::Draw(void)
{

	DrawFormatStringToHandle(
		Application::SCREEN_SIZE_X / 2 - 150.0f,
		Application::SCREEN_SIZE_Y / 6,
		0x000000,
		pauseFont_,
		pasueList_[static_cast<int>(LIST::ASOSOLES)].c_str());

	for (int i = 1; i < LIST_MAX; ++i)
	{
		//座標位置を設定
		int posX = Application::SCREEN_SIZE_X / 2 - 150.0f;
		int posY = (Application::SCREEN_SIZE_Y / 2 + (100.0f * i)) - 100.0f;

		//文字列を描画
		DrawFormatStringToHandle(
			posX,
			posY,
			0x000000,
			pauseFont_,
			pasueList_[i].c_str());
	}

}

void TitleScene::Release(void)
{
	
	// フォントハンドルを削除
	DeleteFontToHandle(pauseFont_);

}
