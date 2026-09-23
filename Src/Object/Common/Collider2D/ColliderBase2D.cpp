#include "ColliderBase2D.h"

ColliderBase2D::ColliderBase2D(SHAPE shape, TAG tag)
	:
	shape_(shape),
	tag_(tag),
	isValid_(true)
{
}
ColliderBase2D::~ColliderBase2D(void)
{
}

void ColliderBase2D::Draw(void)
{
	int color = COLOR_INVALID;
	if (isValid_)
	{
		color = COLOR_VALID;
	}
	DrawDebug(color);
}

void ColliderBase2D::SetValid(bool isValid)
{
	isValid_ = isValid;
}