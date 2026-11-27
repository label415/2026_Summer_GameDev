#include <DxLib.h>
#include "../../../Manager/Common/ResourceManager.h"
#include "../../../Manager/Common/SceneManager.h"
#include "../../Camera.h"
#include "../../../Manager/Common/InputManager.h"
#include "../../../Manager/Common/SoundManager.h"
#include "../../../Manager/Common/EffectManager.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/CollisionUtility.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../../Application.h"
#include "../../../Common/Quaternion.h"
#include "../../Common/Collider/ColliderLine.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../../Common/Collider/ColliderSphere.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../Weapon/WeaponBlade.h"
#include "Enemy/EnemyDragon.h"
#include "Player.h"

Player::Player(void)
	: animLockPos_(AsoUtility::VECTOR_ZERO)
	, stRecoverTime_(0.0f)
	, effHandle_(-1)
	, deathAnimationTime_(0.0f)
	, invincibleTimer_(0.0f)
	, st_(MAX_ST)
	, recoveryBottleCount_(INITIAL_RECOVERY_BOTTLE_COUNT)
	, isComboNext_(false)
	, isInvincible_(false)
	, stateAtkCombo_(STATE_ATTACK_COMBO::COMBO_1)
	, state_(STATE::IDLE)
{
}

void Player::Draw(void)
{
	// 基底クラスの描画処理
	CharacterBase::Draw();

	// 武器描画
	for (const auto& weapon : weapons_) {
		weapon->Draw();
	}
}

void Player::Release(void)
{
	// 再生中エフェクトを停止してから基底クラスの解放処理を実行
	EffectManager::GetInstance().Stop(effHandle_);
	effHandle_ = -1;
	CharacterBase::Release();
}

void Player::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharacterBase::InitLoad();

	//プレイヤー
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::MODEL_PLAYER));

	// 武器
	auto weaponBlade = std::make_unique<WeaponBlade>(transform_, WEAPON_ATTACH_FRAME_NO);
	weapons_.push_back(std::move(weaponBlade));
	for (const auto& weapon : weapons_) {
		weapon->Load();
	}

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
}

void Player::InitTransform(void)
{
	// トランスフォームの初期化
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(ANGLE_AXIS_Y), AsoUtility::AXIS_Y));
	transform_.pos = INITIAL_POS;
	transform_.Update();
}

void Player::InitCollider(void)
{
	// 線分コライダ
	std::vector<ColliderBase::TAG> lineTags = { ColliderBase::TAG::STAGE };
	auto colLine = std::make_shared<ColliderLine>(
		ColliderBase::TAG::GROUND,
		lineTags,
		&transform_,
		COL_LINE_START_LOCAL_POS,
		COL_LINE_END_LOCAL_POS);

	// 所有コライダーへ登録
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::LINE)].push_back(colLine);

	// カプセルコライダ
	std::vector<ColliderBase::TAG> CapsuleTags = {
	ColliderBase::TAG::ENEMY,
	ColliderBase::TAG::ENEMY_WEAPON,
	ColliderBase::TAG::STAGE
	};

	auto colCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::PLAYER,
		CapsuleTags,
		&transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);

	// 所有コライダーへ登録
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::CAPSULE)].push_back(colCapsule);
}

