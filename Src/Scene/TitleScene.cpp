#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/FontManager.h"
#include "../Utility/AsoUtility.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	selectIndex_(0),
	isHovered(true)
{
}

TitleScene::~TitleScene(void) {}

void TitleScene::Load(void)
{
	// フォントのロード
	resMng_.Load(ResourceManager::SRC::FONT);
	pauseFont_ = fontMng_.GetInstance().CreateMyFont(fontName_, FONT_SIZE, FONT_THICKNESS);

	// 画像のロード
	titleImg_ = resMng_.Load(ResourceManager::SRC::TITLE_IMG).handleId_;
	selectImg_ = resMng_.Load(ResourceManager::SRC::TITLE_SELECT).handleId_;

	// 各UIコライダー生成
	for (int i = 0; i < LIST_MAX; ++i)
	{
		uiBoxs_[i] = new ColliderBox2D(
			ColliderBase2D::TAG::UI,
			Vector2F(
				Application::HALF_SCREEN_SIZE_X - BOX_ADJUST_X,
				(Application::HALF_SCREEN_SIZE_Y + BOX_ADJUST_Y) + (BOX_Y_INTERVAL * i)),
			BOX_WIDTH, BOX_HEIGHT);
	}
}

void TitleScene::LoadEnd(void)
{
	// BGM再生
	bgm_ = resMng_.Load(ResourceManager::SRC::TITLE_BGM).handleId_;
	volume_ = BGM_VOLUME;
	SoundManager::GetInstance().PlayBGM(bgm_, volume_);
}

void TitleScene::Update(void)
{
	// 入力管理インスタンスを取得
	auto& ins = InputManager::GetInstance();

	// マウス判定
	Vector2 mPos = ins.GetMousePos();
	for (int i = 0; i < LIST_MAX; ++i)
	{
		if (uiBoxs_[i] && uiBoxs_[i]->Contains(mPos.x, mPos.y))
		{
			ins.SetMouseFlage(true);
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
		ins.SetMouseFlage(false);

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
			Application::GetInstance().SetIsEnd(true);
		}
	}
}

void TitleScene::Draw(void)
{
	// タイトル画像
	DrawRotaGraph(
		Application::HALF_SCREEN_SIZE_X,
		Application::HALF_SCREEN_SIZE_Y - TITLE_IMG_ADJUST_Y,
		TITLE_IMG_SIZE, 0.0f, titleImg_, true);

	// カーソル画像
	if (isHovered && uiBoxs_[selectIndex_])
	{
		float centerY =
			uiBoxs_[selectIndex_]->Top()
			+ (uiBoxs_[selectIndex_]->Bottom()
				- uiBoxs_[selectIndex_]->Top()) * 0.5f;

		DrawRotaGraph(
			Application::HALF_SCREEN_SIZE_X,
			static_cast<int>(centerY),
			SELECT_IMG_SIZE, 0.0f, selectImg_, true
		);
	}

	// 文字とコライダー描画
	for (int i = 0; i < LIST_MAX; ++i)
	{
		// 文字描画
		int strW = GetDrawStringWidthToHandle(pasueList_[i].c_str(), -1, pauseFont_);
		int posX = static_cast<int>(Application::SCREEN_SIZE_X - strW) / FONT_ADJUST_X;
		int posY = static_cast<int>(
			(Application::HALF_SCREEN_SIZE_Y + FONT_ADJUST_Y) + (FONT_INTERVAL * i));

		DrawFormatStringToHandle(
			posX, posY,
			FONT_COLOR,
			pauseFont_,
			pasueList_[i].c_str());

#ifdef _DEBUG
		// ボックスコライダー描画
		if (uiBoxs_[i])
		{
			uiBoxs_[i]->Draw();
			uiBoxs_[i]->SetValid(isHovered && (selectIndex_ == i));
		}
#endif
	}
}

void TitleScene::Release(void)
{
	// フォントハンドルの削除
	DeleteFontToHandle(pauseFont_);
}