#include <algorithm>

#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "UISt.h"

UISt::UISt(float pos1X, float pos1Y, float pos2X, float pos2Y, float imgSize, float maxHp)
	:
	pos1_(pos1X, pos1Y),
	pos2_(pos2X, pos2Y),
	imgSize_(imgSize),
	maxHp_(maxHp),
	currentHp_(maxHp)
{
}

UISt::~UISt(void)
{
}

void UISt::Update(void)
{
	// 必要ならここでアニメーション等を行う
}

void UISt::Draw(void)
{
	// 背景バー
	DrawBoxAA(
		pos1_.x - imgSize_, pos1_.y - imgSize_,
		pos2_.x + imgSize_, pos2_.y + imgSize_,
		0x000000, true);

	// スタミナバー（割合で描画）
	float ratio = 0.0f;
	if (maxHp_ > 0.0f) ratio = currentHp_ / maxHp_;
	ratio = fmaxf(0.0f, fminf(1.0f, ratio));

	float left = pos1_.x;
	float right = pos1_.x + (pos2_.x - pos1_.x) * ratio;

	// 緑系で表示
	DrawBoxAA(
		left, pos1_.y,
		right, pos2_.y,
		0x00ff00, true);
}

void UISt::SetHp(float delta)
{
	// delta は「減少量」として扱う
	currentHp_ -= delta;
	if (currentHp_ < 0.0f) currentHp_ = 0.0f;
	if (currentHp_ > maxHp_) currentHp_ = maxHp_;
}

void UISt::SetHpAbsolute(float hp)
{
	currentHp_ = hp;
	if (currentHp_ < 0.0f) currentHp_ = 0.0f;
	if (currentHp_ > maxHp_) currentHp_ = maxHp_;
}

void UISt::SetMaxHp(float maxHp)
{
	maxHp_ = maxHp;
	if (currentHp_ > maxHp_) currentHp_ = maxHp_;
}

void UISt::InitLoad(void)
{
}

void UISt::InitTransform(void)
{
}

void UISt::InitPost(void)
{
}
