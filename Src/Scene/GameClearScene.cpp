#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/FontManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage/SkyDome.h"
#include "GameClearScene.h"

GameClearScene::GameClearScene(void)
	:
	SceneBase()
{
}

GameClearScene::~GameClearScene(void)
{
}

void GameClearScene::Load(void)
{
	i = resMng_.Load(ResourceManager::SRC::TITLE_SELECT).handleId_;

	// フォントハンドルの作成
	resMng_.Load(ResourceManager::SRC::FONT);
	titleFont_ = fontMng_.GetInstance().CreateMyFont(L"KazukiReiwa", 140, 140);
	pauseFont_ = fontMng_.GetInstance().CreateMyFont(L"KazukiReiwa", 25, 25);
}

void GameClearScene::LoadEnd(void)
{
}

void GameClearScene::Update(void)
{

	// シーン遷移
	auto const& ins = InputManager::GetInstance();

	bool nextSene = ins.IsTrgDown(KEY_INPUT_SPACE)
		|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	if (nextSene)
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

}

void GameClearScene::Draw(void)
{

	DrawFormatStringToHandle(
		Application::SCREEN_SIZE_X / 10,
		Application::SCREEN_SIZE_Y / 3,
		0xffffff,
		titleFont_,
		pasueList_[static_cast<int>(LIST::ASOSEOUL)].c_str());


	for (int i = 1; i < LIST_MAX; ++i)
	{
		//座標位置を設定
		/*int posX = Application::SCREEN_SIZE_X / 2 - 110;
		int posY = (Application::SCREEN_SIZE_Y / 2 +50+ (50.0f * i));*/
		int posX = Application::SCREEN_SIZE_X / 2 - 200;
		int posY = (Application::SCREEN_SIZE_Y / 2 + 50 + (50.0f * i));

		//文字列を描画
		DrawFormatStringToHandle(
			posX,
			posY,
			0xffffff,
			pauseFont_,
			pasueList_[i].c_str());
	}
}

void GameClearScene::Release(void)
{
	DeleteFontToHandle(titleFont_);
	DeleteFontToHandle(pauseFont_);
}

