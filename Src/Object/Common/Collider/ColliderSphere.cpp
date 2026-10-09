#include "../../Common/Transform.h"
#include "ColliderModel.h"
#include "ColliderSphere.h"

ColliderSphere::ColliderSphere(
	TAG tag,
	const std::vector<TAG>& targetTags,
	const Transform* follow,
	const VECTOR& localPos, 
	float radius, int patrTag)
	:
	ColliderBase(SHAPE::SPHERE, tag, targetTags, follow, patrTag),
	localPos_(localPos),
	radius_(radius)
{
}

ColliderSphere::~ColliderSphere(void) {}

void ColliderSphere::DrawDebug(int color)
{
	// デバッグ用に球体を描画
	DrawSphere3D(GetPos(), GetRadius(), DIV_NUM, color, color, false);
}