void Player::InitAnimation(void)
{
	// 各行動用のアニメーションモデルを複製ロードして登録し、待機を再生
	animationController_->Add(static_cast<int>(ANIM_TYPE::IDLE),
		ANIM_SPEED_IDLE, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_IDLE));
	animationController_->Add(static_cast<int>(ANIM_TYPE::RUN),
		ANIM_SPEED_RUN, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_RUN));
	animationController_->Add(static_cast<int>(ANIM_TYPE::FAST_RUN),
		ANIM_SPEED_FAST_RUN, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_RUN));
	animationController_->Add(static_cast<int>(ANIM_TYPE::ATTACK_1),
		ANIM_SPEED_ATTACK_1, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_ATTACK_1));
	animationController_->Add(static_cast<int>(ANIM_TYPE::ATTACK_2),
		ANIM_SPEED_ATTACK_2, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_ATTACK_2));
	animationController_->Add(static_cast<int>(ANIM_TYPE::ATTACK_3),
		ANIM_SPEED_ATTACK_3, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_ATTACK_3));
	animationController_->Add(static_cast<int>(ANIM_TYPE::EVASION),
		ANIM_SPEED_EVASION, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_EVASION));
	animationController_->Add(static_cast<int>(ANIM_TYPE::DOWN),
		ANIM_SPEED_DOWN, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_DOWN));
	animationController_->Add(static_cast<int>(ANIM_TYPE::UP),
		ANIM_SPEED_UP, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_UP));
	animationController_->Add(static_cast<int>(ANIM_TYPE::RECOVERY),
		ANIM_SPEED_RECOVERY, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_RECOVERY));
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void Player::InitPost(void)
{
	// 移動方向
	moveDir_ = AsoUtility::DIR_F;
	// 移動スピード
	moveSpeed_ = 0.0f;
	// 移動量
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 状態
	state_ = STATE::IDLE;

	// 武器
	for (const auto& weapon : weapons_) {
		weapon->LoadEnd();
	}

	// クールタイム
	stRecoverTime_ = 0.0f;

	// 死亡アニメーション再生時間
	deathAnimationTime_ = 0.0f;

	// 無敵時間
	invincibleTimer_ = 0.0f;

	// エフェクトハンドル初期化
	effHandle_ = -1;

	// 攻撃コンボデータの初期化
	ATTACK_COMBO data;
	// 横切り攻撃
	data = {
		ANIM_TYPE::ATTACK_1,
		COMBO1_STEP_INPUT_START,
		COMBO1_STEP_INPUT_END,
		COMBO1_STEP_COL_START,
		COMBO1_STEP_COL_END,
		COMBO1_STEP_INTERRUPT,
		COMBO1_MOVE_SPEED,
		STATE_ATTACK_COMBO::COMBO_2,
		[this](void) { return false; },
		false,
		false,
		nullptr, nullptr, nullptr, nullptr,
	};
	atkComboData_.emplace(
		STATE_ATTACK_COMBO::COMBO_1, data);

	// 縦切り攻撃
	data = {
		ANIM_TYPE::ATTACK_2,
		COMBO2_STEP_INPUT_START,
		COMBO2_STEP_INPUT_END,
		COMBO2_STEP_COL_START,
		COMBO2_STEP_COL_END,
		COMBO2_STEP_INTERRUPT,
		COMBO2_MOVE_SPEED,
		STATE_ATTACK_COMBO::COMBO_3,
		[this](void) { return false; },
		false,
		false,
		nullptr, nullptr, nullptr, nullptr,
	};
	atkComboData_.emplace(
		STATE_ATTACK_COMBO::COMBO_2, data);

	// 回転攻撃
	data = {
		ANIM_TYPE::ATTACK_3,
		COMBO3_STEP_INPUT_START,
		COMBO3_STEP_INPUT_END,
		COMBO3_STEP_COL_START,
		COMBO3_STEP_COL_END,
		COMBO3_STEP_INTERRUPT,
		COMBO3_MOVE_SPEED,
		STATE_ATTACK_COMBO::MAX,
		[this]() { return false; },
		false,
		true,
		nullptr, nullptr, nullptr, nullptr,
	};
	atkComboData_.emplace(
		STATE_ATTACK_COMBO::COMBO_3, data);

	// 攻撃コンボ状態の初期化
	stateAtkCombo_ = STATE_ATTACK_COMBO::COMBO_1;
	// 攻撃コンボの次の攻撃を受け付けるかどうかのフラグを初期化
	isComboNext_ = false;
}

