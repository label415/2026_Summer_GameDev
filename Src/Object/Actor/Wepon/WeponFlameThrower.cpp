#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SoundManager.h"
#include "../../Common/Collider/ColliderSphere.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../../Common/EffectController.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Application.h"
#include "../../../Libs/ImGui/imgui.h"
#include "WeponFlameThrower.h"

WeponFlameThrower::WeponFlameThrower(const Transform& followTransform, const ColliderModel* colMod, const VECTOR moverDir, int followFrameId)
	:
	WeponBase(followTransform, followFrameId),
	ColMod_(colMod),
	moveDir_(moverDir)
{
}

WeponFlameThrower::~WeponFlameThrower(void)
{
}

void WeponFlameThrower::Update(void)
{
	prePos_ = transform_.pos;
	Move();
	effect_->SetEffectPos(static_cast<int>(effectType_), transform_.pos);
	effect_->SetEffectScl(static_cast<int>(effectType_), VScale(AsoUtility::VECTOR_ONE, radius_));
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

	radius_ = MIN_RADIUS;

	effect_ = new EffectController();
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::BULLET),
		(Application::PATH_EFFECT + L"FireBall_Bullet.efkefc"));
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::EXPLOSION),
		(Application::PATH_EFFECT + L"FireBall_Explosion.efkefc"));

	effectType_ = EFFECT_TYPE::NONE;
	ChangerEffect(EFFECT_TYPE::BULLET);

	exState_ = EXPLOSION_STATO_TIME;
}

void WeponFlameThrower::Move(void)
{

	//自身の持つコライダー情報
	const auto& vecs = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::SPHERE));
	const ColliderSphere* colliderCapsule1 =
		dynamic_cast<const ColliderSphere*>(vecs[0]);

	if (colliderCapsule1->GetHitSpher_Model(ColMod_, false, false)) {

		int bgm_ = resMng_.Load(ResourceManager::SRC::ENEMY_ARE_ENEMY_BREASE2).handleId_;
		int volume_ = 100;
		SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_ARE_ENEMY_BREASE2, bgm_, volume_);

		transform_.pos = prePos_;

		if (MAX_RADIUS >= radius_) {
			ChangerEffect(EFFECT_TYPE::EXPLOSION);

			radius_ += 50.0f;

			const auto& cols = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::SPHERE));
			for (const auto& col : cols) {

				if (col->GetTag() != ColliderBase::TAG::ENEMY_WEPON) continue;
				ColliderSphere* colliderSphere = dynamic_cast<ColliderSphere*>(col);

				if (colliderSphere) {
					colliderSphere->SetRadius(radius_);
				}

			}
		}
		else {
			isAlive_ = false;
		}

	}
	else {
		transform_.pos = VAdd(transform_.pos, VScale(moveDir_, SPEED));
	}

}

void WeponFlameThrower::ChangerEffect(EFFECT_TYPE effectType)
{
	if (effectType_ == effectType)return;

	if(effectType_ != EFFECT_TYPE::NONE){
		effect_->Stop(static_cast<int>(effectType_));
	}

	effect_->Play(
		static_cast<int>(effectType),
		transform_.pos,
		VGet(0.0f, 0.0f, 0.0f),
		VGet(radius_, radius_, radius_));

	effectType_ = effectType;

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