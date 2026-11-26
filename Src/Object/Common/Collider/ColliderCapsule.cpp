#include <DxLib.h>
#include "../../../Utility/AsoUtility.h"
#include "../../Common/Transform.h"
#include "ColliderCapsule.h"
#include "ColliderModel.h"

ColliderCapsule::ColliderCapsule(
	TAG tag, const std::vector<TAG>& targetTags, const Transform* follow,
	const VECTOR& localPosTop, const VECTOR& localPosDown,
	float radius, int partTag)
	: ColliderBase(SHAPE::CAPSULE, tag, targetTags, follow, partTag)
	, localPosTop_(localPosTop)
	, localPosDown_(localPosDown)
	, radius_(radius)
	, center_(AsoUtility::VECTOR_ZERO)
{
}

VECTOR& ColliderCapsule::GetCenter(void)
{
	// è„â∫ÇÃç¿ïWÇ©ÇÁíÜêSç¿ïWÇåvéZ
	VECTOR top = GetPosTop();
	VECTOR down = GetPosDown();
	VECTOR diff = VSub(top, down);
	center_ = VAdd(down, VScale(diff, AsoUtility::HALF));
	return center_;
}

void ColliderCapsule::DrawDebug(int color)
{
	// è„ÇÃãÖëÃ
	VECTOR pos1 = GetPosTop();
	DrawSphere3D(pos1, radius_, DEBUG_SPHERE_DIV_NUM, color, color, false);

	// â∫ÇÃãÖëÃ
	VECTOR pos2 = GetPosDown();
	DrawSphere3D(pos2, radius_, DEBUG_SPHERE_DIV_NUM, color, color, false);

	// ãÖëÃÇåqÇÆê¸
	VECTOR dir;
	VECTOR s;
	VECTOR e;
	// ãÖëÃÇåqÇÆê¸(X+)
	dir = follow_->GetRight();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);

	// ãÖëÃÇåqÇÆê¸(X-)
	dir = follow_->GetLeft();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);

	// ãÖëÃÇåqÇÆê¸(Z+)
	dir = follow_->GetForward();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);

	// ãÖëÃÇåqÇÆê¸(Z-)
	dir = follow_->GetBack();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);

	// ÉJÉvÉZÉãÇÃíÜêS
	DrawSphere3D(GetCenter(), DEBUG_CENTER_SPHERE_RADIUS, DEBUG_CENTER_SPHERE_DIV_NUM, color, color, true);
}