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

WeponFlameThrower::WeponFlameThrower(
	const Transform& followTransform,
	const VECTOR moverDir, int followFrameId)
	: WeponBase(WeponBase::WEAPON_TYPE::FLAME,followTransform,followFrameId)
	, moveDir_(moverDir)
	, prePos_(AsoUtility::VECTOR_ZERO)
	, radius_(MIN_RADIUS)
	, effectType_(EFFECT_TYPE::NONE)
	, exState_(EXPLOSION_STATO_TIME)
	, moveSpeed_(SPEED)
{}

WeponFlameThrower::~WeponFlameThrower(void){}

void WeponFlameThrower::Update(void)
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
	effect_->SetEffectPos(static_cast<int>(effectType_), transform_.pos);
	effect_->SetEffectScl(static_cast<int>(effectType_), VScale(AsoUtility::VECTOR_ONE, radius_));
	effect_->Update(static_cast<int>(effectType_));
}

void WeponFlameThrower::InitLoad(void){}

void WeponFlameThrower::InitTransform(void){}

void WeponFlameThrower::InitCollider(void)
{
	// 衝突判定の対象タグを設定
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::PLAYER,
		ColliderBase::TAG::STAGE
	};

	// 球体コライダーを作成
	auto colliderSphere = std::make_shared<ColliderSphere>(
		ColliderBase::TAG::ENEMY_WEPON,
		targetTags,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		MIN_RADIUS
	);

	// 初期状態では当たり判定を無効化
	colliderSphere->SetIsCollier(false);

	// コライダーを所有コライダーに追加
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::SPHERE)].push_back(colliderSphere);
	CollisionManager::GetInstance().AddCollider(colliderSphere);
}

void WeponFlameThrower::InitAnimation(void){}

void WeponFlameThrower::InitPost(void)
{
	// 追従元の座標へリセット
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	// エフェクトコントローラの作成とエフェクトの追加
	effect_ = std::make_unique<EffectController>();
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::BULLET),
		(Application::PATH_EFFECT + L"FireBall_Bullet.efkefc"));
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::EXPLOSION),
		(Application::PATH_EFFECT + L"FireBall_Explosion.efkefc"));

	// 生存フラグを初期化
	isAlive_ = false;
}

void WeponFlameThrower::Move(void)
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
			if (hitCol && hitCol->GetIsCollier()
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
			int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_ARE_BREASE_2).handleId_;
			SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_ARE_ENEMY_BREASE2, bgm_, 100);

			// 爆発エフェクトに切り替え
			transform_.pos = prePos_;
			ChangerEffect(EFFECT_TYPE::EXPLOSION);
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

void WeponFlameThrower::ChangerEffect(EFFECT_TYPE effectType)
{
	// すでに同じエフェクトが再生中なら何もしない
	if (effectType_ == effectType)return;

	// すでに再生中のエフェクトがあれば停止する
	if(effectType_ != EFFECT_TYPE::NONE){
		effect_->Stop(static_cast<int>(effectType_));
	}

	// 新しいエフェクトを再生する
	effect_->Play(
		static_cast<int>(effectType),
		transform_.pos,
		VGet(0.0f, 0.0f, 0.0f),
		VGet(radius_, radius_, radius_));

	// 現在のエフェクトタイプを更新
	effectType_ = effectType;
}

void WeponFlameThrower::Draw(void)
{
	// 基底クラスの描画処理
	ActorBase::Draw();
}

void WeponFlameThrower::Release(void)
{
	// 基底クラスの解放処理
	ActorBase::Release();
}

void WeponFlameThrower::SetCollider(void)
{
	// 球体コライダーの当たり判定を有効化
	int colliderType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	for (const auto& col : ownColliders_.at(colliderType))
	{
		auto* sphere = static_cast<ColliderSphere*>(col.get());
		sphere->SetIsCollier(true);
	}
}

void WeponFlameThrower::ClearCollider(void)
{
	// 球体コライダーの当たり判定を無効化
	int colliderType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	for (const auto& col : ownColliders_.at(colliderType))
	{
		auto* sphere = static_cast<ColliderSphere*>(col.get());
		// 当たり判定をオフにする
		sphere->SetIsCollier(false);
		sphere->SetRadius(MIN_RADIUS);
	}

	// 大きさと位置を初期状態へリセット
	radius_ = MIN_RADIUS;

	// 追従元の座標へリセット
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);
	prePos_ = transform_.pos;

	// 再生中のエフェクトがあれば停止する
	if (effectType_ != EFFECT_TYPE::NONE)
	{
		effect_->Stop(static_cast<int>(effectType_));
		effectType_ = EFFECT_TYPE::NONE;
	}

	// 生存フラグをリセット
	isAlive_ = false;
}

void WeponFlameThrower::Shot(const VECTOR& dir)
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
	ChangerEffect(EFFECT_TYPE::BULLET);
}
