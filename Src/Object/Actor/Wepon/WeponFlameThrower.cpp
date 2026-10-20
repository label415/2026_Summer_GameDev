#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/CollisionManager.h"
#include "../../../Manager/SoundManager.h"
#include "../../Common/Collider/ColliderSphere.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../../Common/EffectController.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Application.h"
#include "../../../Libs/ImGui/imgui.h"
#include "WeponFlameThrower.h"

WeponFlameThrower::WeponFlameThrower(const Transform& followTransform, const VECTOR moverDir, int followFrameId)
	:
	WeponBase(WeponBase::WEAPON_TYPE::FLAME, followTransform, followFrameId),
	moveDir_(moverDir)
{
}

WeponFlameThrower::~WeponFlameThrower(void)
{
}

void WeponFlameThrower::Update(void)
{
	if (!isAlive_) return;

	prePos_ = transform_.pos;
	Move();
	if (!isAlive_ || effectType_ == EFFECT_TYPE::NONE) return;

	effect_->SetEffectPos(static_cast<int>(effectType_), transform_.pos);
	effect_->SetEffectScl(static_cast<int>(effectType_), VScale(AsoUtility::VECTOR_ONE, radius_));
	effect_->Update(static_cast<int>(effectType_));
}

void WeponFlameThrower::InitLoad(void)
{
}

void WeponFlameThrower::InitTransform(void)
{
}

void WeponFlameThrower::InitCollider(void)
{
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::PLAYER,
		ColliderBase::TAG::STAGE
	};

	auto colliderSphere = std::make_shared<ColliderSphere>(
		ColliderBase::TAG::ENEMY_WEPON,
		targetTags,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		MIN_RADIUS
	);

	colliderSphere->SetIsCollier(false);

	ownColliders_[static_cast<int>(ColliderBase::SHAPE::SPHERE)].push_back(colliderSphere);
	CollisionManager::GetInstance().AddCollider(colliderSphere);
}

void WeponFlameThrower::InitAnimation(void)
{
}

void WeponFlameThrower::InitPost(void)
{
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	radius_ = MIN_RADIUS;

	effect_ = std::make_unique<EffectController>();
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::BULLET),
		(Application::PATH_EFFECT + L"FireBall_Bullet.efkefc"));
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::EXPLOSION),
		(Application::PATH_EFFECT + L"FireBall_Explosion.efkefc"));

	effectType_ = EFFECT_TYPE::NONE;
	isAlive_ = false;
	exState_ = EXPLOSION_STATO_TIME;
}

void WeponFlameThrower::Move(void)
{
	bool isHitStage = false;

	int sphereType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	for (const auto& vec : ownColliders_.at(sphereType))
	{
		// 所有権の操作は不要なため生ポインタの static_cast を使用
		auto* colliderSphere = static_cast<ColliderSphere*>(vec.get());
		for (const auto& hit : colliderSphere->GetHitColliders())
		{
			auto hitCol = hit.lock();
			if (hitCol && hitCol->GetIsCollier()
				&& hitCol->GetTag() == ColliderBase::TAG::STAGE)
			{
				isHitStage = true;
				break;
			}
		}
	}

	// すでに爆発中、またはステージに着弾した瞬間
	if (effectType_ == EFFECT_TYPE::EXPLOSION || isHitStage)
	{
		if (effectType_ != EFFECT_TYPE::EXPLOSION)
		{
			// 着弾SE再生
			int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_ARE_BREASE_2).handleId_;
			SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_ARE_ENEMY_BREASE2, bgm_, 100);

			transform_.pos = prePos_;
			ChangerEffect(EFFECT_TYPE::EXPLOSION);
		}

		if (radius_ <= MAX_RADIUS)
		{
			radius_ += 50.0f;
			for (const auto& vec : ownColliders_.at(sphereType))
			{
				auto* colliderSphere = static_cast<ColliderSphere*>(vec.get());
				colliderSphere->SetRadius(radius_);
			}
		}
		else
		{
			ClearCollider();
		}
	}
	else
	{
		// 飛翔中
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
}

void WeponFlameThrower::SetCollider(void)
{
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		auto colliderSphere = std::dynamic_pointer_cast<ColliderSphere>(vec);
		colliderSphere->SetIsCollier(true);
	}
}

void WeponFlameThrower::ClearCollider(void)
{
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		auto colliderSphere = std::dynamic_pointer_cast<ColliderSphere>(vec);
		// 当たり判定をオフにする
		colliderSphere->SetIsCollier(false);
		colliderSphere->SetRadius(MIN_RADIUS);
	}

	// 大きさと位置を初期状態へリセット
	radius_ = MIN_RADIUS;

	// 追従元の口元座標へリセット
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);
	prePos_ = transform_.pos;

	if (effectType_ != EFFECT_TYPE::NONE)
	{
		effect_->Stop(static_cast<int>(effectType_));
		effectType_ = EFFECT_TYPE::NONE;
	}

	isAlive_ = false;
}

void WeponFlameThrower::Shot(const VECTOR& dir)
{
	ClearCollider();

	moveDir_ = dir;
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);
	prePos_ = transform_.pos;
	radius_ = MIN_RADIUS;

	isAlive_ = true;
	SetCollider();
	ChangerEffect(EFFECT_TYPE::BULLET);
}
