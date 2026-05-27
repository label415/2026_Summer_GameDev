#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../Common/AnimationController.h"
#include "../../../Common/Collider/ColliderLine.h"
#include "../../../Common/Collider/ColliderCapsule.h"
#include "../../../Common/Collider/ColliderModel.h"
#include "../../../Common/Collider/ColliderSphere.h"
#include "../../../Common/Collider/ColliderBase.h"
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

#ifdef _DEBUG

#endif
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

	std::vector<ColliderBase*> colLines;
	colLines.push_back(colLine);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLines);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule1 = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);

	ColliderCapsule* colCapsule2 = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &transform_,
		{50.0f,50.0f,50.0f},
		{ 0.0f,50.0f,-50.0f }, COL_CAPSULE_RADIUS);


	std::vector<ColliderBase*> colCapsules;
	colCapsules.push_back(colCapsule1);
	colCapsules.push_back(colCapsule2);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsules);
}

void EnemyDragon::InitAnimation(void)
{
	anim_ = new AnimationController(transform_.modelId);
	// FBX内のアニメーション設定
	int type = -1;

	type = static_cast<int>(ANIM_TYPE::IDLE);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::WALK);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::FLYING);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::BRACELET_ATTACK);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::HOVER);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::TAKEOFF);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::LANDS);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::DIE);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::FLYING_ATTACK);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::ROAR);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::MELEE_ATTACK);
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
	stateChanges_.emplace(static_cast<int>(STATE::ROAR),
		std::bind(&EnemyDragon::ChangeStateRoar, this));
	stateChanges_.emplace(static_cast<int>(STATE::FLYING),
		std::bind(&EnemyDragon::ChangeStateFlying, this));
	stateChanges_.emplace(static_cast<int>(STATE::BRACELET_ATTACK),
		std::bind(&EnemyDragon::ChangeStateBreathAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::FLYING_ATTACK),
		std::bind(&EnemyDragon::ChangeStateFlyingAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::MELEE_ATTACK),
		std::bind(&EnemyDragon::ChangeStateMeleeAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::HOVER),
		std::bind(&EnemyDragon::ChangeStateHover, this));
	stateChanges_.emplace(static_cast<int>(STATE::TAKEOFF),
		std::bind(&EnemyDragon::ChangeStateTakeOff, this));
	stateChanges_.emplace(static_cast<int>(STATE::LANDS),
		std::bind(&EnemyDragon::ChangeStateLands, this));
	stateChanges_.emplace(static_cast<int>(STATE::DEAD),
		std::bind(&EnemyDragon::ChangeStateDead, this));
	stateChanges_.emplace(static_cast<int>(STATE::END),
		std::bind(&EnemyDragon::ChangeStateEnd, this));

	

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
	if (rand < 20)
	{
		ChangeState(STATE::IDLE);
	}
	else if (rand < 10) {
		ChangeState(STATE::ROAR);
	}
	else{
		ChangeState(STATE::PATROL);
	}
}

void EnemyDragon::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateIdle, this);
	// ランダムな待機時間
	step_ = 1.0f + static_cast<float>(GetRand(3));
	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 待機アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);
}

void EnemyDragon::ChangeStateRoar(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateRoar, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 待機アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::ROAR), false);
}

void EnemyDragon::ChangeStatePatrol(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdatePatrol, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::WALK), true);
}

void EnemyDragon::ChangeStateChase(void)
{
}

void EnemyDragon::ChangeStateFlying(void)
{
}

void EnemyDragon::ChangeStateFlyingAttack(void)
{
}

void EnemyDragon::ChangeStateBreathAttack(void)
{
}

void EnemyDragon::ChangeStateMeleeAttack(void)
{
}

void EnemyDragon::ChangeStateHover(void)
{
}

void EnemyDragon::ChangeStateTakeOff(void)
{
}

void EnemyDragon::ChangeStateLands(void)
{
}

void EnemyDragon::ChangeStateDead(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateDead, this);
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
}

void EnemyDragon::UpdateRoar(void)
{
	if (anim_->IsEnd()) {
		// 待機終了
		ChangeState(STATE::THINK);
		return;
	}
}

void EnemyDragon::UpdatePatrol(void)
{
	SetTargetCollider();
	float dot = VDot(VNorm(transform_.GetForward()), targetCollider_->GetFollow()->pos);
	float angle = acosf(dot);
	float a = AsoUtility::Deg2RadF(30.0f);
	if (angle <= a){
		ChangeState(STATE::CHASE);
		return;
	};
}

void EnemyDragon::UpdateChase(void)
{
}

void EnemyDragon::UpdateFlying(void)
{
}

void EnemyDragon::UpdateFlyingAttack(void)
{
}

void EnemyDragon::UpdateBreathAttack(void)
{
}

void EnemyDragon::UpdateMeleeAttack(void)
{
}

void EnemyDragon::UpdateHover(void)
{
}

void EnemyDragon::UpdateTakeOff(void)
{
}

void EnemyDragon::UpdateLands(void)
{
}

void EnemyDragon::UpdateDead(void)
{
}

void EnemyDragon::UpdateEnd(void)
{
}

void EnemyDragon::SetTargetCollider(void)
{
	for (const auto& hitCol : hitColliders_)
	{
		for (const auto& i : hitCol.second)
		{
			// プレイヤーのカプセル以外は処理を飛ばす
			if (i->GetTag() != ColliderBase::TAG::PLAYER) continue;

			//派生クラスへキャスト
			const ColliderCapsule* colliderCapsule =
				dynamic_cast<const ColliderCapsule*>(i);

			if (colliderCapsule == nullptr)continue;

			targetCollider_ = colliderCapsule;
		}
	}
}
