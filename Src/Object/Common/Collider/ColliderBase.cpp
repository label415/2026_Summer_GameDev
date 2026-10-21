#include "../../Common/Transform.h"
#include "ColliderBase.h"

ColliderBase::ColliderBase(
	SHAPE shape, TAG tag,
	const std::vector<TAG>& targetTags,
	const Transform* follow,
	int patrTag)
	:
	shape_(shape), tag_(tag),
	targetTags_(targetTags), patrTag_(patrTag),
	follow_(follow), isCollier_(true), isAction_(false)
{
}
ColliderBase::~ColliderBase(void)
{
}
void ColliderBase::Draw(void)
{
	int color = COLOR_INVALID;
	if (isCollier_)
	{
		color = COLOR_VALID;
	}
	DrawDebug(color);
}
void ColliderBase::SetFollow(const Transform* follow)
{
	follow_ = follow;
}

void ColliderBase::SetIsCollier(bool isCollier)
{
	isCollier_ = isCollier;
}

void ColliderBase::ClearHitCollider(void)
{
	hitCollider_.clear();
}

VECTOR ColliderBase::GetRotPos(const VECTOR& localPos) const
{
	// 追従相手の回転に合わせて指定ローカル座標を回転し、
		// 基準座標に加えることでワールド座標へ変換
	VECTOR localRotPos = follow_->quaRot.PosAxis(localPos);
	return VAdd(follow_->pos, localRotPos);
}