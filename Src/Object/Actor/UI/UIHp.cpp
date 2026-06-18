#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "UIHp.h"

UIHp::UIHp(float pos1X, float pos1Y, float pos2X, float pos2Y, float imgSize)
	:
	pos1_(pos1X, pos1Y),
	pos2_(pos2X, pos2Y),
	imgSize_(imgSize),
	currentHp_(0.0f)
{
}

UIHp::~UIHp(void)
{
}

void UIHp::Update(void)
{
}

void UIHp::Draw(void)
{
	// 背景バー
	DrawBoxAA(
		pos1_.x - imgSize_, pos1_.y - imgSize_,
		pos2_.x + imgSize_, pos2_.y + imgSize_,
		0x000000, true);

	// HPバー
	DrawBoxAA(
		pos1_.x, pos1_.y,
		pos2_.x + currentHp_, pos2_.y,
		0xff0000, true);
}

void UIHp::SetHp(int currentHp)
{
	currentHp_ = currentHp;
}

void UIHp::InitLoad(void)
{
}

void UIHp::InitTransform(void)
{
}

void UIHp::InitPost(void)
{
}
