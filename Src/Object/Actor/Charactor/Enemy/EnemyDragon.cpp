#include <DxLib.h>
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../Common/AnimationController.h"
#include "../../../Common/Collider/ColliderLine.h"
#include "../../../Common/Collider/ColliderCapsule.h"
#include "../../../Common/Collider/ColliderModel.h"
#include "../../../Common/Collider/ColliderSphere.h"
#include "../../../Common/Collider/ColliderBase.h"
#include "../../../../Libs/ImGui/imgui.h"
#include "../../../../Utility/ModelFrameUtility.h"
#include "../../../../Common/Quaternion.h"
#include "../../Wepon/WeponBracelet.h"
#include "../../UI/UIHp.h"
#include "../../../../Application.h"
#include "../../../Common/EffectController.h"
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

	if (wepon_ != nullptr) {
		wepon_->Draw();
	}

#ifdef _DEBUG

	VECTOR pos = MV1GetFramePosition(transform_.modelId, 1);

	DrawFormatString(0, 20, 0xffffff, L"モデルフレーム座標 %.1f, %.1f, %.1f", pos.x, pos.y, pos.z);

	std::wstring stateName[] = {
		L"NONE",L"THINK",L"IDLE",L"ROAR",L"PATROL",
		L"CHASE",L"FLYING",L"FLYING_ATTACK",L"BRACELET_ATTACK",
		L"MELEE_ATTACK",L"HOVER",L"TAKEOFF",L"LANDS",L"DEAD",L"END" };

	std::wstring attributeName[] = { L"NONE",L"ABOVE_GROUND",L"AIR" };

	DrawFormatString(0, 40, 0xffffff, L"Enemy State: %s", stateName[static_cast<int>(state_)].c_str());
	DrawFormatString(0, 60, 0xffffff, L"Enemy Attribute: %s", attributeName[static_cast<int>(attribute_)].c_str());


	std::wstring hit;
	if (isDamage_) {
		hit = L"当たっている";
	}
	else {
		hit = L"当たっていない";
	}
	DrawString(0, 100, hit.c_str(), 0xffffff);

	const auto& cols = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	int cnt = 0;
	for (const auto& col : cols) {
		if (col->GetTag() != ColliderBase::TAG::ENEMY) continue;
		ColliderCapsule* colliderCapsule = dynamic_cast<ColliderCapsule*>(col);
		if (colliderCapsule){
			if (cnt < std::size(ENEMY_CAPSULE_FRAMES)){
				if(isAttack_){
					colliderCapsule->SetValid(true);
				}
				else {
					colliderCapsule->SetValid(false);
				}
			}
			cnt++;
		}
	}

	if (targetTrans_ != nullptr)
	{
		float diff = VSize(VSub(*targetTrans_, transform_.pos));
		DrawFormatString(0, 120, 0xffffff, L"diff:%.1f", diff);
	}

#endif
}

void EnemyDragon::Release(void)
{
	CharactorBase::Release();
	delete uiHp_;
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
	//ライン
	std::vector<ColliderBase*> colLines;
	// 地面との衝突判定で使用するラインコライダー
	ColliderLine* groundLine = new ColliderLine(
		ColliderBase::TAG::GROUND, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	colLines.push_back(groundLine);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLines);
	colLines.clear();

	//カプセル
	std::vector<ColliderBase*> colCapsules;

	for (int i = 0; i < std::size(ENEMY_CAPSULE_FRAMES); i++) {
		ColliderCapsule* hitCapsule = new ColliderCapsule(
			ColliderBase::TAG::ENEMY, &colTransform_,
			AsoUtility::VECTOR_ZERO,
			AsoUtility::VECTOR_ZERO,
			HIT_RADIUS,
			static_cast<int>(ENEMY_CAPSULE_FRAMES[i].patrTag));
		colCapsules.push_back(hitCapsule);
	}

	// 地面との衝突判定で使用するカプセルコライダー
	ColliderCapsule* groundCapsule = new ColliderCapsule(
		ColliderBase::TAG::GROUND, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);
	colCapsules.push_back(groundCapsule);

	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsules);
	colCapsules.clear();
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

	type = static_cast<int>(ANIM_TYPE::CHARGE);
	anim_->AddInFbx(type, 50.0f, type);

	type = static_cast<int>(ANIM_TYPE::FLYING);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::BRACELET_ATTACK);
	anim_->AddInFbx(type, 20.0f, type);

	type = static_cast<int>(ANIM_TYPE::HOVER);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::TAKEOFF);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::LANDS);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::DIE);
	anim_->AddInFbx(type, 30.0f, type);

	type = static_cast<int>(ANIM_TYPE::FLYING_ATTACK);
	anim_->AddInFbx(type, 20.0f, type);

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
	stateChanges_.emplace(static_cast<int>(STATE::ROAR),
		std::bind(&EnemyDragon::ChangeStateRoar, this));
	stateChanges_.emplace(static_cast<int>(STATE::CHARGE),
		std::bind(&EnemyDragon::ChangeStateCharge, this));
	stateChanges_.emplace(static_cast<int>(STATE::FLYING),
		std::bind(&EnemyDragon::ChangeStateFlying, this));
	stateChanges_.emplace(static_cast<int>(STATE::FALLING＿ATTACK),
		std::bind(&EnemyDragon::ChangeStateFallingAttack, this));
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

	uiHp_ = new UIHp(100.0f, 600.0f, 1180.0f, 680.0f, 5.0f);

	effect_ = new EffectController();
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::BRACELET),
		(Application::PATH_EFFECT + L"Breath/Breath.efkefc"));

}

