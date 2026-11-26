#include "../../Common/Transform.h"
#include "ColliderBase.h"

ColliderBase::ColliderBase(
	SHAPE shape, TAG tag,
	const std::vector<TAG>& targetTags,
	const Transform* follow, int partTag)
	: shape_(shape)
	, tag_(tag)
	, targetTags_(targetTags)
	, partTag_(partTag)
	, follow_(follow)
	, isCollider_(true)
	, isAction_(false)
{
}

void ColliderBase::Draw(void)
{
	int color = COLOR_INVALID;
	if (isCollider_)
	{
		color = COLOR_VALID;
	}

	DrawDebug(color);
}

VECTOR ColliderBase::GetRotPos(const VECTOR& localPos) const
{
	// 追従相手の回転に合わせて指定ローカル座標を回転し、
	// 基準座標に加えることでワールド座標へ変換
	VECTOR localRotPos = follow_->quaRot.PosAxis(localPos);
	return VAdd(follow_->pos, localRotPos);
}

void ColliderBase::ClearHitCollider(void)
{
	hitCollider_.clear();
}