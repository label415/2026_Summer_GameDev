#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/Common/ResourceManager.h"
#include "../../../Manager/Common/SceneManager.h"
#include "../../../Manager/Common/EffectManager.h"
#include "../../../Application.h"
#include "../../../Libs/ImGui/imgui.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "WeaponBreath.h"

WeaponBreath::WeaponBreath(
	const Transform& followTransform,
	const VECTOR moveDir, int followFrameId)
	: WeaponBase(WeaponBase::WEAPON_TYPE::BREATH, followTransform, followFrameId)
	, moveDir_(moveDir)
	, isAttack_(false)
	, isEnd_(false)
	, topPos_(COL_CAPSULE_TOP_LOCAL_POS)
	, downPos_(COL_CAPSULE_DOWN_LOCAL_POS)
	, moveSpeed_(SPEED)
	, effHandle_(-1)
{
}

void WeaponBreath::Update(void)
{
	// 生存していない時は処理しない
	if (!isAlive_) return;

	// 移動処理
	Move();
}

void WeaponBreath::Draw(void)
{
	ActorBase::Draw();
}

void WeaponBreath::Release(void)
{
	EffectManager::GetInstance().Stop(effHandle_);
	effHandle_ = -1;
	ActorBase::Release();
}

void WeaponBreath::InitCollider(void)
{
	// 衝突判定のターゲットタグを設定
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::PLAYER
	};

	// カプセルコライダーを作成して登録
	auto colliderCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::ENEMY_WEAPON,
		targetTags,
		&transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS
	);

	// 初期状態では当たり判定を無効化
	colliderCapsule->SetIsCollider(false);

	// コライダーを所有コライダーに追加
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::CAPSULE)].push_back(colliderCapsule);
}

void WeaponBreath::InitPost(void)
{
	// 初期座標を設定
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	// コライダーを初期化
	ClearCollider();
}

void WeaponBreath::SetCollider(void)
{
	// カプセルコライダーを有効化
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& col : ownColliders_.at(capsuleType))
	{
		col->SetIsCollider(true);
	}
}

void WeaponBreath::ClearCollider(void)
{
	// カプセルコライダーを無効化して初期座標に戻す
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& col : ownColliders_.at(capsuleType))
	{
		auto* capsule = static_cast<ColliderCapsule*>(col.get());
		capsule->SetIsCollider(false);
		capsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
		capsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
		capsule->SetRadius(COL_CAPSULE_RADIUS);
	}

	// 初期座標とすべてのフラグをリセット
	topPos_ = COL_CAPSULE_TOP_LOCAL_POS;
	downPos_ = COL_CAPSULE_DOWN_LOCAL_POS;
	isAttack_ = false;
	isEnd_ = false;

	// 再生中のブレスエフェクトを停止
	EffectManager::GetInstance().Stop(effHandle_);
	effHandle_ = -1;
}

void WeaponBreath::SetIsAttack(bool isAttack)
{
	// 攻撃フラグが切り替わった場合にエフェクトを再生
	if (!isAttack_ && isAttack) {

		// コライダーをクリア
		ClearCollider();

		// エフェクト角度と座標を計算
		float yaw = atan2f(moveDir_.x, moveDir_.z);
		float pitch = -asinf(moveDir_.y);
		VECTOR euler = { pitch, yaw, 0.0f };
		euler = VAdd(euler, WeaponBreath::DEFAULT_ROT);
		VECTOR effPos = transform_.pos;
		effPos.y -= EFFECT_OFFSET_Y;

		// エフェクトを再生
		EffectManager::GetInstance().Stop(effHandle_);
		effHandle_ = EffectManager::GetInstance().PlayEffect(
			EffectManager::EFFECT_ID::BREATH,
			effPos,
			euler, VGet(EFFECT_SCALE_XY, EFFECT_SCALE_XY, LENGTH / EFFECT_LENGTH_DIV));

		// 攻撃開始時にコライダーを設定
		SetCollider();
	}

	// 攻撃フラグを更新
	isAttack_ = isAttack;
}

void WeaponBreath::Move(void)
{
	// 追従対象のフレーム座標を取得して武器の座標を更新
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	// フラグがどちらも立っていない場合は移動処理を行わない
	if (!isAttack_ && !isEnd_) return;

	// 攻撃中は先端を伸ばし、終了フェーズでは根元側を縮めて追いつかせる
	if (isAttack_ && !isEnd_)
	{
		if (VSize(VSub(topPos_, downPos_)) < LENGTH) {
			topPos_ = VAdd(topPos_, VScale(moveDir_, moveSpeed_));
		}
	}
	else if (isEnd_)
	{
		downPos_ = VAdd(downPos_, VScale(moveDir_, moveSpeed_));

		// 先端と根元が交差・到達したらブレスを完全終了
		VECTOR remainVec = VSub(topPos_, downPos_);
		if (VSize(remainVec) <= moveSpeed_ || VDot(remainVec, moveDir_) <= 0.0f) {
			ClearCollider();
			return;
		}
	}

	// 更新後の先端・根元座標をカプセルコライダーへ反映
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		auto* capsuleCollider = static_cast<ColliderCapsule*>(vec.get());
		capsuleCollider->SetLocalPosTop(topPos_);
		capsuleCollider->SetLocalPosDown(downPos_);
	}
}