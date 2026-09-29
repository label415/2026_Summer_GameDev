#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/FontManager.h"
#include "../Utility/AsoUtility.h"
#include "PauseScene.h"

PauseScene::PauseScene(void)
	:
	selectIndex_(0),
	isHovered(true),
	isPauseScene_(false)
{
}

PauseScene::~PauseScene(void){}

void PauseScene::LoadEnd(void)
{
	// フォントハンドルの作成
	resMng_.Load(ResourceManager::SRC::FONT);
	pauseFont_ = fontMng_.GetInstance().CreateMyFont(fontName_, FONT_SIZE, FONT_THICKNESS);

	// 画像のロード
	selectImg_ = resMng_.Load(ResourceManager::SRC::UI_SELECTION_CURSOR).handleId_;

	// 各UIコライダー生成
	for (int i = 0; i < LIST_MAX; ++i)
	{
		uiBoxs_[i] = new ColliderBox2D(
			ColliderBase2D::TAG::UI,
			Vector2F(
				Application::HALF_SCREEN_SIZE_X - BOX_ADJUST_X,
				(Application::HALF_SCREEN_SIZE_Y - BOX_ADJUST_Y) + (BOX_Y_INTERVAL * i)),
			BOX_WIDTH, BOX_HEIGHT);
	}
}

void PauseScene::Update(void)
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

	// 決定入力があり、かつ選択中のUIが表示されている時
	bool isDecide = ins.IsTrgMouseLeft()
		|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
	if (isDecide && isHovered)
	{
		// 選択肢に応じてシーン遷移またはアプリ終了
		if (selectIndex_ == static_cast<int>(LIST::ゲームに戻る))
		{
			isPauseScene_ = false;
		}
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
	// ポーズ画面半透明黒
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, PAUSE_BLENDPARAM);
	DrawBoxAA(
		PAUSE_HEIGHT,
		Application::HALF_SCREEN_SIZE_Y - PAUSE_HEIGHT,
		Application::SCREEN_SIZE_X - PAUSE_HEIGHT,
		Application::HALF_SCREEN_SIZE_Y + PAUSE_HEIGHT,
		PAUSE_COLOR,
		true, PAUSE_SIZE
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// ポーズ画面枠
	DrawBoxAA(
		PAUSE_HEIGHT,
		Application::HALF_SCREEN_SIZE_Y - PAUSE_HEIGHT,
		Application::SCREEN_SIZE_X - PAUSE_HEIGHT,
		Application::HALF_SCREEN_SIZE_Y + PAUSE_HEIGHT,
		PAUSE_FRAME_COLOR,
		false,
		PAUSE_FRAME_SIZE);

	// カーソル画像
	if (isHovered && uiBoxs_[selectIndex_])
	{
		float centerY =
			uiBoxs_[selectIndex_]->Top()
			+ (uiBoxs_[selectIndex_]->Bottom()
				- uiBoxs_[selectIndex_]->Top()) * SELECT_IMG_ADJUST_Y;

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
			(Application::HALF_SCREEN_SIZE_Y - FONT_ADJUST_Y) + (FONT_INTERVAL * i));

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

void PauseScene::Release(void){}