void EnemyDragon::UpdateProcess(void)
{
	preMoverDir_ = moveDir_;
	//ターゲットの方向更新
	moveDir_ = GetTargetDir();

	// 無敵タイマー更新
	if (isInvincible_) {
		invincibleTimer_ -= scnMng_.GetDeltaTime();
		if (invincibleTimer_ <= 0.0f) {
			isInvincible_ = false;
			invincibleTimer_ = 0.0f;
			
		}
	}
	
	// 状態別更新
	stateUpdate_();

	/*UpdateDebugImGui();*/

	const auto& cols = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	int cnt = 0; // 対象となるカプセルの個数を数えるカウンタ
	for (const auto& col : cols) {
		if (col->GetTag() != ColliderBase::TAG::ENEMY) continue;

		ColliderCapsule* colliderCapsule = dynamic_cast<ColliderCapsule*>(col);
		if (colliderCapsule)
		{
			// 配列の範囲外アクセス防止
			if (cnt < std::size(ENEMY_CAPSULE_FRAMES))
			{
				// 現在のカウントに対応するフレームIDのペアを取得
				int topFrame = ENEMY_CAPSULE_FRAMES[cnt].top;
				int downFrame = ENEMY_CAPSULE_FRAMES[cnt].down;

				VECTOR tFramePos = MV1GetFramePosition(transform_.modelId, topFrame);
				VECTOR dFramePos = MV1GetFramePosition(transform_.modelId, downFrame);

				if (colliderCapsule->GetPatrTag() == static_cast<int>(PATR_TAG::BODY)) {
					colliderCapsule->SetRadius(BODY_RADIUS);
					tFramePos.y -= 90.0f;
					dFramePos.y -= 90.0f;
				}
				else if (colliderCapsule->GetPatrTag() == static_cast<int>(PATR_TAG::NECK)
					|| colliderCapsule->GetPatrTag() == static_cast<int>(PATR_TAG::TAIL)) {
					colliderCapsule->SetRadius(80.0f);
					tFramePos.y -= 50.0f;
					dFramePos.y -= 50.0f;
				}

				colliderCapsule->SetLocalPosTop(tFramePos);
				colliderCapsule->SetLocalPosDown(dFramePos);
			}
			cnt++;
		}
	}

	SetFrameUserLocalPos(LOCK_POS, LOCK_FRAME_NO);

	if(wepon_ != nullptr &&!wepon_->GetIsAlive())
	{
		wepon_->ClearCollider();
		wepon_->Release();
		delete wepon_;
		wepon_ = nullptr;
	}
}

void EnemyDragon::UpdateDebugImGui(void)
{
	//// ウィンドウタイトル&開始処理
	//ImGui::Begin("EnemyDragon");
	//// 終了処理
	//ImGui::End();
}

void EnemyDragon::UpdateProcessPost(void)
{
	EnemyBase::UpdateProcessPost();
}

