#pragma once
#include <DxLib.h>

class ShadowMap
{
public:

	// コンストラクタ
	ShadowMap(int x, int y);

	// デストラクタ
	~ShadowMap(void) {}

	// シャドウマップの光源を設定
	void AddShadowMapLight(VECTOR lightPos) { SetShadowMapLightDirection(shadowId_, lightPos); }

	// シャドウマップの描画範囲を設定
	void AddShadowMapDrawArea(VECTOR minPos, VECTOR maxPos) { SetShadowMapDrawArea(shadowId_, minPos, maxPos); }

	// シャドウマップの深度を調整	
	void AddShadowMapAdjustDepth(float depth) { SetShadowMapAdjustDepth(shadowId_, depth); }

	// シャドウマップへの描画の準備
	void DrawSetup(void);

	// シャドウマップへの描画を終了
	void DrawEnd(void);

	// 描画に使用するシャドウマップを設定
	void SetShadow(void);

	// 描画に使用するシャドウマップの設定を解除
	void EndShadow(void);

	// シャドウマップのテスト描画
	void TestDraw(void);

	// シャドウマップの解放
	void Release(void);

private:

	// テスト描画幅の縮小除数
	static constexpr int TEST_DRAW_WIDTH_DIV = 2;

	// シャドウマップのID
	int shadowId_;

};

