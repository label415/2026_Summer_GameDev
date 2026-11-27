#include <DxLib.h>
#include "../Manager/Common/InputManager.h"
#include "../Manager/Common/SceneManager.h"
#include "../Manager/Common/ResourceManager.h"
#include "../Manager/Common/SoundManager.h"
#include "../Object/Camera.h"
#include "../Manager/Game/UiManager.h"
#include "../UI/ImgUI.h"
#include "../Utility/AsoUtility.h"
#include "PauseScene.h"

PauseScene::PauseScene(void)
	: SceneBase()
	, uiManager_(nullptr)
	, selectIndex_(0)
	, isHovered(true)
	, isStickInput_(false)
	, pauseFont_(-1)
	, uiBoxs_()
{
}

void PauseScene::LoadEnd(void)
{
	// UI管理の生成およびカーソルImgUIの登録
	uiManager_ = std::make_unique<UiManager>();
	int selectHandle = resMng_.Load(ResourceManager::SRC::UI_SELECTION_CURSOR).handleId_;
	constexpr Vector2F halfScreen = Application::HALF_SCREEN_SIZE;
	uiManager_->Add(
		UI_ID_CURSOR,
		std::make_unique<ImgUI>(
			selectHandle,
			Vector2F(halfScreen.x, 0.0f),
			SELECT_IMG_SIZE));

	// フォントのロード
	pauseFont_ = resMng_.LoadFont(ResourceManager::SRC::FONT, FONT_SIZE, FONT_THICKNESS);

	// 各UIコライダー生成
	for (int i = 0; i < LIST_MAX; ++i)
	{
		uiBoxs_[i] = std::make_unique<ColliderBox2D>(
			ColliderBase2D::TAG::UI,
			Vector2F(
				Application::HALF_SCREEN_SIZE.x - BOX_ADJUST.x,
				(Application::HALF_SCREEN_SIZE.y - BOX_ADJUST.y) + (BOX_Y_INTERVAL * i)),
			BOX_SIZE);
	}

	// マウス入力を有効化
	InputManager::GetInstance().SetMouseFlag(true);
}

void PauseScene::Update(void)
{
	//ポーズシーン中は、SEを停止する
	SoundManager::GetInstance().AllStopSE();

	// 入力管理インスタンスを取得
	auto& ins = InputManager::GetInstance();

	// マウス判定
	Vector2F mPos = ins.GetMousePos();
	for (int i = 0; i < LIST_MAX; ++i)
	{
		// マウスがUIコライダー内にあるか判定
		if (uiBoxs_[i] && uiBoxs_[i]->Contains(mPos))
		{
			// マウス入力を有効化
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
		// マウス入力を無効化
		ins.SetMouseFlag(false);

		// スティック入力が初めての入力である場合、選択インデックスを更新
		if (!isStickInput_)
		{
			// 選択インデックスを更新し、スティック入力フラグを立てる
			selectIndex_ = (selectIndex_ + dirY + LIST_MAX) % LIST_MAX;
			isStickInput_ = isHovered = true;
		}
	}
	else
	{
		// スティック入力がない場合、フラグをリセット
		isStickInput_ = false;
	}

	// カーソルUIの表示・座標更新およびUI一括更新
	if (uiManager_)
	{
		bool showCursor = (isHovered && uiBoxs_[selectIndex_]);
		uiManager_->SetActive(UI_ID_CURSOR, showCursor);
		if (showCursor)
		{
			float centerY =
				uiBoxs_[selectIndex_]->Top()
				+ (uiBoxs_[selectIndex_]->Bottom()
					- uiBoxs_[selectIndex_]->Top()) * SELECT_IMG_ADJUST_Y;
			constexpr Vector2F halfScreen = Application::HALF_SCREEN_SIZE;
			uiManager_->SetPos(
				UI_ID_CURSOR,
				Vector2F(halfScreen.x, centerY));
		}
		uiManager_->Update();
	}

	// 決定入力があり、かつ選択中のUIが表示されている時
	bool isDecide = ins.IsTrgMouseLeft()
		|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
	if (isDecide && isHovered)
	{
		// 選択肢に応じてシーン遷移またはアプリ終了
		if (selectIndex_ == static_cast<int>(LIST::ゲームに戻る))
		{
			InputManager::GetInstance().SetMouseFlag(false);
			sceMng_.PopScene();
		}
		// タイトルに戻るを選択した場合、タイトルシーンに遷移する
		else if (selectIndex_ == static_cast<int>(LIST::タイトルに戻る))
		{
			sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
		}
		// ゲーム終了を選択した場合、アプリケーションを終了する
		else if (selectIndex_ == static_cast<int>(LIST::ゲーム終了))
		{
			Application::GetInstance().SetIsEnd();
		}
	}
}

void PauseScene::Draw(void)
{
	// ポーズ画面半透明黒
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, PAUSE_BLENDPARAM);
	DrawBoxAA(
		PAUSE_HEIGHT,
		Application::HALF_SCREEN_SIZE.y - PAUSE_HEIGHT,
		Application::SCREEN_SIZE.x - PAUSE_HEIGHT,
		Application::HALF_SCREEN_SIZE.y + PAUSE_HEIGHT,
		PAUSE_COLOR,
		true, PAUSE_SIZE
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// ポーズ画面枠
	DrawBoxAA(
		PAUSE_HEIGHT,
		Application::HALF_SCREEN_SIZE.y - PAUSE_HEIGHT,
		Application::SCREEN_SIZE.x - PAUSE_HEIGHT,
		Application::HALF_SCREEN_SIZE.y + PAUSE_HEIGHT,
		PAUSE_FRAME_COLOR,
		false,
		PAUSE_FRAME_SIZE);

	// カーソルUIを一括描画
	if (uiManager_)
	{
		uiManager_->Draw();
	}

	// 各ポーズフォント
	for (int i = 0; i < LIST_MAX; ++i)
	{
		// 文字の描画位置を計算
		int strW = GetDrawStringWidthToHandle(pauseList_[i].c_str(), -1, pauseFont_);
		Vector2F pos = {
			(Application::SCREEN_SIZE.x - strW) / FONT_ADJUST.x,
			(Application::HALF_SCREEN_SIZE.y - FONT_ADJUST.y) + (FONT_INTERVAL * i)
		};

		// ポーズフォント描画
		DrawFormatStringFToHandle(
			pos.x, pos.y,
			FONT_COLOR,
			pauseFont_,
			pauseList_[i].c_str());
	}
}

void PauseScene::Release(void)
{
	// UI管理の解放とポーズ画面解放時のフォントハンドル参照リセット
	if (uiManager_)
	{
		uiManager_->Release();
	}
	pauseFont_ = -1;
}
