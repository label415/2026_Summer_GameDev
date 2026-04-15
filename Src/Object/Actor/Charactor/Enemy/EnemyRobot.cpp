#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../Common/AnimationController.h"
#include "../../../Common/Collider/ColliderLine.h"
#include "../../../Common/Collider/ColliderCapsule.h"
#include "../../../Common/Collider/ColliderModel.h"
#include "../../../Common/Collider/ColliderSphere.h"
#include "../../UI/UISurprise.h"
#include "EnemyRobot.h"

EnemyRobot::EnemyRobot(const EnemyBase::EnemyData& data)
	:
	EnemyBase(data),
	state_(STATE::NONE),
	step_(0.0f)
{
}

EnemyRobot::~EnemyRobot(void)
{
}

void EnemyRobot::Draw(void)
{
	// 基底クラスの描画処理
	CharactorBase::Draw();

	uiSurprise_->Draw();

	if (attackShere.isAlive) {
		DrawSphere3D(attackShere.pos_, ATTACK_SHOT_RADIUS, 12,
			GetColor(255, 255, 255), GetColor(255,255,255), true);
	}

#pragma region 視野(円錐)の描画
	SetUseLighting(FALSE);
	MV1SetFrameOpacityRate(viewRangeTransform_.modelId, 2, 0.3f);
	MV1DrawModel(viewRangeTransform_.modelId);
	SetUseLighting(TRUE);
#pragma endregion

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

void EnemyRobot::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();
	// モデルのロード
	transform_.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY_ROBOT));
	// 視野(円錐)モデルのロード
	viewRangeTransform_.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::VIEW_RANGE));
}

void EnemyRobot::InitTransform(void)
{
	// ロボット自身
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, SCALE);
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal = Quaternion::Euler(DEFAULT_LOCAL_ROT);
	transform_.Update();

	// 視野用円錐モデル
	viewRangeTransform_.scl = VIEW_RANGE_SCL;
	viewRangeTransform_.pos =
		MV1GetFramePosition(transform_.modelId, VIEW_RANGE_SYNC_FRAME_IDX);
	viewRangeTransform_.quaRot =
		transform_.quaRot.Mult(
			Quaternion::AngleAxis(VIEW_RANGE_ROT_X, AsoUtility::AXIS_X));
	viewRangeTransform_.quaRotLocal =
		Quaternion::AngleAxis(VIEW_RANGE_LOCAL_ROT_X, AsoUtility::AXIS_X);
	viewRangeTransform_.Update();
}

void EnemyRobot::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(
		static_cast<int>(ColliderBase::SHAPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);
	ownColliders_.emplace(
		static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);


	MV1SetupCollInfo(viewRangeTransform_.modelId);

	//モデルのコライダー
	ColliderModel* colliderModel =
		new ColliderModel(ColliderBase::TAG::VIEW_RANGE, &viewRangeTransform_);
	ownColliders_.emplace(
		static_cast<int>(ColliderBase::SHAPE::MODEL), colliderModel);
}