void Player::ProcessMove(void)
{
	// 移動量の初期化
	moveSpeed_ = 0.0f;
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 移動状態以外ではSEを停止
	if (state_ != STATE::IDLE
		&& state_ != STATE::RUN
		&& state_ != STATE::FAST_RUN) {
		SoundManager::GetInstance().StopSound(SoundManager::SOUND_ID::PLAYER_WALK);
		SoundManager::GetInstance().StopSound(SoundManager::SOUND_ID::PLAYER_RUN);
		return;
	}

	// 共通化した関数から方向（カメラ/ターゲット変換済み）を取得
	VECTOR inputDir = GetInputDirection();

	if (!AsoUtility::EqualsVZero(inputDir))
	{
		state_ = STATE::RUN;
		moveDir_ = inputDir;

		// ダッシュ入力判定
		auto& ins = InputManager::GetInstance();
		bool isR = ins.IsNew(KEY_INPUT_LSHIFT)
			|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

		// スタミナが回復中で、かつスタミナが残っている場合はダッシュ可能
		if (isR && stRecoverTime_ <= 0.0f) {
			moveSpeed_ = SPEED_DASH;
			state_ = STATE::FAST_RUN;
			ConsumeSt(CONSUMPTION_ST_FAST_RUN * SceneManager::GetInstance().GetDeltaTime());
			animationController_->Play(static_cast<int>(ANIM_TYPE::FAST_RUN));
			SoundManager::GetInstance().StopSound(SoundManager::SOUND_ID::PLAYER_WALK);
			SoundManager::GetInstance().PlaySoundId(SoundManager::SOUND_ID::PLAYER_RUN, true);
		}
		else {
			moveSpeed_ = SPEED_MOVE;
			animationController_->Play(static_cast<int>(ANIM_TYPE::RUN));
			SoundManager::GetInstance().StopSound(SoundManager::SOUND_ID::PLAYER_RUN);
			SoundManager::GetInstance().PlaySoundId(SoundManager::SOUND_ID::PLAYER_WALK, true);
		}

		movePow_ = VAdd(movePow_, VScale(moveDir_, moveSpeed_));
	}
	else {
		state_ = STATE::IDLE;
		SoundManager::GetInstance().StopSound(SoundManager::SOUND_ID::PLAYER_WALK);
		SoundManager::GetInstance().StopSound(SoundManager::SOUND_ID::PLAYER_RUN);
		animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE));
	}
}

