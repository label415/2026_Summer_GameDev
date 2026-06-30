#include <DxLib.h>
#include "ShadowMap.h"

ShadowMap::ShadowMap(int x, int y)
{
	shadowId_ = MakeShadowMap(x, y);
}

ShadowMap::~ShadowMap(void)
{
}

void ShadowMap::AddShadowMapLight(VECTOR lightPos)
{
	SetShadowMapLightDirection(shadowId_, lightPos);
}

void ShadowMap::AddShadowMapDrawArea(VECTOR minPos, VECTOR maxPos)
{
	SetShadowMapDrawArea(shadowId_, minPos, maxPos);
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

void ShadowMap::Release(void)
{
	DeleteShadowMap(shadowId_);
}