void EnemyRobot::InitAnimation(void)
{
	anim_ = new AnimationController(transform_.modelId);
	// FBX内のアニメーション設定
	int type = -1;
	type = static_cast<int>(ANIM_TYPE::DANCE);
	anim_->AddInFbx(type, 10.0f, type);

	type = static_cast<int>(ANIM_TYPE::IDLE);
	anim_->AddInFbx(type, 20.0f, type);

	type = static_cast<int>(ANIM_TYPE::WALK);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::RUN);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::KICK);
	anim_->AddInFbx(type, 45.0f, type);

	type = static_cast<int>(ANIM_TYPE::SHOOT);
	anim_->AddInFbx(type, 30.0f, type);

	// 初期アニメーション再生
	anim_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void EnemyRobot::InitPost(void)
{
	// 状態遷移初期処理登録
	stateChanges_.emplace(static_cast<int>(STATE::NONE),
		std::bind(&EnemyRobot::ChangeStateNone, this));
	stateChanges_.emplace(static_cast<int>(STATE::THINK),
		std::bind(&EnemyRobot::ChangeStateThink, this));
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&EnemyRobot::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::PATROL),
		std::bind(&EnemyRobot::ChangeStatePatrol, this));
	stateChanges_.emplace(static_cast<int>(STATE::SURPRISE),
		std::bind(&EnemyRobot::ChangeStateSurprise, this));
	stateChanges_.emplace(static_cast<int>(STATE::ALERT),
		std::bind(&EnemyRobot::ChangeStateAlert, this));
	stateChanges_.emplace(static_cast<int>(STATE::CHASE),
		std::bind(&EnemyRobot::ChangeStateChase, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK_KICK),
		std::bind(&EnemyRobot::ChangeStateAttackKick, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK_SHOOT),
		std::bind(&EnemyRobot::ChangeStateAttackShoot, this));
	stateChanges_.emplace(static_cast<int>(STATE::ESCAPE),
		std::bind(&EnemyRobot::ChangeStateEscape, this));
	stateChanges_.emplace(static_cast<int>(STATE::DEAD),
		std::bind(&EnemyRobot::ChangeStateDead, this));
	stateChanges_.emplace(static_cast<int>(STATE::KNOCKBACK),
		std::bind(&EnemyRobot::ChangeStateKnockBack, this));
	stateChanges_.emplace(static_cast<int>(STATE::END),
		std::bind(&EnemyRobot::ChangeStateEnd, this));

	// 巡回ルート
	wayPoints_.emplace_back(VGet(1926.18f, 1.76f, 618.34f));
	wayPoints_.emplace_back(VGet(2553.30f, -11.82f, -593.32f));
	wayPoints_.emplace_back(VGet(1400.34f, -26.21f, -457.11f));
	wayPoints_.emplace_back(VGet(1274.85f, -49.35f, 168.02f));

	uiSurprise_ = new UISurprise(&transform_, 200.0f, 80.0f, 96.0f);
	uiSurprise_->Init();

	// 初期状態設定
	ChangeState(STATE::THINK);
}

void EnemyRobot::UpdateProcess(void)
{
	// 状態別更新
	stateUpdate_();
}

void EnemyRobot::UpdateProcessPost(void)
{
	EnemyBase::UpdateProcessPost();

	viewRangeTransform_.pos =
		MV1GetFramePosition(transform_.modelId, VIEW_RANGE_SYNC_FRAME_IDX);
	viewRangeTransform_.quaRot =
		transform_.quaRot.Mult(
			Quaternion::AngleAxis(VIEW_RANGE_ROT_X, AsoUtility::AXIS_X));
	viewRangeTransform_.Update();

	float diff = VSize(VSub(attackShere.pos_, transform_.pos));
	if (diff >= 10000.0f && attackShere.isAlive == true){ 
		attackShere.isAlive = false;
	}
}

void EnemyRobot::ChangeState(STATE state)
{
	state_ = state;
	EnemyBase::ChangeState(static_cast<int>(state_));
}

void EnemyRobot::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateNone, this);
}

void EnemyRobot::ChangeStateThink(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateThink, this);
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

	MV1SetMaterialDifColor(viewRangeTransform_.modelId, 0, GetColorF(0.0f, 0.0f, 1.0f, 1.0f));
}

void EnemyRobot::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateIdle, this);
	// ランダムな待機時間
	step_ = 3.0f + static_cast<float>(GetRand(3));
	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;
	// 待機アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);

	MV1SetMaterialDifColor(viewRangeTransform_.modelId, 0, GetColorF(0.0f, 0.0f, 1.0f, 1.0f));
}

void EnemyRobot::ChangeStatePatrol(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdatePatrol, this);

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

	MV1SetMaterialDifColor(viewRangeTransform_.modelId, 0, GetColorF(0.0f, 0.0f, 1.0f, 1.0f));
}

void EnemyRobot::ChangeStateSurprise(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateSurprise, this);
	uiSurprise_->Start();
	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;
	Cnt = 0.0f;
	// 待機アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);
}

