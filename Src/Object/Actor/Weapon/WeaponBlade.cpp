#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "WeaponBlade.h"

WeaponBlade::WeaponBlade(const Transform& followTransform, int followFrameId)
	:
	WeaponBase(WeaponBase::WEAPON_TYPE::BLADE, followTransform, followFrameId)
{
	// 生存フラグを無効にする
	isAlive_ = false;
}

void WeaponBlade::Update(void)
{
	WeaponBase::Update();
}

void WeaponBlade::InitLoad(void)
{
	// モデルのロード
	transform_.SetModel(
		resMng_.Load(ResourceManager::SRC::MODEL_WEAPON_BLADE).handleId_);
}

void WeaponBlade::InitTransform(void)
{
	// モデルの大きさ、回転、座標の初期化
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, SCALE);
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal = Quaternion();
	transform_.pos = AsoUtility::VECTOR_ZERO;
	localPos_ = DEFAULT_LOCAL_POS;
	localRot_ = {
	AsoUtility::Deg2RadF(0.0f),
	AsoUtility::Deg2RadF(0.0f),
	AsoUtility::Deg2RadF(DEFAULT_LOCAL_ROT_Z_DEG)
	};
}

void WeaponBlade::InitCollider(void)
{
	// 攻撃対象タグ（敵本体など）
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::ENEMY
	};

	// 武器コライダーを初期化時に生成
	auto colliderCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::PLAYER_WEAPON,
		targetTags,
		&transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS
	);

	// 初期状態は当たり判定を無効化
	colliderCapsule->SetIsCollider(false);

	// コライダーを所有コライダーに登録
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::CAPSULE)].push_back(colliderCapsule);
}

void WeaponBlade::SetCollider(void)
{
	isAlive_ = true;
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		vec->SetIsCollider(true);
	}
}

void WeaponBlade::ClearCollider(void)
{
	isAlive_ = false;
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		auto capsuleCollider = std::dynamic_pointer_cast<ColliderCapsule>(vec);
		// 当たり判定をオフにする
		capsuleCollider->SetIsCollider(false);

		// 生成時の初期座標・初期サイズに戻す
		capsuleCollider->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
		capsuleCollider->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
		capsuleCollider->SetRadius(COL_CAPSULE_RADIUS);
	}
}
