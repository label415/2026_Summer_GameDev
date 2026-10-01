#include <DxLib.h>
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/SoundManager.h"
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
#include "../../Wepon/WeponBase.h"
#include "../../Wepon/WeponFlameThrower.h"
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
}

void EnemyDragon::Release(void)
{
	CharactorBase::Release();
	if (wepon_ != nullptr) {
		wepon_->Release();
		delete wepon_;
	}

	delete uiHp_;
}

void EnemyDragon::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();
	// モデルのロード
	transform_.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_ENEMY_DRAGON));

	uiHp_ = new UIHp(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y - UI_HP_OFFSET_Y,
		UI_HP_SCALE_X, UI_HP_SCALE_Y, UI_HP_SCALE_Z);
	uiHp_->Load();
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
	anim_->AddInFbx(type, ANIM_SPEED_IDLE, type);

	type = static_cast<int>(ANIM_TYPE::WALK);
	anim_->AddInFbx(type, ANIM_SPEED_WALK, type);

	type = static_cast<int>(ANIM_TYPE::CHARGE);
	anim_->AddInFbx(type, ANIM_SPEED_CHARGE, type);

	type = static_cast<int>(ANIM_TYPE::FLYING);
	anim_->AddInFbx(type, ANIM_SPEED_FLYING, type);

	type = static_cast<int>(ANIM_TYPE::BRACELET_ATTACK);
	anim_->AddInFbx(type, ANIM_SPEED_BRACELET_ATTACK, type);

	type = static_cast<int>(ANIM_TYPE::HOVER);
	anim_->AddInFbx(type, ANIM_SPEED_HOVER, type);

	type = static_cast<int>(ANIM_TYPE::TAKEOFF);
	anim_->AddInFbx(type, ANIM_SPEED_TAKEOFF, type);

	type = static_cast<int>(ANIM_TYPE::LANDS);
	anim_->AddInFbx(type, ANIM_SPEED_LANDS, type);

	type = static_cast<int>(ANIM_TYPE::DIE);
	anim_->AddInFbx(type, ANIM_SPEED_DIE, type);

	type = static_cast<int>(ANIM_TYPE::FLYING_ATTACK);
	anim_->AddInFbx(type, ANIM_SPEED_FLYING_ATTACK, type);

	type = static_cast<int>(ANIM_TYPE::ROAR);
	anim_->AddInFbx(type, ANIM_SPEED_ROAR, type);

	type = static_cast<int>(ANIM_TYPE::MELEE_ATTACK);
	anim_->AddInFbx(type, ANIM_SPEED_MELEE_ATTACK, type);

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
	stateChanges_.emplace(static_cast<int>(STATE::FALLING_ATTACK),
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

	effectType_ = EFFECT::NONE;

	effect_ = new EffectController();
	effect_->Add(
		static_cast<int>(EFFECT::FALLING_ATTACK),
		(Application::PATH_EFFECT + L"Fall.efkefc"));
	effect_->Add(
		static_cast<int>(EFFECT::ROAT),
		(Application::PATH_EFFECT + L"RoarEff.efkefc"));
	effect_->Add(
		static_cast<int>(EFFECT::CHARGE),
		(Application::PATH_EFFECT + L"Charge.efkefc"));
	effect_->Add(
		static_cast<int>(EFFECT::BLOOD),
		(Application::PATH_EFFECT + L"Blood.efkefc"));

	uiHp_->Init();

	// 初期状態設定
	ChangeState(STATE::ROAR);
}