void EnemyDragon::CollisionCapsule(void)
{
	// カプセルコライダ  
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);

	// カプセルコライダが無ければ処理を抜ける  
	if (ownColliders_.count(capsuleType) == 0) return;

	const auto& vecs = ownColliders_.at(capsuleType);
	for (const auto& vec : vecs)
	{
		if (vec->GetTag() != ColliderBase::TAG::GROUND)continue;

		// カプセルコライダ情報  
		ColliderCapsule* colliderCapsule =
			dynamic_cast<ColliderCapsule*>(vec);

		if (colliderCapsule == nullptr) return;

		// 登録されている衝突物を全てチェック  
		for (const auto& hitCol : hitColliders_)
		{
			for (const auto& i : hitCol.second)
			{
				// モデル以外は処理を飛ばす  
				if (i->GetShape() != ColliderBase::SHAPE::MODEL) continue;

				const ColliderModel* colliderModel =
					dynamic_cast<const ColliderModel*>(i);

				if (colliderModel == nullptr) continue;

				colliderCapsule->PushBackAlongNormal(
					colliderModel,
					transform_,
					CNT_TRY_COLLISION,
					COLLISION_BACK_DIS,
					true,
					false
				);
			}
		}
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

	anim_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);

	float diff = VSize(VSub(*targetTrans_, transform_.pos));
	if(attribute_ == ATTRIBUTE::ABOVE_GROUND)
	{
		ChangeState(STATE::BRACELET_ATTACK);
		return;
		//// 思考
		//int rand = GetRand(100);
		//if(rand < 40){
		//	ChangeState(STATE::TAKEOFF);
		//	return;
		//}

		//if (diff < 800.0f) {
		//	rand = GetRand(100);
		//	if (rand < 40)
		//	{
		//		ChangeState(STATE::MELEE_ATTACK);
		//		return;
		//	}
		//	else {
		//		ChangeState(STATE::CHARGE);
		//		return;
		//	}
		//}
		//else if(diff >= 800.0f
		//	&&  diff <= 2000.0f){
		//	// 思考
		//	rand = GetRand(100);
		//	if (rand < 20)
		//	{
		//		ChangeState(STATE::PATROL);
		//		return;
		//	}
		//	else if (rand >= 20
		//		&& rand < 60) {
		//		ChangeState(STATE::CHARGE);
		//		return;
		//	}
		//	else {
		//		ChangeState(STATE::BRACELET_ATTACK);
		//		return;
		//	}
		//}
		//else {
		//	ChangeState(STATE::PATROL);
		//	return;
		//}

	}
	if (attribute_ == ATTRIBUTE::AIR) 
	{
		if (diff < 800.0f) {
			ChangeState(STATE::FALLING＿ATTACK);
			return;
		}
		else if (diff >= 800.0f
			&& diff <= 2000.0f) {
			ChangeState(STATE::FLYING_ATTACK);
			return;
		}
		else {
			ChangeState(STATE::FLYING);
			return;
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

void EnemyDragon::ChangeStateCharge(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateCharge, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// ランダムな待機時間
	step_ = 2.0f + static_cast<float>(GetRand(2));

	// 歩きアニメーション再生
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

void EnemyDragon::ChangeStateFlying(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateFlying, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::FLYING), true);
}

void EnemyDragon::ChangeStateFallingAttack(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateFallingAttack, this);

	// アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::FALLING＿ATTACK), false);

}

void EnemyDragon::ChangeStateFlyingAttack(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateFlyingAttack, this);

	wepon_ = new WeponBracelet(transform_, VNorm(VSub(*targetTrans_, transform_.pos)), 28);
	wepon_->Init();

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::FLYING_ATTACK), false);
	anim_->SetStateTime(90.0f);
}

void EnemyDragon::ChangeStateBreathAttack(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateBreathAttack, this);

	attackCnt_ = 0.0f;
	VECTOR dir = VNorm(VSub(*targetTrans_, transform_.pos));
	wepon_ = new WeponBracelet(transform_, dir, 28);
	wepon_->Init();
	effect_->Play(
		static_cast<int>(EFFECT_TYPE::BRACELET),
		MV1GetFramePosition(transform_.modelId, 28),
		dir, VGet(100.0f, 100.0f, 100.0f));

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

void EnemyDragon::UpdateCharge(void)
{
	if (anim_->GetPlayAnim().step >= 109.0f
		&& anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::ROAR))
	{
		anim_->Play(
			static_cast<int>(ANIM_TYPE::CHARGE), true);
	}
	else if(anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::CHARGE)){
		moveDir_ = preMoverDir_;
		step_ -= scnMng_.GetDeltaTime();
		isAttack_ = true;
		moveSpeed_ = SPEED_DASH;
		movePow_ = VScale(moveDir_, moveSpeed_);

		if (step_ < 0.0f)
		{
			isAttack_ = false;
			ChangeState(STATE::IDLE);
			return;
		}
	}
}

void EnemyDragon::UpdatePatrol(void)
{
	step_ -= scnMng_.GetDeltaTime();
	moveSpeed_ = SPEED_MOVE;
	movePow_ = VScale(moveDir_, moveSpeed_);

	// 思考
	int rand = GetRand(2);
	float diff = VSize(VSub(*targetTrans_, transform_.pos));
	if (diff <= ENEMY_ATTACK[rand])
	{
		ChangeState(STATE::IDLE);
		return;
	}
}

void EnemyDragon::UpdateFallingAttack(void)
{
	if (isJump_) {
		float jumpSpeed = 2000.0f * scnMng_.GetDeltaTime();
		jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_D, jumpSpeed));
		isAttack_ = true;
		anim_->SetSpecificTime(15.0f,20.0f, true);
	}
	else {
		anim_->SetSpecificTime(0.0f, 0.0f, false);
	}

	if(anim_->IsEnd()){
		attribute_ = ATTRIBUTE::ABOVE_GROUND;
		ChangeState(STATE::IDLE);
		return;
	}
}

