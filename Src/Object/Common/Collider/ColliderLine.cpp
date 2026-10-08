#include "../../../Utility/AsoUtility.h"
#include "../../Common/Transform.h"
#include "ColliderModel.h"
#include "ColliderLine.h"

ColliderLine::ColliderLine(
	TAG tag, const std::vector<TAG>& targetTags, std::weak_ptr<const Transform> follow,
	const VECTOR& localPosStart, const VECTOR& localPosEnd, int patrTag)
	:
	ColliderBase(SHAPE::LINE, tag, targetTags, follow, patrTag),
	localPosStart_(localPosStart),
	localPosEnd_(localPosEnd)
{
}

ColliderLine::~ColliderLine(void) {}

void ColliderLine::SetLocalPosStart(const VECTOR& pos)
{
	localPosStart_ = pos;
}

void ColliderLine::SetLocalPosEnd(const VECTOR& pos)
{
	localPosEnd_ = pos;
}

const VECTOR& ColliderLine::GetLocalPosStart(void) const
{
	return localPosStart_;
}

const VECTOR& ColliderLine::GetLocalPosEnd(void) const
{
	return localPosEnd_;
}

VECTOR ColliderLine::GetPosStart(void) const
{
	return GetRotPos(localPosStart_);
}

VECTOR ColliderLine::GetPosEnd(void) const
{
	return GetRotPos(localPosEnd_);
}

void ColliderLine::DrawDebug(int color)
{
	VECTOR s = GetPosStart();
	VECTOR e = GetPosEnd();
	// 線分を描画
	DrawLine3D(s, e, color);
	// 始点・終点を球体で補助表示
	DrawSphere3D(s, RADIUS, DIV_NUM, color, color, true);
	DrawSphere3D(e, RADIUS, DIV_NUM, color, color, true);
}