void EnemyDragon::UpdateProcess(void)
{
	effect_->SetEffectPos(static_cast<int>(effectType_), transform_.pos);
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
					tFramePos.y += BODY_COL_OFFSET_Y;
					dFramePos.y += BODY_COL_OFFSET_Y;
				}
				else if (colliderCapsule->GetPatrTag() == static_cast<int>(PATR_TAG::NECK)
					|| colliderCapsule->GetPatrTag() == static_cast<int>(PATR_TAG::TAIL)) {
					colliderCapsule->SetRadius(NECK_TAIL_RADIUS);
					tFramePos.y += NECK_TAIL_COL_OFFSET_Y;
					dFramePos.y += NECK_TAIL_COL_OFFSET_Y;
				}

				colliderCapsule->SetLocalPosTop(tFramePos);
				colliderCapsule->SetLocalPosDown(dFramePos);
			}
			cnt++;
		}
	}

	SetFrameUserLocalPos(LOCK_POS, LOCK_FRAME_NO);

	if (wepon_ != nullptr) {
		if (!wepon_->GetIsAlive()) {
			delete wepon_;
			wepon_ = nullptr;
			return;
		}
		wepon_->Update();
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
		if (vec->GetTag() != ColliderBase::TAG::GROUND) continue;

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
	if (attribute_ == ATTRIBUTE::ABOVE_GROUND)
	{
		// 思考
		int rand = GetRand(STATE_RAND);
		if (rand < AI_PROB_TAKEOFF) {
			ChangeState(STATE::TAKEOFF);
			return;
		}

		if (diff < AI_DIST_NEAR) {
			rand = GetRand(STATE_RAND);
			if (rand < AI_PROB_MELEE_IN_NEAR)
			{
				ChangeState(STATE::MELEE_ATTACK);
				return;
			}
			else {
				ChangeState(STATE::CHARGE);
				return;
			}
		}
		else if (diff >= AI_DIST_NEAR && diff <= AI_DIST_FAR) {
			// 思考
			rand = GetRand(STATE_RAND);
			if (rand < AI_PROB_PATROL_IN_MID)
			{
				ChangeState(STATE::PATROL);
				return;
			}
			else if (rand >= AI_PROB_PATROL_IN_MID && rand < AI_PROB_CHARGE_IN_MID) {
				ChangeState(STATE::CHARGE);
				return;
			}
			else {
				ChangeState(STATE::BRACELET_ATTACK);
				return;
			}
		}
		else {
			ChangeState(STATE::PATROL);
			return;
		}
	}
	if (attribute_ == ATTRIBUTE::AIR)
	{
		// 思考
		int rand = GetRand(STATE_RAND);
		if (rand < AI_PROB_LANDS) {
			ChangeState(STATE::LANDS);
		}
		else {
			if (diff < AI_DIST_MID) {
				ChangeState(STATE::FALLING_ATTACK);
				return;
			}
			else if (diff >= AI_DIST_MID && diff <= AI_DIST_FAR) {
				ChangeState(STATE::FLYING_ATTACK);
				return;
			}
			else {
				ChangeState(STATE::FLYING);
				return;
			}
		}
	}
}

void EnemyDragon::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateIdle, this);
	// ランダムな待機時間 (1.0f ? 3.0f)
	step_ = 1.0f + static_cast<float>(GetRand(STATE_END_RAND));
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

	effectType_ = EFFECT::ROAT;
	effect_->Play(static_cast<int>(effectType_));
	effect_->SetEffectScl(static_cast<int>(effectType_), EFFECT_ROAR_SCALE);

	int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_ROAR).handleId_;
	SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_ROAR, bgm_, SE_VOLUME_DEFAULT);

	// 待機アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::ROAR), false);
}

void EnemyDragon::ChangeStateCharge(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateCharge, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// ランダムな待機時間 (2.0f ? 4.0f)
	step_ = 1.0f + static_cast<float>(GetRand(STATE_END_RAND));

	int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_ROAR).handleId_;
	SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_ROAR, bgm_, SE_VOLUME_DEFAULT);

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::ROAR), false);
}

void EnemyDragon::ChangeStatePatrol(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdatePatrol, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_WAKE).handleId_;
	SoundManager::GetInstance().PlayLoopSE(SoundManager::SeId::ENEMY_WAKE, bgm_, SE_VOLUME_PATROL);
	SoundManager::GetInstance().SetSESpeed(SoundManager::SeId::ENEMY_WAKE, SE_SPEED_PATROL);
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

	attackCnt_ = 0.0f;

	// アニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::FALLING_ATTACK), false);
}

void EnemyDragon::ChangeStateFlyingAttack(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateFlyingAttack, this);

	int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_ARE_BREASE_1).handleId_;
	SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_ARE_ENEMY_BREASE1, bgm_, SE_VOLUME_DEFAULT);

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::FLYING_ATTACK), false);

	anim_->SetStateTime(FLYING_ATTACK_ANIM_STATE_TIME);
}

