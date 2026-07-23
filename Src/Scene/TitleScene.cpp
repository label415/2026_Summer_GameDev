#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/FontManager.h"
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

	selectIndex_ = 0;
	selectImgX_ = 0.0f;
	selectImgY_ = 0.0f;
	isHovered = true;
}

void TitleScene::LoadEnd(void)
{
}

void TitleScene::Update(void)
{
	auto& ins = InputManager::GetInstance();

	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	VECTOR dir = AsoUtility::VECTOR_ZERO;
	dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	constexpr float STICK_DEAD_ZONE = 0.5f;

	bool isUp = (dir.z > STICK_DEAD_ZONE);
	bool isDown = (dir.z < -STICK_DEAD_ZONE);


	if (isUp)
	{
		InputManager::GetInstance().SetMouseFlage(false);
		if (!isStickInput_)
		{
			selectIndex_--;
			if (selectIndex_ < 0)
			{
				selectIndex_ = LIST_MAX - 1;
			}
			isStickInput_ = true;
			isHovered = true;
		}
	}
	else if (isDown)
	{
		InputManager::GetInstance().SetMouseFlage(false);
		if (!isStickInput_)
		{
			selectIndex_++;
			if (selectIndex_ >= LIST_MAX)
			{
				selectIndex_ = 0;
			}
			isStickInput_ = true;
			isHovered = true;
		}
	}
	else
	{
		isStickInput_ = false;
	}

	bool nextSene = ins.IsTrgMouseLeft()
		|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	if (nextSene && isHovered)
	{
		if (selectIndex_ == static_cast<int>(LIST::始める))
		{
			sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
		}
		else if (selectIndex_ == static_cast<int>(LIST::ゲーム終了))
		{
			Application::GetInstance().SetIsEnd(true);
		}
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

	// マウスが動いた場合のみ、マウスでホバー中の項目に切り替える
	for (int i = 0; i < LIST_MAX; ++i)
	{
		int itemPosY = static_cast<int>(Application::SCREEN_SIZE_Y / 1.6f) + (70 * i);

		if (mousePos.y >= itemPosY && mousePos.y < itemPosY + 50)
		{
			InputManager::GetInstance().SetMouseFlage(true);
			isHovered = true;
			selectIndex_ = i;
			break;
		}
	}

	// 選択中の項目のY座標を算出
	float selectImgY = Application::SCREEN_SIZE_Y / 2 + 105.0f;
	if (selectIndex_ >= 0 && selectIndex_ < LIST_MAX)
	{
		int itemPosY = static_cast<int>(Application::SCREEN_SIZE_Y / 1.6f) + (70 * selectIndex_);
		selectImgY = static_cast<float>(itemPosY + 15);
	}

	// 選択画像（カーソル）の描画
	if (isHovered && selectIndex_ >= 0)
	{
		DrawRotaGraph(
			Application::SCREEN_SIZE_X / 2,
			static_cast<int>(selectImgY),
			0.2f, 0.0f,
			selectImg_, true);
	}

	// メニュー文字の描画
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