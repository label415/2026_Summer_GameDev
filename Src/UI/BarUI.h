#pragma once
#include <functional>
#include "UIBase.h"

class BarUI : public UIBase
{
public:

	// コンストラクタ
	BarUI(
		int barImg,
		int frameImg,
		Vector2F pos,
		Vector2F barScl,
		float frameScl = 1.0f,
		float initRate = 1.0f);

	// デストラクタ
	~BarUI(void) override = default;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// モデル側の割合取得関数をバインドする
	void BindRate(std::function<float(void)> rateGetter);

	// 目標割合を設定（0.0f?1.0f）
	void SetRate(float rate);

	// 現在の目標割合を取得
	float GetRate(void) const { return targetRate_; }

private:

	// 表示割合の補間速度
	static constexpr float INTERP_SPEED = 12.0f;
	// 補間スナップしきい値
	static constexpr float SNAP_EPSILON = 0.001f;

	// 枠画像のハンドルID
	int frameImg_;

	// 枠の描画スケール
	float frameScl_;

	// モデルから同期される目標割合（0.0f?1.0f）
	float targetRate_;

	// 画面描画に使う現在表示割合（0.0f?1.0f）
	float displayRate_;

	// モデル参照用コールバック
	std::function<float(void)> rateGetter_;

};