void EnemyDragon::ChangeStateBreathAttack(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateBreathAttack, this);

	attackCnt_ = 0.0f;
	VECTOR dir = VNorm(VSub(*targetTrans_, transform_.pos));

	// 登録されている衝突物を全てチェック  
	for (const auto& hitCol : hitColliders_)
	{
		for (const auto& i : hitCol.second)
		{
			// モデル以外は処理を飛ばす  
			if (i->GetShape() != ColliderBase::SHAPE::MODEL
				&& i->GetTag() != ColliderBase::TAG::STAGE) continue;

			//派生クラスへキャスト
			const ColliderModel* colliderModel =
				dynamic_cast<const ColliderModel*>(i);

			wepon_ = new WeponBracelet(transform_, colliderModel, dir, FRAME_NO_MOUTH);
			wepon_->Init();
		}
	}

	int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_BREASE_1).handleId_;
	SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_BREASE1, bgm_, SE_VOLUME_DEFAULT);

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

	// ランダムな待機時間 (1.0f ? 3.0f)
	step_ = 1.0f + static_cast<float>(GetRand(STATE_END_RAND));

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 歩きアニメーション再生
	anim_->Play(
		static_cast<int>(ANIM_TYPE::HOVER), true);
}

void EnemyDragon::ChangeStateTakeOff(void)
{
	stateUpdate_ = std::bind(&EnemyDragon::UpdateTakeOff, this);

	int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_FLAP).handleId_;
	SoundManager::GetInstance().PlayLoopSE(SoundManager::SeId::ENEMY_ARE, bgm_, SE_VOLUME_TAKEOFF);

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
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}
}

void EnemyDragon::UpdateIdle(void)
{
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

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
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	effect_->SetEffectPos(static_cast<int>(effectType_), MV1GetFramePosition(transform_.modelId, FRAME_NO_MOUTH));
	effect_->Update(static_cast<int>(effectType_));

	if (anim_->IsEnd()) {
		// 待機終了
		attribute_ = ATTRIBUTE::ABOVE_GROUND;
		ChangeState(STATE::THINK);
		return;
	}
}

void EnemyDragon::UpdateCharge(void)
{
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	if (step_ < 0.0f)
	{
		effect_->Stop(static_cast<int>(effectType_));
		isAttack_ = false;
		ChangeState(STATE::IDLE);
		SoundManager::GetInstance().StopSE(SoundManager::SeId::ENEMY_WAKE);
		return;
	}

	if (anim_->GetPlayAnim().step >= CHARGE_SE_START_STEP
		&& anim_->GetPlayAnim().step <= CHARGE_SE_END_STEP) {
		int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_WAKE).handleId_;
		SoundManager::GetInstance().PlayLoopSE(SoundManager::SeId::ENEMY_WAKE, bgm_, SE_VOLUME_PATROL);
		SoundManager::GetInstance().SetSESpeed(SoundManager::SeId::ENEMY_WAKE, SE_SPEED_PATROL);
	}

	if (anim_->GetPlayAnim().step >= CHARGE_TRANS_STEP
		&& anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::ROAR))
	{
		anim_->Play(
			static_cast<int>(ANIM_TYPE::CHARGE), true);
		effectType_ = EFFECT::CHARGE;
		float yaw = atan2f(moveDir_.x, moveDir_.z);
		float pitch = -asinf(moveDir_.y);
		VECTOR euler = { pitch, yaw, 0.0f };
		euler = VAdd(euler, VGet(0.0f, 0.0f * DX_PI_F / 180.0f, 0.0f));
		effect_->Play(
			static_cast<int>(effectType_),
			MV1GetFramePosition(transform_.modelId, FRAME_NO_MOUTH),
			euler, EFFECT_CHARGE_SCALE);
	}
	else if (anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::CHARGE)) {
		moveDir_ = preMoverDir_;
		step_ -= scnMng_.GetDeltaTime();
		isAttack_ = true;
		moveSpeed_ = SPEED_DASH;
		movePow_ = VScale(moveDir_, moveSpeed_);

		effect_->SetEffectPos(static_cast<int>(effectType_), MV1GetFramePosition(transform_.modelId, FRAME_NO_MOUTH));
		effect_->Update(static_cast<int>(effectType_), true);
	}
}

void EnemyDragon::UpdatePatrol(void)
{
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	step_ -= scnMng_.GetDeltaTime();
	moveSpeed_ = SPEED_MOVE;
	movePow_ = VScale(moveDir_, moveSpeed_);

	// 思考
	int rand = GetRand(STATE_END_RAND);
	float diff = VSize(VSub(*targetTrans_, transform_.pos));
	if (diff <= ENEMY_ATTACK[rand])
	{
		ChangeState(STATE::IDLE);
		SoundManager::GetInstance().StopSE(SoundManager::SeId::ENEMY_WAKE);
		return;
	}
}

