#include "../../Common/Transform.h"
#include "ColliderBase.h"
ColliderBase::ColliderBase(SHAPE shape, TAG tag, std::weak_ptr<const Transform> follow, int patrTag)
	:
	shape_(shape),
	tag_(tag),
	follow_(follow),
	patrTag_(patrTag),
	isValid_(true)
{
}
ColliderBase::~ColliderBase(void)
{
}
void ColliderBase::Draw(void)
{
	int color = COLOR_INVALID;
	if (isValid_)
	{
		color = COLOR_VALID;
	}
	DrawDebug(color);
}
void ColliderBase::SetFollow(std::weak_ptr<const Transform> follow)
{
	follow_ = follow;
}

void ColliderBase::SetValid(bool isValid)
{
	isValid_ = isValid;
}

VECTOR ColliderBase::GetRotPos(const VECTOR& localPos) const
{
	// 追従相手の回転に合わせて指定ローカル座標を回転し、
		// 基準座標に加えることでワールド座標へ変換
	VECTOR localRotPos = follow_.lock()->quaRot.PosAxis(localPos);
	return VAdd(follow_.lock()->pos, localRotPos);
}