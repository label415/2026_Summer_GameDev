#include "../../Common/Transform.h"
#include "ColliderModel.h"
#include "ColliderSphere.h"

ColliderSphere::ColliderSphere(
	TAG tag, const std::vector<TAG>& targetTags,
	const Transform* follow, const VECTOR& localPos, 
	float radius, int partTag)
	: ColliderBase(SHAPE::SPHERE, tag, targetTags, follow, partTag)
	, localPos_(localPos)
	, radius_(radius)
{
}
