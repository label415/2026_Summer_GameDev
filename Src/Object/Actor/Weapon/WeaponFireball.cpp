#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/Common/ResourceManager.h"
#include "../../../Manager/Common/SoundManager.h"
#include "../../Common/Collider/ColliderSphere.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../../../Manager/Common/SceneManager.h"
#include "../../../Manager/Common/EffectManager.h"
#include "../../../Application.h"
#include "../../../Libs/ImGui/imgui.h"
#include "WeaponFireball.h"

WeaponFireball::WeaponFireball(
	const Transform& followTransform,
	const VECTOR moveDir, int followFrameId)
	: WeaponBase(WeaponBase::WEAPON_TYPE::FIREBALL,followTransform,followFrameId)
	, moveDir_(moveDir)
	, prePos_(AsoUtility::VECTOR_ZERO)
	, radius_(MIN_RADIUS)
	, effectType_(EFFECT_TYPE::NONE)
	, effHandle_(-1)
	, exState_(EXPLOSION_START_TIME)
	, moveSpeed_(SPEED)
{}

void WeaponFireball::Update(void)
{
	// 生存フラグが立っていなければ処理しない
	if (!isAlive_) return;

	// 前フレームの座標を保存
	prePos_ = transform_.pos;

	// 移動処理
	Move();

	// エフェクトタイプがNONEならエフェクト処理を行わない
	if (effectType_ == EFFECT_TYPE::NONE) return;

	// エフェクト更新
	EffectManager::GetInstance().SetPos(effHandle_, transform_.pos);
	EffectManager::GetInstance().SetScl(effHandle_, VScale(AsoUtility::VECTOR_ONE, radius_));
}

void WeaponFireball::Draw(void)
{
	ActorBase::Draw();
}

void WeaponFireball::Release(void)
{
	EffectManager::GetInstance().Stop(effHandle_);
	effHandle_ = -1;
	ActorBase::Release();
}

void WeaponFireball::InitCollider(void)
{
	// 衝突判定の対象タグを設定
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::PLAYER,
		ColliderBase::TAG::STAGE
	};

	// 球体コライダーを作成
	auto colliderSphere = std::make_shared<ColliderSphere>(
		ColliderBase::TAG::ENEMY_WEAPON,
		targetTags,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		MIN_RADIUS
	);

	// 初期状態では当たり判定を無効化
	colliderSphere->SetIsCollider(false);

	// コライダーを所有コライダーに追加
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::SPHERE)].push_back(colliderSphere);
}

void WeaponFireball::InitPost(void)
{
	// 追従元の座標へリセット
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	// 再生ハンドルを初期化
	effHandle_ = -1;

	// 生存フラグを初期化
	isAlive_ = false;
}

void WeaponFireball::Move(void)
{
	bool isHitStage = false;

	// 球体コライダーの衝突判定を確認
	int colliderType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	for (const auto& col : ownColliders_.at(colliderType))
	{
		// 自身のカプセルコライダーを取得
		auto* sphere = static_cast<ColliderSphere*>(col.get());
		for (const auto& hit : sphere->GetHitColliders())
		{
			// 衝突したコライダーを取得
			auto hitCol = hit.lock();
			if (hitCol && hitCol->GetIsCollider()
				&& hitCol->GetTag() == ColliderBase::TAG::STAGE)
			{
				// 衝突フラグを立てる
				isHitStage = true;
				break;
			}
		}
	}

	// すでに爆発中、またはステージに着弾した瞬間
	if (effectType_ == EFFECT_TYPE::EXPLOSION || isHitStage)
	{
		// 爆発中でなければ爆発エフェクトに切り替える
		if (effectType_ != EFFECT_TYPE::EXPLOSION)
		{
			// 着弾SE再生
			SoundManager::GetInstance().PlaySoundId(SoundManager::SOUND_ID::ENEMY_AIR_BREATH2);

			// 爆発エフェクトに切り替え
			transform_.pos = prePos_;
			ChangeEffect(EFFECT_TYPE::EXPLOSION);
		}

		// 爆発中の処理
		if (radius_ <= MAX_RADIUS)
		{
			radius_ += RADIUS_INCREASE;
			for (const auto& col : ownColliders_.at(colliderType))
			{
				// 球体コライダーの半径を更新
				auto* sphere = static_cast<ColliderSphere*>(col.get());
				sphere->SetRadius(radius_);
			}
		}
		else
		{
			// 爆発が終了したら当たり判定をクリアして非表示にする
			ClearCollider();
		}
	}
	else
	{
		// 飛翔中
		transform_.pos = VAdd(transform_.pos, VScale(moveDir_, SPEED));
	}
}

void WeaponFireball::ChangeEffect(EFFECT_TYPE effectType)
{
	// すでに同じエフェクトが再生中なら何もしない
	if (effectType_ == effectType)return;

	// すでに再生中のエフェクトがあれば停止する
	EffectManager::GetInstance().Stop(effHandle_);
	effHandle_ = -1;

	// 新しいエフェクトを再生する
	if (effectType != EFFECT_TYPE::NONE)
	{
		EffectManager::EFFECT_ID id = (effectType == EFFECT_TYPE::BULLET)
			? EffectManager::EFFECT_ID::FIREBALL_BULLET
			: EffectManager::EFFECT_ID::FIREBALL_EXPLOSION;

		effHandle_ = EffectManager::GetInstance().PlayEffect(
			id,
			transform_.pos,
			AsoUtility::VECTOR_ZERO,
			VGet(radius_, radius_, radius_));
	}

	// 現在のエフェクトタイプを更新
	effectType_ = effectType;
}

void WeaponFireball::SetCollider(void)
{
	// 球体コライダーの当たり判定を有効化
	int colliderType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	for (const auto& col : ownColliders_.at(colliderType))
	{
		auto* sphere = static_cast<ColliderSphere*>(col.get());
		sphere->SetIsCollider(true);
	}
}

void WeaponFireball::ClearCollider(void)
{
	// 球体コライダーの当たり判定を無効化
	int colliderType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	for (const auto& col : ownColliders_.at(colliderType))
	{
		auto* sphere = static_cast<ColliderSphere*>(col.get());
		// 当たり判定をオフにする
		sphere->SetIsCollider(false);
		sphere->SetRadius(MIN_RADIUS);
	}

	// 大きさと位置を初期状態へリセット
	radius_ = MIN_RADIUS;

	// 追従元の座標へリセット
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);
	prePos_ = transform_.pos;

	// 再生中のエフェクトがあれば停止する
	EffectManager::GetInstance().Stop(effHandle_);
	effHandle_ = -1;
	effectType_ = EFFECT_TYPE::NONE;

	// 生存フラグをリセット
	isAlive_ = false;
}

void WeaponFireball::Shot(const VECTOR& dir)
{
	// コライダーをクリアして初期状態に戻す
	ClearCollider();

	// 移動方向を設定して座標を追従元の座標にリセット
	moveDir_ = dir;
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);
	prePos_ = transform_.pos;
	radius_ = MIN_RADIUS;

	// 生存フラグを立ててコライダーを有効化
	isAlive_ = true;
	SetCollider();
	ChangeEffect(EFFECT_TYPE::BULLET);
}