void EnemyDragon::UpdateFlying(void)
{
	transform_.pos.y = MAX_TAKE;
	step_ -= scnMng_.GetDeltaTime();
	moveSpeed_ = SPEED_MOVE;
	movePow_ = VScale(moveDir_, moveSpeed_);

	// 思考
	int rand = GetRand(2);
	float diff = VSize(VSub(*targetTrans_, transform_.pos));
	if (diff <= ENEMY_ATTACK[rand])
	{
		ChangeState(STATE::HOVER);
		return;
	}
}

void EnemyDragon::UpdateFlyingAttack(void)
{
	transform_.pos.y = MAX_TAKE;
	moveDir_ = preMoverDir_;
	if (wepon_ != nullptr) {
		wepon_->Update();
	}

	if (anim_->GetPlayAnim().step >= 120.0f)
	{
		if (attackCnt_ <= 2.0f) {
			isAttack_ = true;
			anim_->SetSpecificTime(123.0f, 125.0f, true);
			attackCnt_ += 1.0f * SceneManager::GetInstance().GetDeltaTime();
			wepon_->SetIsAttack(true);
		}
		else {
			anim_->SetSpecificTime(0.0f, 0.0f, false);
		}
	}
	if (anim_->GetPlayAnim().step >= 127.0f)
	{
		if (wepon_ != nullptr)
		{
			wepon_->SetIsEnd(true);
		}
	}

	if (anim_->GetPlayAnim().step >= 180.0f)
	{
		ChangeState(STATE::HOVER);
	}
}

void EnemyDragon::UpdateBreathAttack(void)
{
	moveDir_ = preMoverDir_;
	if (wepon_ != nullptr) {
		wepon_->Update();
	}

	if(anim_->GetPlayAnim().step >= 27.0f)
	{
		if (attackCnt_ <= 2.0f) {
			isAttack_ = true;
			anim_->SetSpecificTime(27.0f, 30.0f, true);
			attackCnt_ += 1.0f * SceneManager::GetInstance().GetDeltaTime();
			wepon_->SetIsAttack(true);
			effect_->Update(static_cast<int>(EFFECT_TYPE::BRACELET));
		}
		else {
			anim_->SetSpecificTime(0.0f, 0.0f, false);
		}
	}
	if (anim_->GetPlayAnim().step >= 40.0f)
	{
		if(wepon_ != nullptr)
		{
			wepon_->SetIsEnd(true);
		}
	}

	if (anim_->GetPlayAnim().step >= 60.0f)
	{
		ChangeState(STATE::IDLE);
	}
}

void EnemyDragon::UpdateMeleeAttack(void)
{
	if (anim_->GetPlayAnim().step >= MELEE_ATTACK_CILLIDER) {
		isAttack_ = true;
	}

	if (anim_->GetPlayAnim().step >= 20.0f)
	{
		moveDir_ = preMoverDir_;
	}

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

void EnemyDragon::HitDamage(bool isHit)
{
	// カプセルコライダ  
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);

	// カプセルコライダが無ければ処理を抜ける  
	if (ownColliders_.count(capsuleType) == 0) return;

	const auto& vecs = ownColliders_.at(capsuleType);
	for (const auto& vec : vecs)
	{
		// カプセルコライダ情報  
		const ColliderCapsule* colliderCapsule1 =
			dynamic_cast<const ColliderCapsule*>(vec);

		if (colliderCapsule1 == nullptr) return;

		// 登録されている衝突物を全てチェック  
		for (const auto& hitCol : hitColliders_)
		{
			for (const auto& i : hitCol.second)
			{
				// モデル以外は処理を飛ばす  
				if (i->GetShape() != ColliderBase::SHAPE::CAPSULE) continue;

				if (i->GetTag() != ColliderBase::TAG::PLAYER_WEPON) continue;

				ColliderCapsule* colliderCapsule2 =
					dynamic_cast<ColliderCapsule*>(i);

				if (colliderCapsule2 == nullptr) continue;

				if (HitCheck_Capsule_Capsule(
					colliderCapsule1->GetPosTop(), colliderCapsule1->GetPosDown(), colliderCapsule1->GetRadius(),
					colliderCapsule2->GetPosTop(), colliderCapsule2->GetPosDown(), colliderCapsule2->GetRadius()))
				{
					if (!isInvincible_) {
						uiHp_->SetHp(10.0f);
						isInvincible_ = true;
						invincibleTimer_ = INVINCIBLE_TIME;
						return;
					}
				}
			}
		}
	}
}

void EnemyDragon::DrawHp(void)
{
	uiHp_->Draw();
}
