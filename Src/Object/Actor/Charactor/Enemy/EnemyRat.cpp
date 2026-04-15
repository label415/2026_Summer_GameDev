#include "../../../../Manager/ResourceManager.h"
#include "../../../../Common/Quaternion.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../Common/Collider/ColliderLine.h"
#include "../../../Common/Collider/ColliderCapsule.h"
#include "EnemyRat.h"

EnemyRat::EnemyRat(const EnemyBase::EnemyData& data)
	:
	EnemyBase(data)
{
}

EnemyRat::~EnemyRat(void)
{
}

void EnemyRat::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();

	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::ENEMY_RAT));
}

void EnemyRat::InitTransform(void)
{
	transform_.scl = { SCALE, SCALE, SCALE };
	transform_.quaRotLocal = Quaternion::Euler(ROT);
	transform_.Update();
}

void EnemyRat::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
}

void EnemyRat::InitAnimation(void)
{
	anim_ = new AnimationController(transform_.modelId);

	anim_->AddInFbx(static_cast<int>(ANIM_TYPE::IDLE), 30.0f, 9);
	anim_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), 30.0f, 13);

	anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void EnemyRat::InitPost(void)
{
	// 状態遷移初期処理登録
	stateChanges_.emplace(static_cast<int>(STATE::NONE),
		std::bind(&EnemyRat::ChangeStateNone, this));
	stateChanges_.emplace(static_cast<int>(STATE::THINK),
		std::bind(&EnemyRat::ChangeStateThink, this));
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&EnemyRat::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::WANDER),
		std::bind(&EnemyRat::ChangeStateWander, this));
	stateChanges_.emplace(static_cast<int>(STATE::END),
		std::bind(&EnemyRat::ChangeStateEnd, this));

	// 初期状態設定
	ChangeState(STATE::THINK);
}

void EnemyRat::UpdateProcess(void)
{
	// 状態別更新
	stateUpdate_();
}

void EnemyRat::UpdateProcessPost(void)
{
	if (!InMovableRange())
	{
		transform_.pos = prevPos_;

		ChangeState(STATE::THINK);
	}
}

void EnemyRat::CollisionReserve(void)
{

	// 通常時の線分に戻す
	if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::LINE)) != 0)
	{
		ColliderLine* colLine = dynamic_cast<ColliderLine*>(
			ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::LINE)));
		colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
		colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
	}
	// 通常時のカプセルに戻す
	if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::CAPSULE)) != 0)
	{
		ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
			ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE)));
		colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
		colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
	}
}

void EnemyRat::ChangeState(STATE state)
{
	state_ = state;

	// 各状態遷移の初期処理
	EnemyBase::ChangeState(static_cast<int>(state_));
}

void EnemyRat::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateNone, this);
}

void EnemyRat::ChangeStateThink(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateThink, this);

	stepCnt_ = 0.0f;

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
		ChangeState(STATE::WANDER);
	}
}

void EnemyRat::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateIdle, this);

	// ランダムな待機時間
	step_ = 3.0f + static_cast<float>(GetRand(3));
	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;
	// 待機アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);
}

void EnemyRat::ChangeStateWander(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateWander, this);

	// ランダムな角度
	float angle = static_cast<float>(GetRand(360)) * DX_PI_F / 180.0f;
	// 移動方向
	moveDir_ = VGet(cosf(angle), 0.0f, sinf(angle));
	// ランダムな移動時間
	step_ = 2.0f + static_cast<float>(GetRand(5));
	// 移動スピード
	moveSpeed_ = 3.0f;
	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::WALK), true);
}

void EnemyRat::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateEnd, this);
}

void EnemyRat::UpdateNone(void)
{
}

void EnemyRat::UpdateThink(void)
{
}

void EnemyRat::UpdateIdle(void)
{
	if (stepCnt_ > step_)
	{
		ChangeState(STATE::THINK);
	}
	stepCnt_+= 0.1f;

	movePow_ = AsoUtility::VECTOR_ZERO;
}

void EnemyRat::UpdateWander(void)
{
	if (stepCnt_ > step_)
	{
		ChangeState(STATE::THINK);
	}

	stepCnt_ += 0.1f;
	movePow_ = VScale(moveDir_, moveSpeed_);
}

void EnemyRat::UpdateEnd(void)
{
}

