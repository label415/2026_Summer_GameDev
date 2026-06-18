#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "WeponBracelet.h"
WeponBracelet::WeponBracelet(const Transform& followTransform, const VECTOR moverDir, int followFrameId)
	:
	WeponBase(followTransform, followFrameId),
	moveDir_(moverDir)
{
	isAttack_ = false;
}

WeponBracelet::~WeponBracelet(void)
{
}

void WeponBracelet::Update(void)
{
	Move();
}

void WeponBracelet::InitLoad(void)
{
}

void WeponBracelet::InitTransform(void)
{
}

void WeponBracelet::InitCollider(void)
{
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMY_WEPON, &transform_,
		AsoUtility::VECTOR_ZERO, AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_RADIUS);

	std::vector<ColliderBase*> colCapsules;
	colCapsules.push_back(colCapsule);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsules);
}

void WeponBracelet::InitAnimation(void)
{
}

void WeponBracelet::InitPost(void)
{ 
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);
	moveSpeed_ = SPEED;
}

void WeponBracelet::Move(void)
{
	if (isAttack_) {
		transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

		if (VSize(VSub(downPos_, topPos_)) <= LENGTH) {
			topPos_ = VAdd(topPos_, VScale(moveDir_, moveSpeed_));
		}
		else {
			topPos_ = topPos_;
		}
	}
	else if (!isEnd_ && !isAttack_) {
		transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);
	}

	const auto& cols = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	int cnt = 0; // 対象となるカプセルの個数を数えるカウンタ
	for (const auto& col : cols) {
		if (col->GetTag() != ColliderBase::TAG::ENEMY_WEPON) continue;

		ColliderCapsule* colliderCapsule = dynamic_cast<ColliderCapsule*>(col);
		if (colliderCapsule)
		{
			colliderCapsule->SetLocalPosTop(topPos_);
		}
	}

	if (!isEnd_)return;
	isAttack_ = false;
	downPos_ = VAdd(downPos_, VScale(moveDir_, moveSpeed_));
	if (VSize(VSub(downPos_, topPos_)) <= 50.0f) {
		isAlive_ = false;
	}

	cnt = 0;
	for (const auto& col : cols) {
		if (col->GetTag() != ColliderBase::TAG::ENEMY_WEPON) continue;

		ColliderCapsule* colliderCapsule = dynamic_cast<ColliderCapsule*>(col);
		if (colliderCapsule)
		{
			colliderCapsule->SetLocalPosDown(downPos_);
		}
	}
}

void WeponBracelet::Draw(void)
{
	ActorBase::Draw();
	DrawCapsule3D(topPos_, downPos_, COL_CAPSULE_RADIUS, 10, 0xff0000, 0xff0000, true);
}

void WeponBracelet::SetCollider(void)
{
}

void WeponBracelet::ClearCollider(void)
{
	
}
