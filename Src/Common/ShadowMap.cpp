#include <DxLib.h>
#include "../Application.h"
#include "ShadowMap.h"

ShadowMap::ShadowMap(const Vector2& size)
	: shadowId_(-1)
{
	// 指定解像度でシャドウマップを生成
	shadowId_ = MakeShadowMap(size.x, size.y);
}

void ShadowMap::TestDraw(void)
{
	// シャドウマップのテスト描画
	TestDrawShadowMap(shadowId_, 0,
		Application::HALF_SCREEN_SIZE.y,
		(Application::HALF_SCREEN_SIZE.x) / TEST_DRAW_WIDTH_DIV,
		Application::SCREEN_SIZE.y);
}

void ShadowMap::DrawSetup(void)
{
	// シャドウマップへの深度書き込み描画を開始
	ShadowMap_DrawSetup(shadowId_);
}

void ShadowMap::DrawEnd(void)
{
	// シャドウマップへの深度書き込み描画を終了
	ShadowMap_DrawEnd();
}

void ShadowMap::SetShadow(void)
{
	// 通常描画に対してシャドウマップの適用を有効化
	SetUseShadowMap(0, shadowId_);
}

void ShadowMap::EndShadow(void)
{
	// 通常描画に対するシャドウマップの適用を解除
	SetUseShadowMap(0, -1);
}

void ShadowMap::Release(void)
{
	// 生成したシャドウマップを解放
	DeleteShadowMap(shadowId_);
}
