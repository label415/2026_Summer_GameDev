#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../../Common/EffectController.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Application.h"
#include "../../../Libs/ImGui/imgui.h"
#include "WeponBracelet.h"

WeponBracelet::WeponBracelet(const Transform& followTransform, const ColliderModel* colMod, const VECTOR moverDir, int followFrameId)
	:
	WeponBase(followTransform, followFrameId),
	ColMod_(colMod),
	moveDir_(moverDir)
{
	isAttack_ = false;
	isEnd_ = false;
	topPos_ = AsoUtility::VECTOR_ZERO;
	downPos_ = AsoUtility::VECTOR_ZERO;
}

WeponBracelet::~WeponBracelet(void)
{
}

void WeponBracelet::Update(void)
{
	if (!isAlive_) {
		ClearCollider();
		return;
	}
	Move();
	effect_->Update(static_cast<int>(EFFECT_TYPE::BRACELET));
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
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_TOP_LOCAL_POS,
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

	topPos_ = AsoUtility::VECTOR_ZERO;
	downPos_ = AsoUtility::VECTOR_ZERO;

	moveSpeed_ = SPEED;

	effect_ = new EffectController();
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::BRACELET),
		(Application::PATH_EFFECT + L"Breath.efkefc"));
}

void WeponBracelet::Move(void)
{
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	if (isAttack_ && !isEnd_)
	{
		if (VSize(VSub(topPos_, downPos_)) < LENGTH) {
			topPos_ = VAdd(topPos_, VScale(moveDir_, moveSpeed_));
		}
	}
	else if (isEnd_)
	{
		isAttack_ = false;
		downPos_ = VAdd(downPos_, VScale(moveDir_, moveSpeed_));
		if (VSize(VSub(topPos_, downPos_)) <= 50.0f) {
			isAlive_ = false;
		}
	}

	const auto& cols = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	for (const auto& col : cols) {
		if (col->GetTag() != ColliderBase::TAG::ENEMY_WEPON) continue;

		ColliderCapsule* colliderCapsule = dynamic_cast<ColliderCapsule*>(col);
		if (colliderCapsule)
		{
			colliderCapsule->SetLocalPosTop(topPos_);
			colliderCapsule->SetLocalPosDown(downPos_);
		}
	}
}

void WeponBracelet::Draw(void)
{
	ActorBase::Draw();
}

void WeponBracelet::Release(void)
{
	ActorBase::Release();
	delete effect_;
}

void WeponBracelet::SetCollider(void)
{
}

void WeponBracelet::ClearCollider(void)
{
	ownColliders_.erase(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
}

void WeponBracelet::SetIsAttack(bool isAttack)
{
	if (!isAttack_) {
		float yaw = atan2f(moveDir_.x, moveDir_.z);
		float pitch = -asinf(moveDir_.y);
		VECTOR euler = { pitch, yaw, 0.0f };
		euler = VAdd(euler, WeponBracelet::DEFAULT_ROT);
		VECTOR effPos = transform_.pos;
		effPos.y -= 250.0f;

		effect_->Play(
			static_cast<int>(EFFECT_TYPE::BRACELET),
			effPos,
			euler, VGet(400.0f, 400.0f, LENGTH / 2.2f));
	}
	isAttack_ = isAttack;
}
