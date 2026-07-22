#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
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

void TitleScene::Load(void)
{
	bgm_ = resMng_.Load(ResourceManager::SRC::TITLE_BGM).handleId_;
	volume_ = 50;
	SoundManager::GetInstance().PlayBGM(bgm_, volume_);

	// フォントハンドルの作成
	resMng_.Load(ResourceManager::SRC::FONT);
	pauseFont_ = fontMng_.GetInstance().CreateMyFont(L"KazukiReiwa", 30, 30);

	titleImg_ = resMng_.Load(ResourceManager::SRC::TITLE_IMG).handleId_;
	selectImg_ = resMng_.Load(ResourceManager::SRC::TITLE_SELECT).handleId_;

	InputManager::GetInstance().SetMouseFlage(true);

}

void TitleScene::LoadEnd(void)
{
}

void TitleScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();

	bool nextSene = ins.IsTrgDown(KEY_INPUT_SPACE)
		|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	if (nextSene)
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 2 - 100,
		0.8f, 0.0f,
		titleImg_, true);

	Vector2 mousePos = InputManager::GetInstance().GetMousePos();

	float selectImgY = Application::SCREEN_SIZE_Y / 2 + 105.0f;
	isHovered = false;
	for (int i = 0; i < LIST_MAX; ++i)
	{
		int itemPosY = static_cast<int>(Application::SCREEN_SIZE_Y / 1.6f) + (70 * i);

		if (mousePos.y >= itemPosY && mousePos.y < itemPosY + 50)
		{
			selectImgY = static_cast<float>(itemPosY + 15);
			isHovered = true;
			break;
		}
	}

	if(isHovered)
	{
		DrawRotaGraph(
			Application::SCREEN_SIZE_X / 2,
			static_cast<int>(selectImgY),
			0.2f, 0.0f,
			selectImg_, true);
	}

	for (int i = 0; i < LIST_MAX; ++i)
	{
		int stringWidth = GetDrawStringWidthToHandle(
			pasueList_[i].c_str(),
			-1,
			pauseFont_);

		int posX = (Application::SCREEN_SIZE_X / 2) - (stringWidth / 2);
		int posY = static_cast<int>(Application::SCREEN_SIZE_Y / 1.6f) + (70 * i);

		DrawFormatStringToHandle(
			posX,
			posY,
			0xffffff,
			pauseFont_,
			pasueList_[i].c_str());
	}
}

void TitleScene::Release(void)
{
	DeleteFontToHandle(pauseFont_);
}