void Player::ProcessAttack(void)
{
	// 攻撃入力判定
	auto& ins = InputManager::GetInstance();
	bool isAttackInput = false;

	// 攻撃入力判定
	if (st_ >= 0.0f) {
		isAttackInput = ins.IsClickMouseLeft()
			|| ins.IsPadBtnTrgDown(
				InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER);
	}

	// 通常状態からの新規攻撃開始
	if (isAttackInput && !isJump_
		&& (state_ == STATE::IDLE || state_ == STATE::RUN || state_ == STATE::FAST_RUN))
	{
		// 攻撃状態に遷移
		state_ = STATE::ATTACK;
		stateAtkCombo_ = STATE_ATTACK_COMBO::COMBO_1;

		// データの参照と初期設定
		auto& comboData = atkComboData_.at(stateAtkCombo_);
		comboData.isNextCombo = false;

		ConsumeSt(CONSUMPTION_ST_ATTACK);

		// 追加の初期処理があれば実行
		if (comboData.extraInit) {
			comboData.extraInit();
		}

		// 攻撃アニメーション再生
		animationController_->Play(static_cast<int>(comboData.animType), false);
	}

	// 攻撃状態でなければ処理終了
	if (state_ != STATE::ATTACK) return;

	// 現在のコンボデータを取得
	auto& comboData = atkComboData_.at(stateAtkCombo_);
	float currentStep = animationController_->GetPlayAnim().step;

	if (comboData.moveSpeed > 0.0f && currentStep < comboData.stepCollisionStart)
	{
		// 入力方向を取得
		VECTOR inputDir = GetInputDirection();

		// 入力方向がゼロベクトルでない場合のみ処理
		if (!AsoUtility::EqualsVZero(inputDir))
		{
			// 入力方向へ回転
			float targetAngleY = atan2f(inputDir.x, inputDir.z);
			Quaternion targetRot = Quaternion::AngleAxis(targetAngleY, AsoUtility::AXIS_Y);
			transform_.quaRot = targetRot;

			// 向いた正面方向へ向かって踏み込み移動
			VECTOR forward = transform_.GetForward();
			VECTOR targetPos = VAdd(transform_.pos, VScale(forward, comboData.moveSpeed));

			transform_.pos = AsoUtility::Lerp(transform_.pos, targetPos, ATTACK_STEP_LERP_RATE);
		}
	}

	// 追加更新処理
	if (comboData.extraUpdate) {
		comboData.extraUpdate();
	}

	// コンボ入力受付判定
	if (comboData.IsValidCombo(currentStep)) {
		// 通常の攻撃入力、または固有の条件（actionNextCombo）を満たした場合
		bool isCustomAction = comboData.actionNextCombo ? comboData.actionNextCombo() : false;
		if (isAttackInput || isCustomAction) {
			comboData.isNextCombo = true;
		}
	}

	// 衝突判定の処理
	if (comboData.IsValidCollision(currentStep)) {
		for (const auto& weapon : weapons_) {
			weapon->SetCollider();
		}

	}
	else {
		for (const auto& weapon : weapons_) {
			weapon->ClearCollider();
		}
	}

	// SE再生処理
	if (currentStep == ATTACK_SE_TRIGGER_STEP) {
		SoundManager::GetInstance().PlaySoundId(SoundManager::SOUND_ID::PLAYER_WEAPON_SE1);
	}

	// コンボ遷移チェック
	bool canInterrupt = comboData.IsValidInterrupt(currentStep);
	bool isAnimEnd = animationController_->IsEnd();

	// 追加割込条件のチェック
	if (comboData.isExtraInterrupt && comboData.isExtraInterrupt(comboData)) {
		canInterrupt = true;
	}

	// 次のコンボへ繋ぐ場合、またはアニメーションが終了した場合に処理
	if ((canInterrupt && comboData.isNextCombo) || isAnimEnd) {
		// 次のコンボへ繋ぐ場合
		if (comboData.isNextCombo && comboData.nextCombo != STATE_ATTACK_COMBO::MAX) {
			// 次のコンボへ状態遷移
			stateAtkCombo_ = comboData.nextCombo;

			for (const auto& weapon : weapons_) {
				weapon->ClearCollider();
			}

			// 新しいコンボデータのセットアップ
			auto& nextData = atkComboData_.at(stateAtkCombo_);
			nextData.isNextCombo = false;

			ConsumeSt(CONSUMPTION_ST_ATTACK);

			if (nextData.extraInit) {
				nextData.extraInit();
			}

			animationController_->Play(static_cast<int>(nextData.animType), false);
		}
		// アニメーションが終了した場合、または追加終了条件を満たした場合
		else if (isAnimEnd || (comboData.isExtraEnd && comboData.isExtraEnd()))
		{
			state_ = STATE::IDLE;
			stateAtkCombo_ = STATE_ATTACK_COMBO::COMBO_1;
			comboData.isNextCombo = false;

			for (const auto& weapon : weapons_) {
				weapon->ClearCollider();
			}
		}
	}
}

void Player::ProcessEvasion(void)
{
	// 回避入力判定とスタミナ残量確認
	bool isP = false;
	auto& ins = InputManager::GetInstance();

	if (st_ >= 0.0f) {
		isP = ins.IsTrgDown(KEY_INPUT_SPACE)
			|| ins.IsPadBtnNew(
				InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);
	}

	// 地上通常状態から回避へ遷移し、無敵と回転演出・SEを設定
	if (isP && !isJump_
		&& (state_ == STATE::IDLE || state_ == STATE::RUN || state_ == STATE::FAST_RUN))
	{
		state_ = STATE::EVASION;
		lastQrot_ = transform_.quaRotLocal;
		transform_.quaRotLocal =
			Quaternion::Mult(transform_.quaRotLocal,
				Quaternion::AngleAxis(
					AsoUtility::Deg2RadF(AVOIDANCE_ANGLE_AXIS_Y), AsoUtility::AXIS_Y));

		ConsumeSt(CONSUMPTION_ST_EVASION);
		SoundManager::GetInstance().PlaySoundId(SoundManager::SOUND_ID::PLAYER_EVASION);

		isInvincible_ = true;
	}

	// 回避状態でなければ以降の移動・終了判定を行わない
	if (state_ != STATE::EVASION) return;

	// 回避アニメーションと回避速度を適用
	animationController_->Play(static_cast<int>(ANIM_TYPE::EVASION), false);
	moveSpeed_ = SPEED_EVASION;
	movePow_ = VScale(moveDir_, moveSpeed_);

	// アニメーション完了でローカル回転・無敵を戻して待機へ復帰
	if (animationController_->IsEnd())
	{
		transform_.quaRotLocal = lastQrot_;
		state_ = STATE::IDLE;
		isInvincible_ = false;
	}
}

