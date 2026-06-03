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
	step_(0.0f),
	attribute_(ATTRIBUTE::NONE)
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

	VECTOR pos = MV1GetFramePosition(transform_.modelId, 1);

	DrawFormatString(0, 20, 0x000000, L"モデルフレーム座標 %.1f, %.1f, %.1f", pos.x, pos.y, pos.z);

	std::wstring stateName[] = {
		L"NONE",L"THINK",L"IDLE",L"ROAR",L"PATROL",
		L"CHASE",L"FLYING",L"FLYING_ATTACK",L"BRACELET_ATTACK",
		L"MELEE_ATTACK",L"HOVER",L"TAKEOFF",L"LANDS",L"DEAD",L"END" };

	std::wstring attributeName[] = { L"NONE",L"ABOVE_GROUND",L"AIR" };

	DrawFormatString(0, 40, 0x000000, L"Enemy State: %s", stateName[static_cast<int>(state_)].c_str());
	DrawFormatString(0, 60, 0x000000, L"Enemy Attribute: %s", attributeName[static_cast<int>(attribute_)].c_str());

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
	transform_.quaRot = Quaternion::Identity();
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

	/*ColliderCapsule* colCapsule2 = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &transform_,
		{50.0f,50.0f,50.0f},
		{ 0.0f,50.0f,-50.0f }, COL_CAPSULE_RADIUS);*/


	std::vector<ColliderBase*> colCapsules;
	colCapsules.push_back(colCapsule1);
	/*colCapsules.push_back(colCapsule2);*/
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
	ChangeState(STATE::ROAR);
}

void EnemyDragon::UpdateProcess(void)
{
	for (const auto& hitCol : hitColliders_)
	{
		for (const auto& i : hitCol.second)
		{
			// モデル以外は処理を飛ばす  
			if (i->GetShape() != ColliderBase::SHAPE::MODEL
				&& i->GetTag() != ColliderBase::TAG::PLAYER) continue;

			const ColliderCapsule* colliderCapsule =
				dynamic_cast<ColliderCapsule*>(i);

			if (colliderCapsule == nullptr) continue;

			VECTOR nom = VNorm(VSub(colliderCapsule->GetFollow()->pos, transform_.pos));

			moveDir_ = nom;
		}
	}

	anim_->SetRoot(L"Bone", LOCK_POS);
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
	int rand = GetRand(100);

	if(attribute_ == ATTRIBUTE::ABOVE_GROUND)
	{
		if (rand < 20)
		{
			ChangeState(STATE::PATROL);
		}
		else if (rand > 20
			&& rand < 60) {
			ChangeState(STATE::TAKEOFF);
		}
		else {
			ChangeState(STATE::MELEE_ATTACK);
		}
	}
	if (attribute_ == ATTRIBUTE::AIR) 
	{
		// 思考
		int rand = GetRand(100);
		if (rand < 35) {
			ChangeState(STATE::FLYING_ATTACK);
		}
		else if (rand > 35
			&& rand < 70) {
			ChangeState(STATE::FLYING);
		}
		else {
			ChangeState(STATE::LANDS);
		}
	}
}

void EnemyDragon::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateIdle, this);
	// ランダムな待機時間
	step_ = 1.0f + static_cast<float>(GetRand(2));
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
		static_cast<int>(ANIM_TYPE::WALK), false);
}

void EnemyDragon::ChangeStateChase(void)
{
}

void EnemyDragon::ChangeStateFlying(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateFlying, this);

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::FLYING), false);
}

void EnemyDragon::ChangeStateFlyingAttack(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateFlyingAttack, this);

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::FLYING_ATTACK), false);
}

void EnemyDragon::ChangeStateBreathAttack(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateBreathAttack, this);

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::BRACELET_ATTACK), false);
}

void EnemyDragon::ChangeStateMeleeAttack(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateMeleeAttack, this);

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::MELEE_ATTACK), false);
}

void EnemyDragon::ChangeStateHover(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateHover, this);

	// ランダムな待機時間
	step_ = 1.0f + static_cast<float>(GetRand(2));

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::HOVER), true);
}

void EnemyDragon::ChangeStateTakeOff(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateTakeOff, this);
	anim_->Play(
		static_cast<int>(ANIM_TYPE::TAKEOFF), false);
}

void EnemyDragon::ChangeStateLands(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateLands, this);

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::LANDS), false);
}

void EnemyDragon::ChangeStateDead(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateDead, this);

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::DIE), false);
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
		attribute_ = ATTRIBUTE::ABOVE_GROUND;
		ChangeState(STATE::THINK);
		return;
	}
}

void EnemyDragon::UpdatePatrol(void)
{
	moveSpeed_ = SPEED_MOVE;
	movePow_ = VScale(moveDir_, moveSpeed_);

	if (anim_->IsEnd()){
		ChangeState(STATE::IDLE);
		return;
	};
}

void EnemyDragon::UpdateChase(void)
{
}

void EnemyDragon::UpdateFlying(void)
{
	transform_.pos.y = MAX_TAKE;

	moveSpeed_ = SPEED_MOVE;
	movePow_ = VScale(moveDir_, moveSpeed_);

	if (anim_->IsEnd())
	{
		ChangeState(STATE::HOVER);
	}
}

void EnemyDragon::UpdateFlyingAttack(void)
{
	transform_.pos.y = MAX_TAKE;

	moveSpeed_ = SPEED_MOVE;
	movePow_ = VScale(moveDir_, moveSpeed_);

	if (anim_->IsEnd())
	{
		ChangeState(STATE::HOVER);
	}
}

void EnemyDragon::UpdateBreathAttack(void)
{
	if (anim_->IsEnd())
	{
		ChangeState(STATE::IDLE);
	}
}

void EnemyDragon::UpdateMeleeAttack(void)
{
	if (anim_->IsEnd())
	{
		ChangeState(STATE::IDLE);
	}
}

void EnemyDragon::UpdateHover(void)
{
	transform_.pos.y = MAX_TAKE;

	step_ -= scnMng_.GetDeltaTime();
	if (step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::THINK);
		return;
	}
}

void EnemyDragon::UpdateTakeOff(void)
{
	if(transform_.pos.y <= MAX_TAKE)
	{
		float jumpSpeed = 690.0f * scnMng_.GetDeltaTime();
		jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_U, jumpSpeed));
	}
	else {
		transform_.pos.y = MAX_TAKE;
	}
	
	// 歩きアニメーション再生
	if (anim_->IsEnd())
	{
		attribute_ = ATTRIBUTE::AIR;
		ChangeState(STATE::HOVER);
	}
}

void EnemyDragon::UpdateLands(void)
{
	if (anim_->IsEnd())
	{
		attribute_ = ATTRIBUTE::ABOVE_GROUND;
		ChangeState(STATE::IDLE);
	}
}

void EnemyDragon::UpdateDead(void)
{
	if (anim_->IsEnd())
	{
		ChangeState(STATE::END);
	}
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
