#include <cmath>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "UIHp.h"

UIHp::UIHp(float pos1X, float pos1Y, float pos2X, float pos2Y, float imgSize, float maxHp)
	:
	pos1_(pos1X, pos1Y),
	pos2_(pos2X, pos2Y),
	imgSize_(imgSize),
	maxHp_(maxHp),
	currentHp_(maxHp)
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
	// 背景バー
	DrawBoxAA(
		pos1_.x - imgSize_, pos1_.y - imgSize_,
		pos2_.x + imgSize_, pos2_.y + imgSize_,
		0x000000, true);

	// HPバー（割合で描画）
	float ratio = 0.0f;
	if (maxHp_ > 0.0f) ratio = currentHp_ / maxHp_;
	ratio = fmaxf(0.0f, fminf(1.0f, ratio));

	float left = pos1_.x;
	float right = pos1_.x + (pos2_.x - pos1_.x) * ratio;

	DrawBoxAA(
		left, pos1_.y,
		right, pos2_.y,
		0xff0000, true);

	if (left >= right)
	{
		active_ = false;
	}
}

void UIHp::SetHp(float delta)
{
	// delta は「減らす量」として扱う（ダメージ）
	currentHp_ -= delta;
	if (currentHp_ < 0.0f) currentHp_ = 0.0f;
	if (currentHp_ > maxHp_) currentHp_ = maxHp_;
}

void UIHp::SetHpAbsolute(float hp)
{
	currentHp_ += hp;
	if (currentHp_ < 0.0f) currentHp_ = 0.0f;
	if (currentHp_ > maxHp_) currentHp_ = maxHp_;
}

void UIHp::SetMaxHp(float maxHp)
{
	maxHp_ = maxHp;
	if (currentHp_ > maxHp_) currentHp_ = maxHp_;
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
