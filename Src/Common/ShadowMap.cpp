#include <DxLib.h>
#include "../Application.h"
#include "ShadowMap.h"

ShadowMap::ShadowMap(int x, int y)
{
	shadowId_ = MakeShadowMap(x, y);
}

ShadowMap::~ShadowMap(void){}

void ShadowMap::AddShadowMapLight(VECTOR lightPos)
{
	// シャドウマップの光源を設定
	SetShadowMapLightDirection(shadowId_, lightPos);
}

void ShadowMap::AddShadowMapDrawArea(VECTOR minPos, VECTOR maxPos)
{
	// シャドウマップの描画範囲を設定
	SetShadowMapDrawArea(shadowId_, minPos, maxPos);
}

void ShadowMap::AddShadowMapAdjustDepth(float depth)
{
	// シャドウマップの深度を調整
	SetShadowMapAdjustDepth(shadowId_, depth);
}

void ShadowMap::DrawSetup(void)
{
	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup(shadowId_);
}

void ShadowMap::DrawEnd(void)
{
	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();
}

void ShadowMap::SetShadow(void)
{
	// 描画に使用するシャドウマップを設定
	SetUseShadowMap(0, shadowId_);

}

void ShadowMap::EndShadow(void)
{
	// 描画に使用するシャドウマップの設定を解除
	SetUseShadowMap(0, -1);
}

void ShadowMap::TestDraw(void)
{
	// シャドウマップのテスト描画
	TestDrawShadowMap(shadowId_,
		0,
		Application::SCREEN_SIZE_Y / 2,
		(Application::SCREEN_SIZE_X / 2) / 2,
		Application::SCREEN_SIZE_Y);
}

void ShadowMap::Release(void)
{
	// シャドウマップの解放
	DeleteShadowMap(shadowId_);
}
