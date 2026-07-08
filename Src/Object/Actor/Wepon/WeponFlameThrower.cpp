#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/Collider/ColliderSphere.h"
#include "../../Common/EffectController.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Application.h"
#include "../../../Libs/ImGui/imgui.h"
#include "WeponFlameThrower.h"

WeponFlameThrower::WeponFlameThrower(const Transform& followTransform, const VECTOR moverDir, int followFrameId)
	:
	WeponBase(followTransform, followFrameId),
	moveDir_(moverDir)
{
}

WeponFlameThrower::~WeponFlameThrower(void)
{
}

void WeponFlameThrower::Update(void)
{
	if (!isAlive_)return;
	Move();
	effect_->SetEffectPos(static_cast<int>(EFFECT_TYPE::FIREBALL), transform_.pos);
	effect_->SetEffectScl(static_cast<int>(EFFECT_TYPE::FIREBALL), VScale(AsoUtility::VECTOR_ONE, radius_));
	effect_->Update(static_cast<int>(EFFECT_TYPE::FIREBALL));
}

void WeponFlameThrower::InitLoad(void)
{
}

void WeponFlameThrower::InitTransform(void)
{
}

void WeponFlameThrower::InitCollider(void)
{
	ColliderSphere* colSphere = new ColliderSphere(
		ColliderBase::TAG::ENEMY_WEPON,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		MIN_RADIUS);

	std::vector<ColliderBase*> colSpheres;
	colSpheres.push_back(colSphere);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::SPHERE), colSpheres);
}

void WeponFlameThrower::InitAnimation(void)
{
}

void WeponFlameThrower::InitPost(void)
{
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	effect_ = new EffectController();
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::FIREBALL),
		(Application::PATH_EFFECT + L"FireBall.efkefc"));
	effect_->Play(
		static_cast<int>(EFFECT_TYPE::FIREBALL),
		transform_.pos,
		VGet(0.0f, 0.0f, 0.0f),
		VGet(MIN_RADIUS, MIN_RADIUS, MIN_RADIUS));

	radius_ = MIN_RADIUS;
}

void WeponFlameThrower::Move(void)
{

	if (transform_.pos.y <= 0.0f)
	{
		transform_.pos.y = 0.0f;
		if (MAX_RADIUS >= radius_) {
			radius_+= 50.0f;
			const auto& cols = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::SPHERE));
			for (const auto& col : cols) {
				if (col->GetTag() != ColliderBase::TAG::ENEMY_WEPON) continue;
				ColliderSphere* colliderSphere = dynamic_cast<ColliderSphere*>(col);
				if (colliderSphere){
					colliderSphere->SetRadius(radius_);
				}
			}
		}
		else {
			ClearCollider();
		}
	}
	else{
		transform_.pos = VAdd(transform_.pos, VScale(moveDir_, SPEED));
	}
}

void WeponFlameThrower::Draw(void)
{
	ActorBase::Draw();
}

void WeponFlameThrower::Release(void)
{
	ActorBase::Release();
	delete effect_;
}

void WeponFlameThrower::ClearCollider(void)
{
	ownColliders_.erase(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	isAlive_ = false;
}