#include <DxLib.h>
#include "../Application.h"
#include "ShadowMap.h"

ShadowMap::ShadowMap(int x, int y)
	: shadowId_(-1)
{
	shadowId_ = MakeShadowMap(x, y);
}

void ShadowMap::TestDraw(void)
{
	// シャドウマップのテスト描画
	TestDrawShadowMap(shadowId_,
		0,
		Application::HALF_SCREEN_SIZE_Y,
		(Application::HALF_SCREEN_SIZE_X) / TEST_DRAW_WIDTH_DIV,
		Application::SCREEN_SIZE_Y);
}

void ShadowMap::DrawSetup(void)
{
	ShadowMap_DrawSetup(shadowId_);
}

void ShadowMap::DrawEnd(void)
{
	ShadowMap_DrawEnd();
}

void ShadowMap::SetShadow(void)
{
	SetUseShadowMap(0, shadowId_);
}

void ShadowMap::EndShadow(void)
{
	SetUseShadowMap(0, -1);
}

void ShadowMap::Release(void)
{
	DeleteShadowMap(shadowId_);
}
