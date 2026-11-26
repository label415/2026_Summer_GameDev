#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/FontManager.h"
#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "UIHp.h"

UIHp::UIHp(float posX, float posY,
	float frameScl, float hpSclX, float hpSclY, float hp)
	: uiPos_(static_cast<int>(posX), static_cast<int>(posY))
	, hpCurrent_(hp)
	, frameScl_(frameScl)
	, hpSclX_(hpSclX)
	, hpSclY_(hpSclY)
	, frameImg_(-1)
	, hpImg_(-1)
{
	// 初期化
	active_ = true;
}

void UIHp::Draw(void)
{
	// 背景バーの描画
	DrawRotaGraph(
		uiPos_.x,
		uiPos_.y,
		frameScl_, 0.0f, frameImg_, true);

	// HPバーの描画
	float hpRate = hpCurrent_ / MAX_HP;
	float realHalfWidth  = IMG_SIZE_X / hpSclX_;
	float realHalfHeight = IMG_SIZE_Y / hpSclY_;

	float left   = static_cast<float>(uiPos_.x) - realHalfWidth;
	float top    = static_cast<float>(uiPos_.y) - realHalfHeight;
	float bottom = static_cast<float>(uiPos_.y) + realHalfHeight;

	int srcWidth = static_cast<int>(IMG_SIZE_X * hpRate);
	float currentBarRight = left + (realHalfWidth * AsoUtility::DOUBLE * hpRate);

	// HPバー
	DrawRectExtendGraphF(
		left, top,
		currentBarRight, bottom, 
		0, 0, srcWidth, 
		static_cast<int>(IMG_SIZE_Y),
		hpImg_, true);
}

void UIHp::Damage(float delta)
{
	// HPを減少させる
	hpCurrent_ -= delta;
	if (hpCurrent_ <= 0.0f)
	{
		hpCurrent_ = 0.0f;
		active_ = false;
	}
}

void UIHp::Heal(float hp)
{
	// HPを回復させる
	hpCurrent_ += hp;
	if (hpCurrent_ >= MAX_HP) hpCurrent_ = MAX_HP;
}

void UIHp::InitLoad(void)
{
	// リソースのロード
	frameImg_ = resMng_.Load(ResourceManager::SRC::UI_BAR_FRAME).handleId_;
	hpImg_ = resMng_.Load(ResourceManager::SRC::UI_HP_BAR).handleId_;
}
