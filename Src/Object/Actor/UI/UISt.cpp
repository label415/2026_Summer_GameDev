#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/FontManager.h"
#include "../../../Application.h"
#include "UISt.h"

UISt::UISt(float posX, float posY, float flameScl,
	float stSclX, float stSclY, float st)
	:
	pos_(posX, posY),
	st_(st),
	flameScl_(flameScl),
	stSclX_(stSclX),
	stSclY_(stSclY)
{
	active_ = true;
}

UISt::~UISt(void)
{
}

void UISt::Update(void)
{
}

void UISt::Draw(void)
{
	// îwåiÉoÅ[ÇÃï`âÊ
	DrawRotaGraph(
		pos_.x,
		pos_.y,
		flameScl_, 0.0f, stUi1_, true);

	float hpRate = st_ / 100.0f;
	float realHalfWidth = IMG_SIZE_X / stSclX_;
	float realHalfHeight = IMG_SIZE_Y / stSclY_;

	float left = pos_.x - realHalfWidth;
	float top = pos_.y - realHalfHeight;
	float bottom = pos_.y + realHalfHeight;

	int srcWidth = static_cast<int>(IMG_SIZE_X * hpRate);
	float currentBarRight = left + (realHalfWidth * 2.0f * hpRate);

	// HPÉoÅ[
	DrawRectExtendGraphF(
		left, top,
		currentBarRight, bottom,
		0, 0, srcWidth,
		static_cast<int>(IMG_SIZE_Y),
		stUi2_, true);
}

void UISt::SetSt(float delta)
{
	st_ -= delta;
	if (st_ < MIN_ST)
	{
		st_ = MIN_ST;
	}
}

void UISt::SetHpAbsolute(float hp)
{
	st_ += hp;
	if (st_ > MAX_ST) st_ = MAX_ST;
}

void UISt::InitLoad(void)
{
	stUi1_ = resMng_.Load(ResourceManager::SRC::HP_1).handleId_;
	stUi2_ = resMng_.Load(ResourceManager::SRC::ST).handleId_;
}

void UISt::InitTransform(void)
{
}

void UISt::InitPost(void)
{
}