void EnemyDragon::UpdateFallingAttack(void)
{
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	moveDir_ = preMoverDir_;
	if (isJump_) {
		jumpPow_ = VAdd(jumpPow_, VScale(moveDir_, FALLING_JUMP_SPEED));
		isAttack_ = true;
		anim_->SetSpecificTime(FALLING_ANIM_LOOP_START, FALLING_ANIM_LOOP_END, true);

		int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_FALL).handleId_;
		SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_FALL, bgm_, SE_VOLUME_FALLING);
	}
	else {
		if (attribute_ == ATTRIBUTE::AIR) {
			effectType_ = EFFECT::FALLING_ATTACK;
			effect_->Play(static_cast<int>(effectType_));
			effect_->SetEffectScl(static_cast<int>(effectType_), EFFECT_FALLING_SCALE);
			effect_->SetEffectPos(static_cast<int>(effectType_), transform_.pos);
			effect_->Update(static_cast<int>(effectType_));
		}
		anim_->SetSpecificTime(0.0f, 0.0f, false);
		attribute_ = ATTRIBUTE::ABOVE_GROUND;
		SoundManager::GetInstance().StopSE(SoundManager::SeId::ENEMY_ARE);
	}

	if (anim_->IsEnd()) {
		ChangeState(STATE::IDLE);
		return;
	}
}

void EnemyDragon::UpdateFlying(void)
{
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	isJump_ = true;
	transform_.pos.y = MAX_TAKE;
	step_ -= scnMng_.GetDeltaTime();
	moveSpeed_ = SPEED_MOVE;
	movePow_ = VScale(moveDir_, moveSpeed_);

	// 思考
	int rand = GetRand(STATE_END_RAND);
	float diff = VSize(VSub(*targetTrans_, transform_.pos));
	if (diff <= ENEMY_ATTACK[rand])
	{
		ChangeState(STATE::HOVER);
		return;
	}
}

void EnemyDragon::UpdateFlyingAttack(void)
{
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	isJump_ = true;
	transform_.pos.y = MAX_TAKE;
	moveDir_ = preMoverDir_;

	if (anim_->GetPlayAnim().step == FLYING_ATTACK_FIRE_STEP)
	{
		VECTOR dir = VNorm(VSub(*targetTrans_, MV1GetFramePosition(transform_.modelId, FRAME_NO_MOUTH)));
		// 登録されている衝突物を全てチェック  
		for (const auto& hitCol : hitColliders_)
		{
			for (const auto& i : hitCol.second)
			{
				// モデル以外は処理を飛ばす  
				if (i->GetShape() != ColliderBase::SHAPE::MODEL
					&& i->GetTag() != ColliderBase::TAG::STAGE) continue;

				//派生クラスへキャスト
				const ColliderModel* colliderModel =
					dynamic_cast<const ColliderModel*>(i);

				wepon_ = new WeponFlameThrower(transform_, colliderModel, dir, FRAME_NO_MOUTH);
				wepon_->Init();
			}
		}
	}

	if (anim_->GetPlayAnim().step >= FLYING_ATTACK_END_STEP)
	{
		ChangeState(STATE::HOVER);
	}
}

void EnemyDragon::UpdateBreathAttack(void)
{
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	moveDir_ = preMoverDir_;

	if (anim_->GetPlayAnim().step >= BREATH_SE2_START_STEP
		&& anim_->GetPlayAnim().step <= BREATH_SE2_END_STEP)
	{
		SoundManager::GetInstance().StopSE(SoundManager::SeId::ENEMY_BREASE1);

		int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_BREASE_2).handleId_;
		SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_BREASE2, bgm_, SE_VOLUME_DEFAULT);
	}

	if (anim_->GetPlayAnim().step >= BREATH_ATTACK_START_STEP)
	{
		if (attackCnt_ <= BREATH_ATTACK_DURATION) {
			anim_->SetSpecificTime(BREATH_ANIM_LOOP_START, BREATH_ANIM_LOOP_END, true);
			attackCnt_ += 1.0f * SceneManager::GetInstance().GetDeltaTime();
			wepon_->SetIsAttack(true);
		}
		else {
			anim_->SetSpecificTime(0.0f, 0.0f, false);
		}
	}
	if (anim_->GetPlayAnim().step >= BREATH_WEAPON_END_STEP)
	{
		if (wepon_ != nullptr)
		{
			wepon_->SetIsEnd(true);
		}
	}

	if (anim_->GetPlayAnim().step >= BREATH_ATTACK_END_STEP)
	{
		ChangeState(STATE::IDLE);
		SoundManager::GetInstance().StopSE(SoundManager::SeId::ENEMY_BREASE2);
	}
}

