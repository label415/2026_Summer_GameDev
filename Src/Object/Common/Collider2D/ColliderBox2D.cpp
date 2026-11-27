#include "../../../Utility/AsoUtility.h"
#include "ColliderBox2D.h"

ColliderBox2D::ColliderBox2D(TAG tag, const Vector2F& pos, const Vector2F& size)
	: ColliderBase2D(SHAPE::BOX, tag)
	, boxPos_(pos)
	, boxSize_(size)
{
}

void ColliderBox2D::DrawDebug(int color)
{
	// デバッグ用のボックス描画
	DrawBoxAA(
		boxPos_.x,
		boxPos_.y,
		Right(),
		Bottom(),
		color,
		false
	);
}