void EnemyRobot::ChangeStateAlert(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateAlert, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	rot1 = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(60.0f), AsoUtility::AXIS_Y));
	rot2 = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-60.0f), AsoUtility::AXIS_Y));
	Cnt = 0.0f;

	// ダンス(足踏み)アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::DANCE), true);

	MV1SetMaterialDifColor(viewRangeTransform_.modelId, 0, GetColorF(1.0f, 1.0f, 0.0f, 1.0f));
}

void EnemyRobot::ChangeStateChase(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateChase, this);

	// 移動スピード
	moveSpeed_ = 6.0f;

	float rand = GetRand(3.0f);
	changeShotCnt = ATTACK_SHOT_TIME + rand;

	anim_->Play(static_cast<int>(ANIM_TYPE::RUN),true);

	MV1SetMaterialDifColor(viewRangeTransform_.modelId, 0, GetColorF(1.0f, 0.0f, 0.0f, 1.0f));
}

void EnemyRobot::ChangeStateAttackKick(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateAttackKick, this);

	attackKickCnt = ATTACK_CT_TIME;

	IsCt = true;
}

void EnemyRobot::ChangeStateAttackShoot(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateAttackShoot, this);

	//遠距離攻撃用球
	attackShere.pos_ = { transform_.pos.x, 40.0f, transform_.pos.z };

	for (const auto& hitCol : hitColliders_)
	{
		// プレイヤーのカプセル以外は処理を飛ばす
		if (hitCol->GetTag() != ColliderBase::TAG::PLAYER) continue;
		//派生クラスへキャスト
		const ColliderCapsule* colliderCapsule =
			dynamic_cast<const ColliderCapsule*>(hitCol);
		VECTOR diff = VSub(colliderCapsule->GetFollow()->pos, transform_.pos);
		attackShere.moveDir_ = VNorm(diff);
	}
	attackShere.isAlive = true;

	changeShotCnt = 0.0f;

	anim_->Play(static_cast<int>(ANIM_TYPE::SHOOT), false);
}

void EnemyRobot::ChangeStateEscape(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateEscape, this);
}

void EnemyRobot::ChangeStateDead(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateDead, this);
}

void EnemyRobot::ChangeStateKnockBack(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateKnockBack, this);
}

void EnemyRobot::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateEnd, this);
}

void EnemyRobot::UpdateNone(void)
{
}

void EnemyRobot::UpdateThink(void)
{
}

void EnemyRobot::UpdateIdle(void)
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
		ChangeState(STATE::SURPRISE);
	}
}

