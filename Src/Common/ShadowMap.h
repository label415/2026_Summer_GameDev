#pragma once

class ShadowMap
{
public:
	// コンストラクタ
	ShadowMap(int x, int y);

	// デストラクタ
	~ShadowMap(void);

	// シャドウマップの光源を設定
	void AddShadowMapLight(VECTOR lightPos);

	// シャドウマップの描画範囲を設定
	void AddShadowMapDrawArea(VECTOR minPos, VECTOR maxPos);

	// シャドウマップの深度を調整	
	void AddShadowMapAdjustDepth(float depth);

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
	// シャドウマップのID
	int shadowId_;
};

