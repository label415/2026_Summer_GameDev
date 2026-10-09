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
	WeponBase(followTransform, followFrameId),
	moveDir_(moverDir),
	isAttack_(false),
	isEnd_(false),
	topPos_(COL_CAPSULE_TOP_LOCAL_POS),
	downPos_(COL_CAPSULE_DOWN_LOCAL_POS),
	moveSpeed_(SPEED),
	colliderCapsule_(nullptr)
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

	colliderCapsule_ = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::ENEMY_WEPON,
		targetTags,
		&transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS
	);

	colliderCapsule_->SetIsCollier(false);

	ownColliders_[static_cast<int>(ColliderBase::SHAPE::CAPSULE)].push_back(colliderCapsule_);
	CollisionManager::GetInstance().AddCollider(colliderCapsule_);
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
}

void WeponBracelet::SetCollider(void)
{
	if (colliderCapsule_) {
		colliderCapsule_->SetIsCollier(true);
	}
}

void WeponBracelet::ClearCollider(void)
{
	if (colliderCapsule_) {
		colliderCapsule_->SetIsCollier(false);
		// 初期座標と初期半径に復帰
		topPos_ = COL_CAPSULE_TOP_LOCAL_POS;
		downPos_ = COL_CAPSULE_DOWN_LOCAL_POS;
		colliderCapsule_->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
		colliderCapsule_->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
		colliderCapsule_->SetRadius(COL_CAPSULE_RADIUS);
	}
}

void WeponBracelet::SetIsAttack(bool isAttack)
{
	if (!isAttack_ && isAttack) {
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

		SetCollider(); // 判定開始
	}
	isAttack_ = isAttack;
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
		downPos_ = VAdd(downPos_, VScale(moveDir_, moveSpeed_));
		if (VSize(VSub(topPos_, downPos_)) <= 50.0f) {
			// 攻撃が終了して縮み切ったら判定をリセット
			ClearCollider();
			isEnd_ = false;
		}
	}

	if (colliderCapsule_)
	{
		colliderCapsule_->SetLocalPosTop(topPos_);
		colliderCapsule_->SetLocalPosDown(downPos_);
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