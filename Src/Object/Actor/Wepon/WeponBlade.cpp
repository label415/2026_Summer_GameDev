#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/CollisionManager.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "WeponBlade.h"

WeponBlade::WeponBlade(const Transform& followTransform, int followFrameId)
	:
	WeponBase(WeponBase::WEAPON_TYPE::BLADE, followTransform, followFrameId)
{
	// 生存フラグを無効にする
	isAlive_ = false;
}

WeponBlade::~WeponBlade(void){}

void WeponBlade::Update(void)
{
	// 基底クラスを更新
	WeponBase::Update();
}

void WeponBlade::InitLoad(void)
{
	// モデルのロード
	transform_.SetModel(
		resMng_.Load(ResourceManager::SRC::MODEL_WEAPON_BLADE).handleId_);
}

void WeponBlade::InitTransform(void)
{
	// モデルの大きさ、回転、座標の初期化
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, SCALE);
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal = Quaternion();
	transform_.pos = AsoUtility::VECTOR_ZERO;
	localPos_ = { -2.0f, 0.0f, -3.0f };
	localRot_ = {
	AsoUtility::Deg2RadF(0.0f),
	AsoUtility::Deg2RadF(0.0f),
	AsoUtility::Deg2RadF(-90.0f)
	};
}

void WeponBlade::InitCollider(void)
{
	// 攻撃対象タグ（敵本体など）
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::ENEMY
	};

	// 武器コライダーを初期化時に生成
	auto colliderCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::PLAYER_WEPON,
		targetTags,
		&transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS
	);

	// 初期状態は当たり判定を無効化
	colliderCapsule->SetIsCollier(false);

	ownColliders_[static_cast<int>(ColliderBase::SHAPE::CAPSULE)].push_back(colliderCapsule);
	CollisionManager::GetInstance().AddCollider(colliderCapsule);
}

void WeponBlade::InitAnimation(void){}

void WeponBlade::InitPost(void){}

void WeponBlade::SetCollider(void)
{
	isAlive_ = true;
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		vec->SetIsCollier(true);
	}
}

void WeponBlade::ClearCollider(void)
{
	isAlive_ = false;
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		auto capsuleCollider = std::dynamic_pointer_cast<ColliderCapsule>(vec);
		// 当たり判定をオフにする
		capsuleCollider->SetIsCollier(false);

		// 生成時の初期座標・初期サイズに戻す
		capsuleCollider->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
		capsuleCollider->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
		capsuleCollider->SetRadius(COL_CAPSULE_RADIUS);
	}
}
