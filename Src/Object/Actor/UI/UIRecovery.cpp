#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/FontManager.h"
#include "../../../Application.h"
#include "UIRecovery.h"

UIRecovery::UIRecovery(int cnt)
	:
	cnt_(cnt)
{
}

UIRecovery::~UIRecovery(void)
{
}

void UIRecovery::Update(void)
{
}

void UIRecovery::Draw(void)
{
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 8,
		Application::SCREEN_SIZE_Y / 1.5f,
		0.8f, 0.0f, imgBox_, true);

	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 8,
		Application::SCREEN_SIZE_Y / 1.5f,
		0.4f, 0.0f, imgBottle_, true);

	DrawFormatStringToHandle(
		Application::SCREEN_SIZE_X / 6.5f,
		Application::SCREEN_SIZE_Y / 1.5f,
		0xffffff,
		titleFont_,
		cntfont_.c_str());


}

void UIRecovery::InitLoad(void)
{
	// フォントハンドルの作成
	resMng_.Load(ResourceManager::SRC::FONT);
	titleFont_ = FontManager::GetInstance().CreateMyFont(L"KazukiReiwa", 50, 50);

	imgBox_ = resMng_.Load(ResourceManager::SRC::UI_BOX).handleId_;;
	imgBottle_ = resMng_.Load(ResourceManager::SRC::UI_RECOVERY_BOTTLE).handleId_;;
}

void UIRecovery::InitTransform(void)
{
}

void UIRecovery::InitPost(void)
{
	cntfont_ = std::to_wstring(cnt_);
}

void UIRecovery::SetBottleCnt(int cnt)
{
	if (cnt_ <= 0)return; 
	cnt_ -= cnt;
	cntfont_ = std::to_wstring(cnt_);
}