void Player::ProcessDownUp(void)
{
	// ダウン状態の処理
	if (state_ == STATE::DOWN)
	{
		isInvincible_ = true;
		state_ = STATE::DOWN;

		EffectManager::GetInstance().SetPos(
			effHandle_,
			MV1GetFramePosition(transform_.modelId, HIT_EFFECT_FRAME_NO));

		if (animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::DOWN)
			&& animationController_->IsEnd()) {

			if (IsDead()) {
				state_ = STATE::DIE;
			}
			else {
				animationController_->Play(
					static_cast<int>(ANIM_TYPE::UP), false);
				invincibleTimer_ = INVINCIBLE_TIME;
				state_ = STATE::UP;
			}
		}
	}

	// アップ状態の処理
	if (state_ == STATE::UP)
	{
		state_ = STATE::UP;
		if (animationController_->IsEnd()) {
			state_ = STATE::IDLE;
		}
	}
}

void Player::ProcessRecovery(void)
{
	// 回復入力判定
	bool isP = false;
	auto& ins = InputManager::GetInstance();
	if (state_ != STATE::IDLE
		&& state_ != STATE::RUN
		&& state_ != STATE::FAST_RUN
		&& state_ != STATE::RECOVERY) {
		return;
	}

	// 回復入力判定
	isP = ins.IsTrgDown(KEY_INPUT_R)
		|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT);

	// 回復入力があり、かつ回復瓶が残っている場合、かつ回復アニメーションが再生されていない場合に回復開始
	if (isP && recoveryBottleCount_ > 0
		&& animationController_->GetPlayType() != static_cast<int>(ANIM_TYPE::RECOVERY)
		&& state_ != STATE::RECOVERY) {

		state_ = STATE::RECOVERY;
		EffectManager::GetInstance().PlayEffect(
			EffectManager::EFFECT_ID::HP_ABSOLUTE,
			VGet(transform_.pos.x, transform_.pos.y 
				+ EFFECT_RECOVERY_OFFSET_Y, transform_.pos.z),
			AsoUtility::VECTOR_ZERO,
			VScale(AsoUtility::VECTOR_ONE, EFFECT_RECOVERY_SCALE));
	}

	// 回復状態でなければ処理終了
	if (state_ != STATE::RECOVERY) return;

	// 回復アニメーション再生
	animationController_->Play(static_cast<int>(ANIM_TYPE::RECOVERY), false);

	// 回復アニメーションの特定のステップで回復処理を実行
	if (animationController_->GetPlayAnim().step == RECOVERY_TRIGGER_STEP) {
		HealHp(HEAL_HP_AMOUNT);
		if (recoveryBottleCount_ > 0)
		{
			recoveryBottleCount_ -= CONSUME_RECOVERY_BOTTLE_COUNT;
		}
		SoundManager::GetInstance().PlaySoundId(SoundManager::SOUND_ID::PLAYER_HEAL);
	}

	// 回復アニメーションが終了したら通常状態に遷移
	if (animationController_->IsEnd()) {
		state_ = STATE::IDLE;
		SoundManager::GetInstance().StopSound(SoundManager::SOUND_ID::PLAYER_HEAL);
	}
}

void Player::ProcessDie(void)
{
	// 死亡アニメーション再生
	deathAnimationTime_ += DIE_FADE_SPEED 
		* SceneManager::GetInstance().GetDeltaTime();
	if (deathAnimationTime_ > DIE_END_THRESHOLD) {
		deathAnimationTime_ = DIE_END_THRESHOLD;
		state_ = STATE::END;
	}
}

void Player::ApplyDamage(float damageAmount)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
	state_ = STATE::DOWN;
	TakeDamage(damageAmount);

	effHandle_ = EffectManager::GetInstance().PlayEffect(
		EffectManager::EFFECT_ID::BLOOD,
		transform_.pos,
		AsoUtility::VECTOR_ZERO,
		EFFECT_BLOOD_SCALE);

	SoundManager::GetInstance().PlaySoundId(SoundManager::SOUND_ID::PLAYER_DAMAGE);
}

