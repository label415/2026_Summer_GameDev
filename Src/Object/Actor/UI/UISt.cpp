#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/FontManager.h"
#include "../../../Application.h"
#include "UISt.h"

UISt::UISt(float posX, float posY, float flameScl,
	float stSclX, float stSclY, float st)
	:
	stPos_(posX, posY),
	stCurrent_(st),
	flameScl_(flameScl),
	stSclX_(stSclX),
	stSclY_(stSclY)
{
	active_ = true;
}

UISt::~UISt(void){}

void UISt::Update(void){}

void UISt::Draw(void)
{
	// 背景バーの描画
	DrawRotaGraph(
		stPos_.x,
		stPos_.y,
		flameScl_, 0.0f, flameImg_, true);

	// 現在のスタミナ値に応じたバーの描画
	float hpRate = stCurrent_ / MAX_ST;
	float realHalfWidth = IMG_SIZE_X / stSclX_;
	float realHalfHeight = IMG_SIZE_Y / stSclY_;

	float left = stPos_.x - realHalfWidth;
	float top = stPos_.y - realHalfHeight;
	float bottom = stPos_.y + realHalfHeight;

	int srcWidth = static_cast<int>(IMG_SIZE_X * hpRate);
	float currentBarRight = left + (realHalfWidth * 2.0f * hpRate);

	// HPバー
	DrawRectExtendGraphF(
		left, top,
		currentBarRight, bottom,
		0, 0, srcWidth,
		static_cast<int>(IMG_SIZE_Y),
		suUiImg_, true);
}

void UISt::SetSt(float delta)
{
	// スタミナの増減
	stCurrent_ -= delta;
	if (stCurrent_ < MIN_ST)
	{
		stCurrent_ = MIN_ST;
	}
}

void UISt::SetHpAbsolute(float hp)
{
	// スタミナの絶対値設定
	stCurrent_ += hp;
	if (stCurrent_ > MAX_ST) stCurrent_ = MAX_ST;
}

void UISt::InitLoad(void)
{
	// リソースのロード
	flameImg_ = resMng_.Load(ResourceManager::SRC::UI_BAR_FRAME).handleId_;
	suUiImg_ = resMng_.Load(ResourceManager::SRC::UI_ST_BAR).handleId_;
}

void UISt::InitTransform(void){}

void UISt::InitPost(void){}
