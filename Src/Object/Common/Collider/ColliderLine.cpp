#include "../../../Utility/AsoUtility.h"
#include "../../Common/Transform.h"
#include "ColliderModel.h"
#include "ColliderLine.h"

ColliderLine::ColliderLine(
	TAG tag, const std::vector<TAG>& targetTags, const Transform* follow,
	const VECTOR& localPosStart, const VECTOR& localPosEnd, int partTag)
	: ColliderBase(SHAPE::LINE, tag, targetTags, follow, partTag)
	, localPosStart_(localPosStart)
	, localPosEnd_(localPosEnd)
{
}

VECTOR ColliderLine::GetPosStart(void) const
{
	// 追従先が設定されていれば、ローカル座標をワールド座標に変換して返す
	if (follow_ != nullptr)
	{
		return GetRotPos(localPosStart_);
	}
	return localPosStart_;
}

VECTOR ColliderLine::GetPosEnd(void) const
{
	// 追従先が設定されていれば、ローカル座標をワールド座標に変換して返す
	if (follow_ != nullptr)
	{
		return GetRotPos(localPosEnd_);
	}
	return localPosEnd_;
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
