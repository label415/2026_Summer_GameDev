#pragma once
#include <DxLib.h>
#include "../../../Template/Vector2Template.h"
#include "ColliderBase2D.h"

class Transform;

class ColliderBox2D : public ColliderBase2D
{
public:

	// コンストラクタ
	ColliderBox2D(
		TAG tag, const Vector2F& pos,
		const Vector2F& size);

	// デストラクタ
	~ColliderBox2D(void) override = default;

	// 境界座標の取得
	float Left(void)   const { return boxPos_.x; }
	float Right(void)  const { return boxPos_.x + boxSize_.x; }
	float Top(void)    const { return boxPos_.y; }
	float Bottom(void) const { return boxPos_.y + boxSize_.y; }

	// 点が含まれているか判定
	bool Contains(const Vector2F& pos) const
	{
		return (pos.x >= boxPos_.x && pos.x <= Right() && pos.y >= boxPos_.y && pos.y <= Bottom());
	}

protected:

	// デバッグ用描画
	void DrawDebug(int color) override;

private:

	//ボックスの上下の座標
	Vector2F boxPos_;

	// ボックスの幅と高さ
	Vector2F boxSize_;

};

