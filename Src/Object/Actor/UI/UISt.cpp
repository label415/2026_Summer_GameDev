#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/FontManager.h"
#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "UISt.h"

UISt::UISt(float posX, float posY, float frameScl,
	float stSclX, float stSclY, float st)
	: stPos_(static_cast<int>(posX), static_cast<int>(posY))
	, stCurrent_(st)
	, frameScl_(frameScl)
	, stSclX_(stSclX)
	, stSclY_(stSclY)
	, frameImg_(-1)
	, stUiImg_(-1)
{
	active_ = true;
}

void UISt::Draw(void)
{
	// 背景バーの描画
	DrawRotaGraph(
		stPos_.x,
		stPos_.y,
		frameScl_, 0.0f, frameImg_, true);

	// 現在のスタミナ値に応じたバーの描画
	float hpRate = stCurrent_ / MAX_ST;
	float realHalfWidth = IMG_SIZE_X / stSclX_;
	float realHalfHeight = IMG_SIZE_Y / stSclY_;

	float left = static_cast<float>(stPos_.x) - realHalfWidth;
	float top = static_cast<float>(stPos_.y) - realHalfHeight;
	float bottom = static_cast<float>(stPos_.y) + realHalfHeight;

	int srcWidth = static_cast<int>(IMG_SIZE_X * hpRate);
	float currentBarRight = left + (realHalfWidth * AsoUtility::DOUBLE * hpRate);

	// HPバー
	DrawRectExtendGraphF(
		left, top,
		currentBarRight, bottom,
		0, 0, srcWidth,
		static_cast<int>(IMG_SIZE_Y),
		stUiImg_, true);
}

void UISt::ConsumeSt(float delta)
{
	// スタミナの増減
	stCurrent_ -= delta;
	if (stCurrent_ < MIN_ST)
	{
		stCurrent_ = MIN_ST;
	}
}

void UISt::RecoverSt(float st)
{
	// スタミナを回復させる
	stCurrent_ += st;
	if (stCurrent_ > MAX_ST) stCurrent_ = MAX_ST;
}

void UISt::InitLoad(void)
{
	// リソースのロード
	frameImg_ = resMng_.Load(ResourceManager::SRC::UI_BAR_FRAME).handleId_;
	stUiImg_ = resMng_.Load(ResourceManager::SRC::UI_ST_BAR).handleId_;
}
