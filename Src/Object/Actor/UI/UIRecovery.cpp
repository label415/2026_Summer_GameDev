#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/FontManager.h"
#include "../../../Application.h"
#include "UIRecovery.h"

UIRecovery::UIRecovery(int cnt)
	:
	bottleCount_(cnt)
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
	// アイテムボックスを描画
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / UI_POS_ADJ_X,
		Application::SCREEN_SIZE_Y / UI_POS_ADJ_Y,
		BOX_SIZE, 0.0f, imgBox_, true);

	// 回復瓶を描画
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / UI_POS_ADJ_X,
		Application::SCREEN_SIZE_Y / UI_POS_ADJ_Y,
		BOTTLE_SIZE, 0.0f, imgBottle_, true);

	// 回復瓶の数を描画
	DrawFormatStringToHandle(
		Application::SCREEN_SIZE_X / FONT_POS_ADJ_X,
		Application::SCREEN_SIZE_Y / FONT_POS_ADJ_Y,
		FONT_COLOR,
		font_,
		cntfont_.c_str());
}

void UIRecovery::InitLoad(void)
{
	// フォントハンドルの作成
	resMng_.Load(ResourceManager::SRC::FONT);
	font_ = FontManager::GetInstance().CreateMyFont(fontName_, FONT_SIZE, FONT_SIZE);

	// アイテムボックスと回復瓶の画像ハンドルを取得
	imgBox_ = resMng_.Load(ResourceManager::SRC::UI_ITEMBOX).handleId_;
	imgBottle_ = resMng_.Load(ResourceManager::SRC::UI_RECOVERY_BOTTLE).handleId_;
}

void UIRecovery::InitTransform(void){}

void UIRecovery::InitPost(void)
{
	// 回復瓶の数を文字列に変換
	cntfont_ = std::to_wstring(bottleCount_);
}

void UIRecovery::SetBottleCnt(int cnt)
{
	// 回復瓶の数を減らす
	if (bottleCount_ <= 0)return; 
	bottleCount_ -= cnt;
	cntfont_ = std::to_wstring(bottleCount_);
}
