#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/FontManager.h"
#include "../../../Application.h"
#include "UISt.h"

UISt::UISt(void)
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
	DrawRotaGraph(
		pos_.x,
		pos_.y,
		0.7f, 0.0f, stUi1_, true);

	float left = pos_.x - IMG_SIZE_X / 2.9f;
	float right = pos_.x + IMG_SIZE_X / 2.9f * (st_ * 0.01f);

	DrawExtendGraphF(
		pos_.x - IMG_SIZE_X / 2.9f,
		pos_.y - IMG_SIZE_Y / 4,
		right,
		pos_.y + IMG_SIZE_Y / 4,
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
	pos_ = { Application::SCREEN_SIZE_X / 4 , 60 };
	st_ = MAX_ST;
}
