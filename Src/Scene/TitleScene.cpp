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
	bgm_ = resMng_.Load(ResourceManager::SRC::SE_5).handleId_;
	// フォントハンドルの作成
	resMng_.Load(ResourceManager::SRC::FONT);
	titleFont_ = fontMng_.GetInstance().CreateMyFont(L"KazukiReiwa", 140, 140);
	pauseFont_ = fontMng_.GetInstance().CreateMyFont(L"KazukiReiwa", 25, 25);

	volume_ = 100;

	// 直接インスタンスを取得して呼び出すことで null 参照を防ぐ
	SoundManager::GetInstance().PlayBGM(bgm_, volume_);
}

void TitleScene::LoadEnd(void)
{
}

void TitleScene::Update(void)
{

	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	auto& son = SoundManager::GetInstance();

	 bool nextSene = ins.IsTrgDown(KEY_INPUT_SPACE)
			|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::DOWN);

	if (nextSene)
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}

}

void TitleScene::Draw(void)
{
	/*int posX = Application::SCREEN_SIZE_X / 2 - 140;
	int posY = Application::SCREEN_SIZE_Y / 2 + 95.0f;
	DrawRotaGraph3(posX, posY, 0.0f, 0.0f, 0.2f, 0.05f, 0.0f, i, true);*/

	DrawFormatStringToHandle(
		Application::SCREEN_SIZE_X / 6,
		Application::SCREEN_SIZE_Y / 3,
		0xffffff,
		titleFont_,
		pasueList_[static_cast<int>(LIST::ASOSEOUL)].c_str());


	for (int i = 1; i < LIST_MAX; ++i)
	{
		//座標位置を設定
		/*int posX = Application::SCREEN_SIZE_X / 2 - 110;
		int posY = (Application::SCREEN_SIZE_Y / 2 +50+ (50.0f * i));*/
		int posX = Application::SCREEN_SIZE_X / 2 - 300;
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

void TitleScene::Release(void)
{
	DeleteFontToHandle(titleFont_);
	DeleteFontToHandle(pauseFont_);
}
