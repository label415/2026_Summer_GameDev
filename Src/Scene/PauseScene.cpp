#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/FontManager.h"
#include "TitleScene.h"
#include "PauseScene.h"

PauseScene::PauseScene(void)
{
}

PauseScene::~PauseScene(void)
{
}

void PauseScene::LoadEnd(void)
{
	// フォントハンドルの作成
	resMng_.Load(ResourceManager::SRC::FONT);
	pauseFont_ = fontMng_.GetInstance().CreateMyFont(L"KazukiReiwa", 30, 30);
	selectImg_ = resMng_.Load(ResourceManager::SRC::TITLE_SELECT).handleId_;

	selectIndex_ = 0;
	selectImgX_ = 0.0f;
	selectImgY_ = 0.0f;

	isHovered = true;
	isAlive_ = false;
}

// PauseScene.cpp

void PauseScene::Update(void)
{
	auto& ins = InputManager::GetInstance();

	// === 1. パッド / キーボードの操作判定 ===
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	VECTOR dir = AsoUtility::VECTOR_ZERO;
	dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	constexpr float STICK_DEAD_ZONE = 0.35f;

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
			// スティック操作があったらホバー状態にする
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
			// スティック操作があったらホバー状態にする
			isHovered = true;
		}
	}
	else
	{
		isStickInput_ = false;
	}

	// === 2. マウスの操作判定（★ここに追加） ===
	Vector2 mousePos = ins.GetMousePos();

	// ★重要：マウスが実際に動いた場合のみ、選択インデックスを更新する
	if (mousePos.x != prevMousePos_.x || mousePos.y != prevMousePos_.y)
	{
		for (int i = 0; i < LIST_MAX; ++i)
		{
			// PauseSceneのY座標計算式
			int itemPosY = static_cast<int>(Application::SCREEN_SIZE_Y / 2.0f - 90) + (70 * i);

			if (mousePos.y >= itemPosY && mousePos.y < itemPosY + 50)
			{
				InputManager::GetInstance().SetMouseFlage(true);
				isHovered = true;
				selectIndex_ = i;
				// 項目に乗ったらループを抜ける
				break;
			}
			else
			{
				// どの項目にも乗っていない場合はホバーをオフにする（オプション）
				// isHovered = false; 
			}
		}
		// 今のマウス位置を記憶
		prevMousePos_ = mousePos;
	}

	// === 3. 決定処理 ===
	bool nextSene = ins.IsTrgMouseLeft()
		|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	if (nextSene && isHovered)
	{
		if (selectIndex_ == static_cast<int>(LIST::ゲームに戻る))
		{
			isAlive_ = false;
		}
		// 念のため else if に変更
		else if (selectIndex_ == static_cast<int>(LIST::タイトルに戻る))
		{
			sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
		}
		else if (selectIndex_ == static_cast<int>(LIST::ゲーム終了))
		{
			Application::GetInstance().SetIsEnd(true);
		}
	}
}

void PauseScene::Draw(void)
{
	const auto centerY = Application::SCREEN_SIZE_Y / 2;
	auto height = 200;

	// 背景の半透明黒（セロファン）
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 168);
	DrawBoxAA(
		height, centerY - height * 1.0f,
		Application::SCREEN_SIZE_X - height, centerY + height * 1.0f,
		0x000000,
		true, 1.0f
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 白枠
	DrawBoxAA(
		height, centerY - height * 1.0f,
		Application::SCREEN_SIZE_X - height, centerY + height * 1.0f,
		0xffffff,
		false,
		3.0f);

	// 選択中の項目のY座標を算出
	float selectImgY = Application::SCREEN_SIZE_Y / 2 + 105.0f;
	if (selectIndex_ >= 0 && selectIndex_ < LIST_MAX)
	{
		int itemPosY = static_cast<int>(Application::SCREEN_SIZE_Y / 2.0f - 90) + (70 * selectIndex_);
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
		int posY = static_cast<int>(Application::SCREEN_SIZE_Y / 2.0f - 90) + (70 * i);

		DrawFormatStringToHandle(
			posX,
			posY,
			0xffffff,
			pauseFont_,
			pasueList_[i].c_str());
	}
}

void PauseScene::Release(void)
{
}