void Player::UpdateProcess(void)
{
	// 死亡状態の処理
	if (state_ == STATE::DIE)
	{
		ProcessDie();
		return;
	}

	// エフェクトの位置をプレイヤーの位置に設定
	EffectManager::GetInstance().SetPos(effHandle_, transform_.pos);

	// 無敵時間の処理
	if (isInvincible_ && invincibleTimer_ >= 0.0f)
	{
		invincibleTimer_ -= 1.0f * SceneManager::GetInstance().GetDeltaTime();
	}
	else {
		// 無敵時間が終了したら無敵状態を解除
		isInvincible_ = false;
	}

	// スタミナ回復処理
	if (st_ <= MIN_ST) {
		stRecoverTime_ = CT;
	}

	// スタミナ回復処理
	if (state_ == STATE::IDLE
		|| state_ == STATE::RUN
		|| state_ == STATE::DOWN
		|| state_ == STATE::UP
		|| state_ == STATE::RECOVERY) {
		RecoverSt(RECOVERY_ST_SPEED 
			* SceneManager::GetInstance().GetDeltaTime());
	}

	// スタミナ回復クールタイムの処理
	if (stRecoverTime_ > 0.0f) {
		stRecoverTime_ -= 1.0f * SceneManager::GetInstance().GetDeltaTime();
	}

	// 武器のコライダーを攻撃中以外では無効化
	if (state_ != STATE::ATTACK)
	{
		for (const auto& weapon : weapons_)
		{
			weapon->ClearCollider();
		}
	}

	// 回復処理
	ProcessRecovery();

	//攻撃処理
	ProcessAttack();

	// 移動操作
	ProcessMove();

	//回避処理
	ProcessEvasion();

	// ダウン・アップ処理
	ProcessDownUp();

	// 武器処理
	for (const auto& weapon : weapons_)
	{
		weapon->Update();
	}

	// コライダーの位置をアニメーションに合わせて変更
	if (STATE::DOWN == state_)
	{
		animLockPos_ = LOCK_POS2;
	}
	else if (STATE::DIE == state_) {
		animLockPos_ = LOCK_POS4;
	}
	else if (STATE::UP == state_)
	{
		animLockPos_.y += UP_LOCK_POS_Y_ADD;
	}
	else
	{
		animLockPos_ = LOCK_POS1;
	}

	//アニメーションの移動量を無効
	SetFrameUserLocalPos(animLockPos_, LOCK_FRAME_NO);
}

void Player::UpdateProcessPost(void)
{
	// ロックオン対象が未設定なら回転追従を行わない
	if (targetPos_ == nullptr)return;

	// ターゲットに向かって回転
	VECTOR targetDir = GetTargetDir();
	float targetAngleY = atan2f(targetDir.x, targetDir.z);
	Quaternion targetRot =
		Quaternion::AngleAxis(targetAngleY, AsoUtility::AXIS_Y);
	transform_.quaRot =
		Quaternion::Slerp(transform_.quaRot, targetRot, ROT_TARGET_SLERP_RATE);
}

void Player::UpdateHitCollider(void)
{
	// 無敵時間中は当たり判定による被ダメージ処理を行わない
	if (isInvincible_) return;

	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	if (ownColliders_.count(capsuleType) == 0) return;

	// 受けるダメージ量を保持する変数
	float damage = 0.0f;

	for (const auto& vec : ownColliders_.at(capsuleType))
	{
		auto playerCapsule = std::dynamic_pointer_cast<ColliderCapsule>(vec);
		if (!playerCapsule || !playerCapsule->GetIsCollider()) continue;

		const auto& hitList = playerCapsule->GetHitColliders();

		for (const auto& hitCol : hitList)
		{
			auto targetCol = hitCol.lock();
			if (!targetCol || !targetCol->GetIsCollider()) continue;

			auto shape = targetCol->GetShape();
			auto tag = targetCol->GetTag();

			// 敵本体との当たり判定
			if (tag == ColliderBase::TAG::ENEMY
				&& shape == ColliderBase::SHAPE::CAPSULE)
			{
				auto enemyCapsule = std::dynamic_pointer_cast<ColliderCapsule>(targetCol);

				transform_.pos =
					VAdd(transform_.pos,
						CollisionUtility::CalcPushCapsuleCapsule(
							*playerCapsule, *enemyCapsule));

				if (!enemyCapsule->GetIsAction())continue;

				// 複数同時に当たった場合は大きい方のダメージを優先
				damage = DAMAGE_ENEMY_BODY;
				continue;
			}

			// タグが敵武器でないときは処理しない
			if (tag != ColliderBase::TAG::ENEMY_WEAPON) continue;

			// 敵武器との当たり判定
			if (shape == ColliderBase::SHAPE::CAPSULE
				|| shape == ColliderBase::SHAPE::SPHERE)
			{
				damage = DAMAGE_ENEMY_WEAPON;
			}
		}
	}

	// ダメージが発生していればループの外で1回だけ適用
	if (damage > 0.0f)
	{
		ApplyDamage(damage);
	}

}

