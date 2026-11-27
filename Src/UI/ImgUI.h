#pragma once
#include "UIBase.h"

class ImgUI : public UIBase
{
public:

	// 不透明（ブレンドなし）の最大透過度
	static constexpr int MAX_ALPHA = 255;

	// コンストラクタ
	ImgUI(int img, Vector2F pos, Vector2F size, float angle = 0.0f, int alpha = MAX_ALPHA);

	// デストラクタ
	~ImgUI(void) override = default;

	// 更新
	void Update(void) override {}

	// 描画
	void Draw(void) override;

	// 回転角設定
	void SetAngle(float angle) { angle_ = angle; }

	// 回転角取得
	float GetAngle(void) const { return angle_; }

	// 透過度設定（0～255）
	void SetAlpha(int alpha);

	// 透過度取得
	int GetAlpha(void) const { return alpha_; }

protected:

	// 描画回転角
	float angle_;

	// 描画透過度（0～255）
	int alpha_;

};
