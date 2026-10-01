#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/FontManager.h"
#include "../../../Application.h"
#include "UIHp.h"

UIHp::UIHp(float posX, float posY,
	float flameScl, float hpSclX, float hpSclY, float hp)
	:
	uiPos_(posX, posY),
	hpCurrent_(hp),
	flameScl_(flameScl),
	hpSclX_(hpSclX),
	hpSclY_(hpSclY)
{
	// 初期化
	active_ = true;
}

UIHp::~UIHp(void){}

void UIHp::Update(void){}

void UIHp::Draw(void)
{
	// 背景バーの描画
	DrawRotaGraph(
		uiPos_.x,
		uiPos_.y,
		flameScl_, 0.0f, flameImg_, true);

	// HPバーの描画
	float hpRate = hpCurrent_ / MAX_HP;
	float realHalfWidth  = IMG_SIZE_X / hpSclX_;
	float realHalfHeight = IMG_SIZE_Y / hpSclY_;

	float left   = uiPos_.x - realHalfWidth;
	float top    = uiPos_.y - realHalfHeight;
	float bottom = uiPos_.y + realHalfHeight;

	int srcWidth = static_cast<int>(IMG_SIZE_X * hpRate);
	float currentBarRight = left + (realHalfWidth * 2.0f * hpRate);

	// HPバー
	DrawRectExtendGraphF(
		left, top,
		currentBarRight, bottom, 
		0, 0, srcWidth, 
		static_cast<int>(IMG_SIZE_Y),
		hpImg_, true);
}

void UIHp::SetHp(float delta)
{
	// HPを減少させる
	hpCurrent_ -= delta;
	if (hpCurrent_ <= 0.0f)
	{
		hpCurrent_ = 0.0f;
		active_ = false;
	}
}

void UIHp::SetHpAbsolute(float hp)
{
	// HPを回復させる
	hpCurrent_ += hp;
	if (hpCurrent_ >= MAX_HP) hpCurrent_ = MAX_HP;
}

void UIHp::InitLoad(void)
{
	// リソースのロード
	flameImg_ = resMng_.Load(ResourceManager::SRC::UI_BAR_FRAME).handleId_;
	hpImg_ = resMng_.Load(ResourceManager::SRC::UI_HP_BAR).handleId_;
}

void UIHp::InitTransform(void){}

void UIHp::InitPost(void){}