VECTOR Player::GetInputDirection(void)
{
	VECTOR dir = AsoUtility::VECTOR_ZERO;
	auto& ins = InputManager::GetInstance();

	// ゲームパッドの入力を取得
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
	VECTOR padDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	// 両対応：ゲームパッド優先、無ければキーボード
	if (!AsoUtility::EqualsVZero(padDir))
	{
		dir = padDir;
	}
	else
	{
		// キーボード入力（WASD）
		bool isUp = ins.IsNew(KEY_INPUT_W);
		bool isDown = ins.IsNew(KEY_INPUT_S);
		bool isLeft = ins.IsNew(KEY_INPUT_A);
		bool isRight = ins.IsNew(KEY_INPUT_D);

		if (isUp) { dir = AsoUtility::DIR_F; }
		if (isLeft) { dir = AsoUtility::DIR_L; }
		if (isDown) { dir = AsoUtility::DIR_B; }
		if (isRight) { dir = AsoUtility::DIR_R; }

		if (isUp && isLeft) { dir = VAdd(AsoUtility::DIR_F, AsoUtility::DIR_L); }
		if (isUp && isRight) { dir = VAdd(AsoUtility::DIR_F, AsoUtility::DIR_R); }
		if (isDown && isLeft) { dir = VAdd(AsoUtility::DIR_B, AsoUtility::DIR_L); }
		if (isDown && isRight) { dir = VAdd(AsoUtility::DIR_B, AsoUtility::DIR_R); }

		// 斜め入力の正規化
		if (!AsoUtility::EqualsVZero(dir)) {
			dir = VNorm(dir);
		}
	}

	// 入力がない場合はゼロベクトルを返す
	if (AsoUtility::EqualsVZero(dir))
	{
		return AsoUtility::VECTOR_ZERO;
	}

	// ターゲットまたはカメラ基準のワールド方向に変換
	VECTOR worldDir = AsoUtility::VECTOR_ZERO;

	if (targetPos_ != nullptr)
	{
		// ターゲットがいる場合
		VECTOR targetDir = GetTargetDir();
		float targetAngleY = atan2f(targetDir.x, targetDir.z);
		Quaternion targetRot = Quaternion::AngleAxis(targetAngleY, AsoUtility::AXIS_Y);
		worldDir = Quaternion::PosAxis(targetRot, dir);
	}
	else
	{
		// 通常時：カメラ方向を基準に変換
		Quaternion cameraRot = scnMng_.GetCamera()->GetQuaRotY();
		worldDir = Quaternion::PosAxis(cameraRot, dir);
	}
	return worldDir;
}

float Player::GetStRate(void) const
{
	// 現在のスタミナ割合を 0.0f〜1.0f で返す（負値は描画上0として扱う）
	if (MAX_ST <= 0.0f || st_ <= 0.0f)
	{
		return 0.0f;
	}
	return st_ / MAX_ST;
}

void Player::ConsumeSt(float delta)
{
	// スタミナを消費し、下限値でクランプする
	st_ -= delta;
	if (st_ < MIN_ST)
	{
		st_ = MIN_ST;
	}
}

void Player::RecoverSt(float amount)
{
	// スタミナを回復し、上限値でクランプする
	st_ += amount;
	if (st_ > MAX_ST)
	{
		st_ = MAX_ST;
	}
}