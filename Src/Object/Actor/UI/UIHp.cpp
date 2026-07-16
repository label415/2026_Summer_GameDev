#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/FontManager.h"
#include "../../../Application.h"
#include "UIHp.h"

UIHp::UIHp(float posX, float posY,
	float flameScl, float hpSclX, float hpSclY, float hp)
	:
	pos_(posX, posY),
	hp_(hp),
	flameScl_(flameScl),
	hpSclX_(hpSclX),
	hpSclY_(hpSclY)
{
	active_ = true;
}

UIHp::~UIHp(void)
{
}

void UIHp::Update(void)
{
}

void UIHp::Draw(void)
{
	// îwåiÉoÅ[ÇÃï`âÊ
	DrawRotaGraph(
		pos_.x,
		pos_.y,
		flameScl_, 0.0f, hpUi1_, true);

	float hpRate = hp_ / 100.0f;
	float realHalfWidth  = IMG_SIZE_X / hpSclX_;
	float realHalfHeight = IMG_SIZE_Y / hpSclY_;

	float left   = pos_.x - realHalfWidth;
	float top    = pos_.y - realHalfHeight;
	float bottom = pos_.y + realHalfHeight;

	int srcWidth = static_cast<int>(IMG_SIZE_X * hpRate);
	float currentBarRight = left + (realHalfWidth * 2.0f * hpRate);

	// HPÉoÅ[
	DrawRectExtendGraphF(
		left, top,
		currentBarRight, bottom, 
		0, 0, srcWidth, 
		static_cast<int>(IMG_SIZE_Y),
		hpUi2_, true);
}

void UIHp::SetHp(float delta)
{
	hp_ -= delta;
	if (hp_ <= 0.0f)
	{
		hp_ = 0.0f;
		active_ = false;
	}
}

void UIHp::SetHpAbsolute(float hp)
{
	hp_ += hp;
	if (hp_ >= MAX_HP) hp_ = MAX_HP;
}

void UIHp::InitLoad(void)
{
	hpUi1_ = resMng_.Load(ResourceManager::SRC::HP_1).handleId_;
	hpUi2_ = resMng_.Load(ResourceManager::SRC::HP_2).handleId_;
}

void UIHp::InitTransform(void)
{
}

void UIHp::InitPost(void)
{
}
