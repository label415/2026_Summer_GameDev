#pragma once
#include <DxLib.h>
#include "ColliderBase2D.h"
#include "../../../Common/Vector2F.h"

class Transform;

class ColliderBox2D : public ColliderBase2D
{
public:

	// コンストラクタ
	ColliderBox2D(
		TAG tag, const Vector2F& pos,
		float width, float height);

	// デストラクタ
	~ColliderBox2D(void) override;

	// 境界座標の取得
	float Left(void)   const { return pos_.x; }
	float Right(void)  const { return pos_.x + width_; }
	float Top(void)    const { return pos_.y; }
	float Bottom(void) const { return pos_.y + height_; }

	/// <summary>
	/// 点が含まれているか判定
	/// </summary>
	bool Contains(float px, float py) const
	{
		return (px >= pos_.x && px <= Right() && py >= pos_.y && py <= Bottom());
	}

protected:

	// デバッグ用描画
	void DrawDebug(int color) override;

private:

	//ボックスの上下の座標
	Vector2F pos_;
	float width_;
	float height_;
};

