#include <DxLib.h>
#include "../Manager/Common/InputManager.h"
#include "../Manager/Common/SceneManager.h"
#include "../Manager/Common/ResourceManager.h"
#include "../Manager/Common/SoundManager.h"
#include "../Manager/Game/UiManager.h"
#include "../UI/ImgUI.h"
#include "../Utility/AsoUtility.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	: SceneBase()
	, uiManager_(nullptr)
	, selectIndex_(0)
	, isHovered(true)
	, isStickInput_(false)
	, titleFont_(-1)
{
}

void TitleScene::Load(void)
{
	// UI管理の生成
	uiManager_ = std::make_unique<UiManager>();

	// 画像のロードとImgUI登録
	int titleHandle = resMng_.Load(ResourceManager::SRC::IMG_TITLE).handleId_;
	int selectHandle = resMng_.Load(ResourceManager::SRC::UI_SELECTION_CURSOR).handleId_;
	constexpr Vector2F halfScreen = Application::HALF_SCREEN_SIZE;

	uiManager_->Add(
		static_cast<int>(UI_ID::TITLE_LOGO),
		std::make_unique<ImgUI>(
			titleHandle,
			Vector2F(
				halfScreen.x,
				halfScreen.y - TITLE_IMG_ADJUST_Y),
			TITLE_IMG_SIZE));

	uiManager_->Add(
		static_cast<int>(UI_ID::CURSOR),
		std::make_unique<ImgUI>(
			selectHandle,
			Vector2F(halfScreen.x, 0.0f),
			SELECT_IMG_SIZE));

	// フォントのロード
	titleFont_ = resMng_.LoadFont(ResourceManager::SRC::FONT, FONT_SIZE, FONT_THICKNESS);

	// 各UIコライダー生成
	for (int i = 0; i < LIST_MAX; ++i)
	{
		uiBoxs_[i] = std::make_unique<ColliderBox2D>(
			ColliderBase2D::TAG::UI,
			Vector2F(
				Application::HALF_SCREEN_SIZE.x - BOX_ADJUST.x,
				(Application::HALF_SCREEN_SIZE.y + BOX_ADJUST.y) + (BOX_Y_INTERVAL * i)),
			BOX_SIZE);
	}
}

void TitleScene::LoadEnd(void)
{
	// BGM再生
	SoundManager::GetInstance().PlaySoundId(SoundManager::SOUND_ID::BGM_TITLE, true);

	// マウスカーソル表示を有効化
	InputManager::GetInstance().SetMouseFlag(true);
}

void TitleScene::Update(void)
{
	// 入力管理インスタンスを取得
	auto& ins = InputManager::GetInstance();

	// マウス判定
	Vector2F mPos = ins.GetMousePos();
	for (int i = 0; i < LIST_MAX; ++i)
	{
		if (uiBoxs_[i] && uiBoxs_[i]->Contains(mPos))
		{
			ins.SetMouseFlag(true);
			selectIndex_ = i;
			isHovered = true;
			break;
		}
	}

	// パッド入力
	auto pad = ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
	float stickZ = ins.GetDirectionXZAKey(pad.AKeyLX, pad.AKeyLY).z;
	int dirY = (stickZ > STICK_DEAD_ZONE) ? -1 : (stickZ < -STICK_DEAD_ZONE) ? 1 : 0;

	// スティック入力があった場合、選択インデックスを更新
	if (dirY != 0)
	{
		ins.SetMouseFlag(false);

		if (!isStickInput_)
		{
			selectIndex_ = (selectIndex_ + dirY + LIST_MAX) % LIST_MAX;
			isStickInput_ = isHovered = true;
		}
	}
	else
	{
		isStickInput_ = false;
	}

	// カーソルUIの座標・表示状態更新およびUI一括更新
	if (uiManager_)
	{
		bool showCursor = (isHovered && uiBoxs_[selectIndex_]);
		uiManager_->SetActive(static_cast<int>(UI_ID::CURSOR), showCursor);
		if (showCursor)
		{
			float centerY =
				uiBoxs_[selectIndex_]->Top()
				+ (uiBoxs_[selectIndex_]->Bottom()
					- uiBoxs_[selectIndex_]->Top()) * SELECT_IMG_ADJUST_Y;
			constexpr Vector2F halfScreen = Application::HALF_SCREEN_SIZE;
			uiManager_->SetPos(
				static_cast<int>(UI_ID::CURSOR),
				Vector2F(halfScreen.x, centerY));
		}
		uiManager_->Update();
	}

	// シーン遷移
	bool isDecide = ins.IsTrgMouseLeft()
		|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	// 決定入力があり、かつ選択中のUIが表示されている時
	if (isDecide && isHovered)
	{
		// 選択肢に応じてシーン遷移またはアプリ終了
		if (selectIndex_ == static_cast<int>(LIST::始める))
		{
			sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
		}
		else if (selectIndex_ == static_cast<int>(LIST::ゲーム終了))
		{
			Application::GetInstance().SetIsEnd();
		}
	}
}

void TitleScene::Draw(void)
{
	// 登録されたUI（タイトルロゴ・カーソル）を一括描画
	if (uiManager_)
	{
		uiManager_->Draw();
	}

	// 文字とコライダー描画
	for (int i = 0; i < LIST_MAX; ++i)
	{
		// 文字描画
		int strW = GetDrawStringWidthToHandle(titleList_[i].c_str(), -1, titleFont_);
		Vector2F pos = {
			(Application::SCREEN_SIZE.x - strW) / FONT_ADJUST.x,
			(Application::HALF_SCREEN_SIZE.y + FONT_ADJUST.y) + (FONT_INTERVAL * i)
		};

		// 文字描画
		DrawFormatStringFToHandle(
			pos.x, pos.y,
			FONT_COLOR,
			titleFont_,
			titleList_[i].c_str());

	}
}

void TitleScene::Release(void)
{
	// UI管理の解放とフォントハンドル参照のリセット
	if (uiManager_)
	{
		uiManager_->Release();
	}
	titleFont_ = -1;
}
