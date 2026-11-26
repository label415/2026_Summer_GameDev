#include "ColliderBase2D.h"

ColliderBase2D::ColliderBase2D(SHAPE shape, TAG tag)
	: shape_(shape)
	, tag_(tag)
	, isValid_(true)
{
}

void ColliderBase2D::Draw(void)
{
	// デバッグ用の色を決定
	int color = COLOR_INVALID;
	if (isValid_)
	{
		color = COLOR_VALID;
	}

	// デバッグ用描画
	DrawDebug(color);
}