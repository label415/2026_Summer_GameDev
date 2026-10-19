#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/CollisionManager.h"
#include "../../../Application.h"
#include "../../../Libs/ImGui/imgui.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../../Common/EffectController.h"
#include "WeponBracelet.h"

WeponBracelet::WeponBracelet(const Transform& followTransform, const VECTOR moverDir, int followFrameId)
	:
	WeponBase(WeponBase::WEAPON_TYPE::BRACKELET, followTransform, followFrameId),
	moveDir_(moverDir),
	isAttack_(false),
	isEnd_(false),
	topPos_(COL_CAPSULE_TOP_LOCAL_POS),
	downPos_(COL_CAPSULE_DOWN_LOCAL_POS),
	moveSpeed_(SPEED)
{
}

WeponBracelet::~WeponBracelet(void) {}

void WeponBracelet::Update(void)
{
	Move();
	if (effect_) {
		effect_->Update(static_cast<int>(EFFECT_TYPE::BRACELET));
	}
}

void WeponBracelet::InitLoad(void) {}
void WeponBracelet::InitTransform(void) {}

void WeponBracelet::InitCollider(void)
{
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::PLAYER
	};

	auto colliderCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::ENEMY_WEPON,
		targetTags,
		&transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS
	);

	colliderCapsule->SetIsCollier(false);

	ownColliders_[static_cast<int>(ColliderBase::SHAPE::CAPSULE)].push_back(colliderCapsule);
	CollisionManager::GetInstance().AddCollider(colliderCapsule);
}

void WeponBracelet::InitAnimation(void) {}

void WeponBracelet::InitPost(void)
{
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);
	topPos_ = COL_CAPSULE_TOP_LOCAL_POS;
	downPos_ = COL_CAPSULE_DOWN_LOCAL_POS;
	moveSpeed_ = SPEED;

	effect_ = std::make_unique<EffectController>();
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::BRACELET),
		(Application::PATH_EFFECT + L"Breath.efkefc"));

	effect_->Stop(static_cast<int>(EFFECT_TYPE::BRACELET));
	ClearCollider();
}

void WeponBracelet::SetCollider(void)
{
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		vec->SetIsCollier(true);
	}
}

void WeponBracelet::ClearCollider(void)
{
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		auto* capsuleCollider = static_cast<ColliderCapsule*>(vec.get());
		capsuleCollider->SetIsCollier(false);
		capsuleCollider->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
		capsuleCollider->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
		capsuleCollider->SetRadius(COL_CAPSULE_RADIUS);
	}

	// 初期座標とすべてのフラグを確実にリセット
	topPos_ = COL_CAPSULE_TOP_LOCAL_POS;
	downPos_ = COL_CAPSULE_DOWN_LOCAL_POS;
	isAttack_ = false;
	isEnd_ = false;
}

void WeponBracelet::SetIsAttack(bool isAttack)
{
	if (!isAttack_ && isAttack) {
		ClearCollider();

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

		SetCollider();
	}
	isAttack_ = isAttack;
}

void WeponBracelet::Move(void)
{
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	if (!isAttack_ && !isEnd_) return;

	if (isAttack_ && !isEnd_)
	{
		if (VSize(VSub(topPos_, downPos_)) < LENGTH) {
			topPos_ = VAdd(topPos_, VScale(moveDir_, moveSpeed_));
		}
	}
	else if (isEnd_)
	{
		downPos_ = VAdd(downPos_, VScale(moveDir_, moveSpeed_));

		VECTOR remainVec = VSub(topPos_, downPos_);
		if (VSize(remainVec) <= moveSpeed_ || VDot(remainVec, moveDir_) <= 0.0f) {
			ClearCollider();
			return;
		}
	}

	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		auto* capsuleCollider = static_cast<ColliderCapsule*>(vec.get());
		capsuleCollider->SetLocalPosTop(topPos_);
		capsuleCollider->SetLocalPosDown(downPos_);
	}
}

void WeponBracelet::Draw(void)
{
	ActorBase::Draw();
}

void WeponBracelet::Release(void)
{
	ActorBase::Release();
}