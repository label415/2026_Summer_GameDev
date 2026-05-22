#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../Common/AnimationController.h"
#include "../../../Common/Collider/ColliderLine.h"
#include "../../../Common/Collider/ColliderCapsule.h"
#include "../../../Common/Collider/ColliderModel.h"
#include "../../../Common/Collider/ColliderSphere.h"
#include "EnemyDragon.h"

EnemyDragon::EnemyDragon(const EnemyBase::EnemyData& data)
	:
	EnemyBase(data),
	state_(STATE::NONE),
	step_(0.0f)
{
}

EnemyDragon::~EnemyDragon(void)
{
}

void EnemyDragon::Draw(void)
{
	// 基底クラスの描画処理
	CharactorBase::Draw();

	if (attackShere.isAlive) {
		DrawSphere3D(attackShere.pos_, ATTACK_SHOT_RADIUS, 12,
			GetColor(255, 255, 255), GetColor(255, 255, 255), true);
	}

#ifdef _DEBUG
	// 巡回ルート描画
	for (const auto& point : wayPoints_)
	{
		DrawSphere3D(
			point, 50.0f, 10,
			0x0000ff, 0x0000ff, false);
	}
#endif // _DEBUG
}

void EnemyDragon::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();
	// モデルのロード
	transform_.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY_DRAGON));
}

void EnemyDragon::InitTransform(void)
{
	// ロボット自身
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, SCALE);
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal = Quaternion::Euler(DEFAULT_LOCAL_ROT);
	transform_.Update();
}

void EnemyDragon::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(
		static_cast<int>(ColliderBase::SHAPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule1 = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);
	ownColliders_.emplace(
		static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule1);
}

void EnemyDragon::InitAnimation(void)
{
	anim_ = new AnimationController(transform_.modelId);
	// FBX内のアニメーション設定
	int type = -1;
	type = static_cast<int>(ANIM_TYPE::FIRE);
	anim_->AddInFbx(type, 10.0f, type);

	type = static_cast<int>(ANIM_TYPE::IDLE);
	anim_->AddInFbx(type, 20.0f, type);

	type = static_cast<int>(ANIM_TYPE::WALK);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::RUN);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::FLY);
	anim_->AddInFbx(type, 45.0f, type);

	type = static_cast<int>(ANIM_TYPE::HIT);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::LOWER);
	anim_->AddInFbx(type, 30.0f, type);

	// 初期アニメーション再生
	anim_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void EnemyDragon::InitPost(void)
{
	// 状態遷移初期処理登録
	stateChanges_.emplace(static_cast<int>(STATE::NONE),
		std::bind(&EnemyDragon::ChangeStateNone, this));
	stateChanges_.emplace(static_cast<int>(STATE::THINK),
		std::bind(&EnemyDragon::ChangeStateThink, this));
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&EnemyDragon::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::PATROL),
		std::bind(&EnemyDragon::ChangeStatePatrol, this));
	stateChanges_.emplace(static_cast<int>(STATE::CHASE),
		std::bind(&EnemyDragon::ChangeStateChase, this));
	stateChanges_.emplace(static_cast<int>(STATE::NOMAL_FLY),
		std::bind(&EnemyDragon::ChangeNomalFly, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK_BREATH),
		std::bind(&EnemyDragon::ChangeStateAttackBreath, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK_FLY_BREATH),
		std::bind(&EnemyDragon::ChangeStateAttackFlyBreath, this));
	stateChanges_.emplace(static_cast<int>(STATE::DEAD),
		std::bind(&EnemyDragon::ChangeStateDead, this));
	stateChanges_.emplace(static_cast<int>(STATE::KNOCKBACK),
		std::bind(&EnemyDragon::ChangeStateKnockBack, this));
	stateChanges_.emplace(static_cast<int>(STATE::END),
		std::bind(&EnemyDragon::ChangeStateEnd, this));

	// 巡回ルート
	wayPoints_.emplace_back(VGet(1926.18f, 1.76f, 618.34f));
	wayPoints_.emplace_back(VGet(2553.30f, -11.82f, -593.32f));
	wayPoints_.emplace_back(VGet(1400.34f, -26.21f, -457.11f));
	wayPoints_.emplace_back(VGet(1274.85f, -49.35f, 168.02f));

	// 初期状態設定
	ChangeState(STATE::THINK);
}

void EnemyDragon::UpdateProcess(void)
{
	// 状態別更新
	stateUpdate_();
}

void EnemyDragon::UpdateProcessPost(void)
{
	EnemyBase::UpdateProcessPost();

	float diff = VSize(VSub(attackShere.pos_, transform_.pos));
	if (diff >= 10000.0f && attackShere.isAlive == true) {
		attackShere.isAlive = false;
	}
}

void EnemyDragon::ChangeState(STATE state)
{
	state_ = state;
	EnemyBase::ChangeState(static_cast<int>(state_));
}

void EnemyDragon::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateNone, this);
}

void EnemyDragon::ChangeStateThink(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateThink, this);
	// 思考
	// ランダムに次の行動を決定
	// 30%で待機、70%で徘徊
	int rand = GetRand(100);
	if (rand < 30)
	{
		ChangeState(STATE::IDLE);
	}
	else
	{
		ChangeState(STATE::PATROL);
	}

}

void EnemyDragon::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateIdle, this);
	// ランダムな待機時間
	step_ = 3.0f + static_cast<float>(GetRand(3));
	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;
	// 待機アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);
}