void EnemyDragon::UpdateMeleeAttack(void)
{
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	if (anim_->GetPlayAnim().step >= MELEE_ATTACK_SE_START_STEP
		&& anim_->GetPlayAnim().step <= MELEE_ATTACK_CILLIDER) {
		int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_ATTCEK).handleId_;
		SoundManager::GetInstance().PlaySE(SoundManager::SeId::ENEMY_ATTCEK, bgm_, SE_VOLUME_DEFAULT);
	}

	if (anim_->GetPlayAnim().step >= MELEE_ATTACK_CILLIDER) {
		isAttack_ = true;
	}

	if (anim_->GetPlayAnim().step >= MELEE_MOVE_DIR_LOCK_STEP)
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
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

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
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	isJump_ = true;
	if (transform_.pos.y <= MAX_TAKE)
	{
		float jumpSpeed = TAKEOFF_SPEED * scnMng_.GetDeltaTime();
		jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_U, jumpSpeed));
	}
	else {
		attribute_ = ATTRIBUTE::AIR;
		ChangeState(STATE::HOVER);
	}
}

void EnemyDragon::UpdateLands(void)
{
	if (!uiHp_->IsActive()) {
		ChangeState(STATE::DEAD);
	}

	if (anim_->IsEnd())
	{
		attribute_ = ATTRIBUTE::ABOVE_GROUND;
		SoundManager::GetInstance().StopSE(SoundManager::SeId::ENEMY_ARE);
		ChangeState(STATE::IDLE);
	}
}

void EnemyDragon::UpdateDead(void)
{
	moveDir_ = preMoverDir_;

	if (anim_->IsEnd())
	{
		deathAnimationTime_ += DIE_FADE_SPEED * SceneManager::GetInstance().GetDeltaTime();
		if (deathAnimationTime_ > DIE_END_THRESHOLD) {
			deathAnimationTime_ = DIE_END_THRESHOLD;
			ChangeState(STATE::END);
		}
	}
}

void EnemyDragon::UpdateEnd(void)
{
	moveDir_ = preMoverDir_;
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

			if (colliderCapsule == nullptr) continue;

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

		if (colliderCapsule1 == nullptr
			|| colliderCapsule1->GetTag() != ColliderBase::TAG::ENEMY) continue;

		// 登録されている衝突物を全てチェック  
		for (const auto& hitCol : hitColliders_)
		{
			for (const auto& i : hitCol.second)
			{
				// モデル以外は処理を飛ばす  
				if (i->GetShape() != ColliderBase::SHAPE::CAPSULE
					|| i->GetTag() != ColliderBase::TAG::PLAYER_WEPON) continue;

				ColliderCapsule* colliderCapsule2 =
					dynamic_cast<ColliderCapsule*>(i);

				if (colliderCapsule2 == nullptr) continue;

				if (HitCheck_Capsule_Capsule(
					colliderCapsule1->GetPosTop(), colliderCapsule1->GetPosDown(), colliderCapsule1->GetRadius(),
					colliderCapsule2->GetPosTop(), colliderCapsule2->GetPosDown(), colliderCapsule2->GetRadius()))
				{
					if (!isInvincible_) {
						uiHp_->SetHp(DAMAGE_HIT_PLAYER_WEAPON);
						isInvincible_ = true;
						invincibleTimer_ = INVINCIBLE_TIME;
						effect_->Play(static_cast<int>(EFFECT::BLOOD));
						effect_->SetEffectScl(static_cast<int>(EFFECT::BLOOD), EFFECT_BLOOD_SCALE);

						VECTOR diff = VSub(colliderCapsule1->GetPosTop(), colliderCapsule1->GetPosDown());
						VECTOR center = VAdd(colliderCapsule1->GetPosDown(), VScale(diff, 0.5f));
						effect_->SetEffectPos(static_cast<int>(EFFECT::BLOOD), center);

						int bgm_ = resMng_.Load(ResourceManager::SRC::SE_ENEMY_HIT_DAMAGE).handleId_;
						SoundManager::GetInstance().PlaySE(SoundManager::SeId::PLAYER_WEPON_SE2, bgm_, SE_VOLUME_DEFAULT);

						return;
					}
					else {
					}
				}
			}
		}
	}
}

void EnemyDragon::DrawHp(void)
{
	if (uiHp_) { uiHp_->Draw(); }
}