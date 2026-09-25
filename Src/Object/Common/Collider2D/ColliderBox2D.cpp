#include "../../../Utility/AsoUtility.h"
#include "ColliderBox2D.h"

ColliderBox2D::ColliderBox2D(TAG tag, const Vector2F& pos, float width, float height)
	: 
    ColliderBase2D(SHAPE::BOX, tag)
	, boxPos_(pos)
	, boxWidth_(width)
	, boxHeight_(height)
{
}

ColliderBox2D::~ColliderBox2D(void)
{
}

void ColliderBox2D::DrawDebug(int color)
{
    DrawLineBox(
        static_cast<int>(boxPos_.x),
        static_cast<int>(boxPos_.y),
        static_cast<int>(Right()),
        static_cast<int>(Bottom()),
        color
    );
}