void EnemyDragon::ChangeStatePatrol(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdatePatrol, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;
	if (activeWayPointIndex_ == wayPoints_.size())
	{
		// 巡回終了
		activeWayPointIndex_ = 0;
		ChangeState(STATE::THINK);
		return;
	}
	// 次の巡回ポイント更新
	nextWayPoint_ = wayPoints_[activeWayPointIndex_];
	// 巡回ルートの移動方向を設定する
	SetMoveDirPatrol();
	// 移動スピード
	moveSpeed_ = 4.0f;
	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::WALK), true);

}

void EnemyDragon::ChangeStateChase(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateChase, this);

	// 移動スピード
	moveSpeed_ = 6.0f;

	float rand = GetRand(3.0f);
	changeShotCnt = ATTACK_SHOT_TIME + rand;

	anim_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
}

void EnemyDragon::ChangeNomalFly(void)
{
}

void EnemyDragon::ChangeStateAttackFlyBreath(void)
{
}

void EnemyDragon::ChangeStateAttackBreath(void)
{
}

void EnemyDragon::ChangeStateDead(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateDead, this);
}

void EnemyDragon::ChangeStateKnockBack(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateKnockBack, this);
}

void EnemyDragon::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateEnd, this);
}

void EnemyDragon::UpdateNone(void)
{
}

void EnemyDragon::UpdateThink(void)
{
}

void EnemyDragon::UpdateIdle(void)
{
	step_ -= scnMng_.GetDeltaTime();
	if (step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::THINK);
		return;
	}

	if (InSearchConeModel())
	{
		ChangeState(STATE::IDLE);
	}
}

void EnemyDragon::UpdatePatrol(void)
{
	// 巡回ポイントとの球体衝突判定(半径30.0fくらい)
	if (AsoUtility::IsHitSpheres(
		transform_.pos, 30.0f,
		nextWayPoint_, 30.0f))
	{
		// 巡回ポイントインデックス更新
		activeWayPointIndex_++;
		// 次の移動地点へか、待機か思考
		ChangeState(STATE::THINK);
		return;
	}
	// 巡回ルートの移動方向を設定する
	SetMoveDirPatrol();

	// 移動量の計算
	movePow_ = VScale(moveDir_, moveSpeed_);

	if (InSearchConeModel())
	{
		ChangeState(STATE::PATROL);
	}
}

void EnemyDragon::UpdateChase(void)
{
	changeShotCnt -= scnMng_.GetDeltaTime();

	for (const auto& hitCol : hitColliders_)
	{
		for (const auto& i : hitCol)
		{
			// プレイヤーのカプセル以外は処理を飛ばす
			if (i->GetTag() != ColliderBase::TAG::PLAYER) continue;
			//派生クラスへキャスト
			const ColliderCapsule* colliderCapsule =
				dynamic_cast<const ColliderCapsule*>(i);

			VECTOR diff = VSub(colliderCapsule->GetFollow()->pos, transform_.pos);

			moveDir_ = VNorm(diff);

			if (VSize(diff) <= VIEW_ANGLE_ATTACK) {
				ChangeState(STATE::ATTACK_BREATH);
			}
		}
	}

	if (changeShotCnt <= 0.0f) {
		ChangeState(STATE::ATTACK_FLY_BREATH);
	}

	// 移動量の計算
	movePow_ = VScale(moveDir_, moveSpeed_);

	if (!InSearchConeModel())
	{
		ChangeState(STATE::PATROL);
	}
}

void EnemyDragon::UpdateNomalFly(void)
{
}

void EnemyDragon::UpdateAttackFlyBreath(void)
{
}

void EnemyDragon::UpdateAttackBreath(void)
{
}

void EnemyDragon::UpdateDead(void)
{
}

void EnemyDragon::UpdateKnockBack(void)
{
}

void EnemyDragon::UpdateEnd(void)
{
}

void EnemyDragon::SetMoveDirPatrol(void)
{
	// 巡回先座標XZ
	VECTOR tmpPos = nextWayPoint_;
	tmpPos.y = 0.0f;
	// 現在地座標XZ
	VECTOR pos = transform_.pos;
	pos.y = 0.0f;
	// XZ平面上の移動方向を計算
	moveDir_ = VNorm(VSub(tmpPos, pos));
}

bool EnemyDragon::InSearchConeModel(void)
{
	bool ret = false;

	// モデルライダ  
	int modelType = static_cast<int>(ColliderBase::TAG::VIEW_RANGE);

	if (ownColliders_.count(modelType) == 0)return ret;

	const auto& vecs = ownColliders_.at(modelType);
	for (const auto& vec : vecs)
	{
		// モデルコライダ情報  
		ColliderModel* colliderModel =
			dynamic_cast<ColliderModel*>(vec);

		if (colliderModel == nullptr)return ret;

		MV1RefreshCollInfo(colliderModel->GetFollow()->modelId);

		for (const auto& hitCol : hitColliders_)
		{
			for (const auto& i : hitCol)
			{
				// プレイヤーのカプセル以外は処理を飛ばす
				if (i->GetTag() != ColliderBase::TAG::PLAYER) continue;

				//派生クラスへキャスト
				const ColliderCapsule* colliderCapsule =
					dynamic_cast<const ColliderCapsule*>(i);

				if (colliderCapsule == nullptr)continue;

				if (colliderCapsule->IsHit(colliderModel, false, false))
				{
					return true;
				}
			}
		}
	}

	return ret;
}