void EnemyRobot::UpdatePatrol(void)
{
	// 巡回ポイントとの球体衝突判定(半径30.0fくらい)
	if(AsoUtility::IsHitSpheres(
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
		ChangeState(STATE::SURPRISE);
	}
}

void EnemyRobot::UpdateSurprise(void)
{
	uiSurprise_->Update();

	Cnt += 1.0f * scnMng_.GetDeltaTime();
	if (Cnt <= 10.0f)
	{
		ChangeState(STATE::ALERT);
	}
}

void EnemyRobot::UpdateAlert(void)
{
	Cnt += 1.0f * scnMng_.GetDeltaTime();

	if (Cnt <= 2.0f) {}
	else if (Cnt <= 6.0f) {
		transform_.quaRot = Quaternion::Slerp(transform_.quaRot, rot1, 0.5f);
	}
	else if (Cnt <= 10.0f) {
		transform_.quaRot = Quaternion::Slerp(transform_.quaRot, rot2, 0.5f);
	}
	else {
		ChangeState(STATE::IDLE);
		uiSurprise_->SetActive(false);
	}

	if (InSearchConeModel())
	{
		ChangeState(STATE::CHASE);
		uiSurprise_->SetActive(false);
	}
}

void EnemyRobot::UpdateChase(void)
{
	changeShotCnt -= scnMng_.GetDeltaTime();

	for (const auto& hitCol : hitColliders_)
	{
		// プレイヤーのカプセル以外は処理を飛ばす
		if (hitCol->GetTag() != ColliderBase::TAG::PLAYER) continue;
		//派生クラスへキャスト
		const ColliderCapsule* colliderCapsule =
			dynamic_cast<const ColliderCapsule*>(hitCol);

		VECTOR diff = VSub(colliderCapsule->GetFollow()->pos, transform_.pos);

		moveDir_ = VNorm(diff);

		if (VSize(diff) <= VIEW_ANGLE_ATTACK) {
			ChangeState(STATE::ATTACK_KICK);
		}
	}

	if (changeShotCnt <= 0.0f) {
		ChangeState(STATE::ATTACK_SHOOT);
	}

	// 移動量の計算
	movePow_ = VScale(moveDir_, moveSpeed_);

	if (!InSearchConeModel())
	{
		ChangeState(STATE::ALERT);
	}
}

void EnemyRobot::UpdateAttackKick(void)
{

	for (const auto& hitCol : hitColliders_)
	{
		// プレイヤーのカプセル以外は処理を飛ばす
		if (hitCol->GetTag() != ColliderBase::TAG::PLAYER) continue;
		//派生クラスへキャスト
		const ColliderCapsule* colliderCapsule =
			dynamic_cast<const ColliderCapsule*>(hitCol);
		VECTOR diff = VSub(colliderCapsule->GetFollow()->pos, transform_.pos);
		moveDir_ = VNorm(diff);

		if (VSize(diff) > VIEW_ANGLE_ATTACK
			&& anim_->IsEnd()) {
			ChangeState(STATE::CHASE);
		}
	}

	if (moveSpeed_ > 0.0f) {
		moveSpeed_--;
		// 移動量の計算
		movePow_ = VScale(moveDir_, moveSpeed_);
	}
	if (moveSpeed_ <= 0.0f) {
		movePow_ = AsoUtility::VECTOR_ZERO;
	}
	
	if (attackKickCnt > 0.0f && IsCt == true) {
		attackKickCnt -= scnMng_.GetDeltaTime();
		anim_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	}

	if (attackKickCnt <= 0.0f) {
		IsCt = false;
		anim_->Play(static_cast<int>(ANIM_TYPE::KICK), false);
		attackKickCnt = ATTACK_CT_TIME;
	}

	if (anim_->IsEnd()) {
		IsCt = true;
	}
}

void EnemyRobot::UpdateAttackShoot(void)
{
	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	changeShotCnt += scnMng_.GetDeltaTime();

	attackShere.pos_ = VAdd(attackShere.pos_, VScale(attackShere.moveDir_, ATTACK_SHOT_SPEED));

	if (changeShotCnt >= 3.0f) {
		ChangeState(STATE::CHASE);
	}
	else {
		anim_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	}

}

void EnemyRobot::UpdateEscape(void)
{
}

void EnemyRobot::UpdateDead(void)
{
}

void EnemyRobot::UpdateKnockBack(void)
{
}

void EnemyRobot::UpdateEnd(void)
{
}

void EnemyRobot::SetMoveDirPatrol(void)
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

bool EnemyRobot::InSearchConeModel(void)
{
	bool ret = false;

	// モデルライダ  
	int modelType = static_cast<int>(ColliderBase::TAG::VIEW_RANGE);

	if (ownColliders_.count(modelType) == 0)return ret;

	// モデルコライダ情報  
	ColliderModel* colliderModel =
		dynamic_cast<ColliderModel*>(ownColliders_.at(modelType));

	if (colliderModel == nullptr)return ret;

	MV1RefreshCollInfo(colliderModel->GetFollow()->modelId);

	for (const auto& hitCol : hitColliders_)
	{
		// プレイヤーのカプセル以外は処理を飛ばす
		if (hitCol->GetTag() != ColliderBase::TAG::PLAYER) continue;

		//派生クラスへキャスト
		const ColliderCapsule* colliderCapsule =
			dynamic_cast<const ColliderCapsule*>(hitCol);

		if (colliderCapsule == nullptr)continue;

		if (colliderCapsule->IsHit(colliderModel, false, false))
		{
			return true;
		}
	}
	return ret;
}
