#include <DxLib.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/CollisionManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SoundManager.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../../Application.h"
#include "../../../Common/Quaternion.h"
#include "../../Common/Collider/ColliderLine.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../../Common/Collider/ColliderSphere.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../Wepon/WeponBlade.h"
#include "../UI/UISt.h"
#include "../UI/UIHp.h"
#include "../UI/UIRecovery.h"
#include "Enemy/EnemyDragon.h"
#include "Player.h"

Player::Player(void)
	: animLockPos_(AsoUtility::VECTOR_ZERO)
	, stRecoverTime_(0.0f)
	, effType_(EFFECT::NONE)
	, deathAnimationTime_(0.0f)
	, invincibleTimer_(0.0f)
	, isComboNext_(false)
	, isVinclible_(false)
	, stateAtkCombo_(STATE_ATTACK_COMBO::COMBO_1)
	, state_(STATE::IDLE)
	, uiRecovery_(nullptr)
	, wepon_(nullptr)
	, uiSt_(nullptr)
{
}

Player::~Player(void) {}

void Player::Draw(void)
{
	// 基底クラスの描画処理
	CharactorBase::Draw();

	// 武器描画
	if (wepon_) {
		wepon_->Draw();
	}
}

void Player::Release(void)
{
	// 基底クラスの解放処理
	CharactorBase::Release();

	// 武器の解放処理
	wepon_->Release();

	// 各ポインタ変数開放
	delete wepon_;
	delete uiHp_;
	delete uiRecovery_;
	if (uiSt_) {
		delete uiSt_;
		uiSt_ = nullptr;
	}
}

void Player::DrawHp(void)
{
	// HPとスタミナのUI描画
	if (uiHp_) { uiHp_->Draw(); }
	if (uiSt_) { uiSt_->Draw(); }

	// 回復瓶のUI描画
	if (uiRecovery_) { uiRecovery_->Draw(); }
}

void Player::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();

	//プレイヤー
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::MODEL_PLAYER));

	// 武器
	wepon_ = new WeponBlade(transform_, WEAPON_ATTACH_FRAME_NO);
	wepon_->Load();

	// 回復瓶UI
	uiRecovery_ = new UIRecovery(INITIAL_RECOVERY_BOTTLE_COUNT);
	uiRecovery_->Load();

	// HP UI を画面左上に表示
	uiHp_ = new UIHp(
		Application::SCREEN_SIZE_X / UI_HP_POS_X_DIV, UI_HP_POS_Y,
		UI_SCALE_X, UI_SCALE_Y, UI_SCALE_Z);
	uiHp_->Load();

	// スタミナUIを HP の下に表示
	uiSt_ = new UISt(
		Application::SCREEN_SIZE_X / UI_HP_POS_X_DIV, UI_ST_POS_Y,
		UI_SCALE_X, UI_SCALE_Y, UI_SCALE_Z);
	uiSt_->Load();
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
		ColliderBase::TAG::PLAYER,
		lineTags,
		&transform_,
		COL_LINE_START_LOCAL_POS,
		COL_LINE_END_LOCAL_POS);

	// マネージャーへ登録
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::LINE)].push_back(colLine);
	CollisionManager::GetInstance().AddCollider(colLine);

	// カプセルコライダ
	std::vector<ColliderBase::TAG> CapsuleTags = {
	ColliderBase::TAG::ENEMY,
	ColliderBase::TAG::ENEMY_WEPON,
	ColliderBase::TAG::STAGE
	};

	auto colCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::PLAYER,
		CapsuleTags,
		&transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);

	// マネージャーへ登録
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::CAPSULE)].push_back(colCapsule);
	CollisionManager::GetInstance().AddCollider(colCapsule);
}

void Player::InitAnimation(void)
{
	anim_ = new AnimationController(transform_.modelId);

	anim_->Add(static_cast<int>(ANIM_TYPE::IDLE),
		ANIM_SPEED_IDLE, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_IDLE));
	anim_->Add(static_cast<int>(ANIM_TYPE::RUN),
		ANIM_SPEED_RUN, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_RUN));
	anim_->Add(static_cast<int>(ANIM_TYPE::FAST_RUN),
		ANIM_SPEED_FAST_RUN, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_RUN));
	anim_->Add(static_cast<int>(ANIM_TYPE::ATTACK_1),
		ANIM_SPEED_ATTACK_1, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLSYER_ATTACK_1));
	anim_->Add(static_cast<int>(ANIM_TYPE::ATTACK_2),
		ANIM_SPEED_ATTACK_2, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLSYER_ATTACK_2));
	anim_->Add(static_cast<int>(ANIM_TYPE::ATTACK_3),
		ANIM_SPEED_ATTACK_3, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLSYER_ATTACK_3));
	anim_->Add(static_cast<int>(ANIM_TYPE::EVASION),
		ANIM_SPEED_EVASION, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_EVASION));
	anim_->Add(static_cast<int>(ANIM_TYPE::DOWN),
		ANIM_SPEED_DOWN, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_DOWN));
	anim_->Add(static_cast<int>(ANIM_TYPE::UP),
		ANIM_SPEED_UP, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_UP));
	anim_->Add(static_cast<int>(ANIM_TYPE::RECOVERY),
		ANIM_SPEED_RECOVERY, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_RECOVERY));
	anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
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
	wepon_->Init();

	// HPUI
	uiHp_->Init();

	// スタミナUI
	uiSt_->Init();

	// クールタイム
	stRecoverTime_ = 0.0f;

	// 死亡アニメーション再生時間
	deathAnimationTime_ = 0.0f;

	// 無敵時間
	invincibleTimer_ = 0.0f;

	// 回復瓶UI
	uiRecovery_->Init();

	// エフェクト
	effType_ = EFFECT::NONE;
	effect_ = std::make_unique<EffectController>();
	effect_->Add(
		static_cast<int>(EFFECT::BLOOD),
		(Application::PATH_EFFECT + L"Blood.efkefc"));
	effect_->Add(
		static_cast<int>(EFFECT::HP_ABSOLUTE),
		(Application::PATH_EFFECT + L"Absolute.efkefc"));

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
		SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_WAKE);
		SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_RAN);
		return;
	}

	// 共通化した関数から方向（カメラ/ターゲット変換済み）を取得
	VECTOR inputDir = GetInputDirection();

	if (!AsoUtility::EqualsVZero(inputDir))
	{
		state_ = STATE::RUN;
		moveDir_ = inputDir; // 最終移動方向を保持

		// ターゲットに向かって回転（ターゲット時のみ）
		if (targetTrans_ != nullptr)
		{
			VECTOR targetDir = GetTargetDir();
			float targetAngleY = atan2f(targetDir.x, targetDir.z);
			Quaternion targetRot = Quaternion::AngleAxis(targetAngleY, AsoUtility::AXIS_Y);
			transform_.quaRot = Quaternion::Slerp(transform_.quaRot, targetRot, ROT_TARGET_SLERP_RATE);
		}

		// ダッシュ入力判定
		auto& ins = InputManager::GetInstance();
		bool isR = ins.IsNew(KEY_INPUT_LSHIFT)
			|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

		// スタミナが回復中で、かつスタミナが残っている場合はダッシュ可能
		if (isR && stRecoverTime_ <= 0.0f) {
			moveSpeed_ = SPEED_DASH;
			state_ = STATE::FAST_RUN;
			uiSt_->SetSt(CONSUMPTION_ST_FAST_RUN * SceneManager::GetInstance().GetDeltaTime());
			anim_->Play(static_cast<int>(ANIM_TYPE::FAST_RUN));
			SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_WAKE);
			int bgm_ = resMng_.Load(ResourceManager::SRC::SE_PLAYER_RUN).handleId_;
			SoundManager::GetInstance().PlayLoopSE(SoundManager::SeId::PLAYER_RAN, bgm_, SE_VOLUME_RUN);
		}
		else {
			moveSpeed_ = SPEED_MOVE;
			anim_->Play(static_cast<int>(ANIM_TYPE::RUN));
			SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_RAN);
			int bgm_ = resMng_.Load(ResourceManager::SRC::SE_PLAYER_WAKE).handleId_;
			SoundManager::GetInstance().PlayLoopSE(SoundManager::SeId::PLAYER_WAKE, bgm_, SE_VOLUME_WALK);
		}

		movePow_ = VScale(moveDir_, moveSpeed_);
	}
	else {
		state_ = STATE::IDLE;
		SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_WAKE);
		SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_RAN);
		anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
	}
}

void Player::ProcessAttack(void)
{
	// 攻撃入力判定
	auto& ins = InputManager::GetInstance();
	bool isAttackInput = false;

	// 攻撃入力判定
	if (uiSt_->GetSt() >= 0) {
		isAttackInput = ins.IsClickMouseLeft()
			|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER);
	}

	// 通常状態（移動中など）からの新規攻撃開始
	if (isAttackInput && !isJump_
		&& (state_ == STATE::IDLE || state_ == STATE::RUN || state_ == STATE::FAST_RUN))
	{
		// 攻撃状態に遷移
		state_ = STATE::ATTACK;
		stateAtkCombo_ = STATE_ATTACK_COMBO::COMBO_1;

		// データの参照と初期設定
		auto& comboData = atkComboData_.at(stateAtkCombo_);
		comboData.isNextCombo = false;

		uiSt_->SetSt(CONSUMPTION_ST_ATTACK);

		// 追加の初期処理があれば実行
		if (comboData.extraInit) {
			comboData.extraInit();
		}

		// 攻撃アニメーション再生
		anim_->Play(static_cast<int>(comboData.animType), false);
	}

	// 攻撃状態でなければ処理終了
	if (state_ != STATE::ATTACK) return;

	// 現在のコンボデータを取得
	auto& comboData = atkComboData_.at(stateAtkCombo_);
	float currentStep = anim_->GetPlayAnim().step;

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

	// 衝突判定（Hitbox）の処理
	if (comboData.IsValidCollsion(currentStep)) {
		wepon_->SetCollider();
	}
	else {
		wepon_->ClearCollider();
	}

	// SE再生処理
	if (currentStep == ATTACK_SE_TRIGGER_STEP) {
		int bgm_ = resMng_.Load(ResourceManager::SRC::SE_PLAYER_WEAPON_1).handleId_;
		SoundManager::GetInstance().PlaySE(
			SoundManager::SeId::PLAYER_WEPON_SE1, bgm_, SE_VOLUME_ATTACK);
	}

	// コンボ遷移チェック
	bool canInterrupt = comboData.IsValidInterrupt(currentStep);
	bool isAnimEnd = anim_->IsEnd();

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
			wepon_->ClearCollider();

			// 新しいコンボデータのセットアップ
			auto& nextData = atkComboData_.at(stateAtkCombo_);
			nextData.isNextCombo = false;

			uiSt_->SetSt(CONSUMPTION_ST_ATTACK);

			if (nextData.extraInit) {
				nextData.extraInit();
			}

			anim_->Play(static_cast<int>(nextData.animType), false);
		}
		// アニメーションが終了した場合、または追加終了条件を満たした場合
		else if (isAnimEnd || (comboData.isExtraEnd && comboData.isExtraEnd()))
		{
			state_ = STATE::IDLE;
			stateAtkCombo_ = STATE_ATTACK_COMBO::COMBO_1;
			comboData.isNextCombo = false;
			wepon_->ClearCollider();
		}
	}
}

void Player::ProcessEvasion(void)
{
	bool isP = false;
	auto& ins = InputManager::GetInstance();

	if (uiSt_->GetSt() >= 0) {
		isP = ins.IsTrgDown(KEY_INPUT_SPACE)
			|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);
	}

	if (isP && !isJump_
		&& (state_ == STATE::IDLE || state_ == STATE::RUN || state_ == STATE::FAST_RUN))
	{
		state_ = STATE::EVASION;
		lastQrot_ = transform_.quaRotLocal;
		transform_.quaRotLocal =
			Quaternion::Mult(transform_.quaRotLocal,
				Quaternion::AngleAxis(AsoUtility::Deg2RadF(AVOIDANCE_ANGLE_AXIS_Y), AsoUtility::AXIS_Y));
		uiSt_->SetSt(CONSUMPTION_ST_EVASION);
		int bgm_ = resMng_.Load(ResourceManager::SRC::SE_PLAYER_EVASION).handleId_;
		SoundManager::GetInstance().PlaySE(SoundManager::SeId::PLAYER_AVE, bgm_, SE_VOLUME_EVASION);

		isVinclible_ = true;
		if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::CAPSULE))) {
			for (auto& col : ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE))) {
				col->SetIsCollier(false);
			}
		}
	}

	if (state_ != STATE::EVASION) return;

	anim_->Play(static_cast<int>(ANIM_TYPE::EVASION), false);
	moveSpeed_ = SPEED_EVASION;
	movePow_ = VScale(moveDir_, moveSpeed_);

	if (anim_->IsEnd()) {
		transform_.quaRotLocal = lastQrot_;
		state_ = STATE::IDLE;
		isVinclible_ = false;

		// 回避終了：コライダーを再有効化
		if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::CAPSULE))) {
			for (auto& col : ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE))) {
				col->SetIsCollier(true);
			}
		}
	}
}

void Player::ProcessDownUp(void)
{
	// ダウン状態の処理
	if (state_ == STATE::DOWN)
	{
		isVinclible_ = true;
		state_ = STATE::DOWN;

		effect_->SetEffectPos(
			static_cast<int>(effType_),
			MV1GetFramePosition(transform_.modelId, HIT_EFFECT_FRAME_NO));

		if (anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::DOWN)
			&& anim_->IsEnd()) {

			if (!uiHp_->IsActive()) {
				state_ = STATE::DIE;
			}
			else {
				anim_->Play(
					static_cast<int>(ANIM_TYPE::UP), false);

				state_ = STATE::UP;
			}
		}
	}

	// アップ状態の処理
	if (state_ == STATE::UP)
	{
		state_ = STATE::UP;
		if (anim_->IsEnd()) {
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
	if (isP && uiRecovery_->GetBottlcCnt() > 0
		&& anim_->GetPlayType() != static_cast<int>(EFFECT::HP_ABSOLUTE)
		&& state_ != STATE::RECOVERY) {

		state_ = STATE::RECOVERY;
		effect_->Play(
			static_cast<int>(EFFECT::HP_ABSOLUTE),
			VGet(transform_.pos.x, transform_.pos.y + EFFECT_RECOVERY_OFFSET_Y, transform_.pos.z),
			AsoUtility::VECTOR_ZERO,
			VScale(AsoUtility::VECTOR_ONE, EFFECT_RECOVERY_SCALE));
	}

	// 回復状態でなければ処理終了
	if (state_ != STATE::RECOVERY) return;

	// 回復アニメーション再生
	anim_->Play(static_cast<int>(ANIM_TYPE::RECOVERY), false);

	// 回復アニメーションの特定のステップで回復処理を実行
	if (anim_->GetPlayAnim().step == RECOVERY_TRIGGER_STEP) {
		uiHp_->SetHpAbsolute(HEAL_HP_AMOUNT);
		uiRecovery_->SetBottleCnt(CONSUME_RECOVERY_BOTTLE_COUNT);
		int bgm_ = resMng_.Load(ResourceManager::SRC::SE_PLAYER_RECOVERY).handleId_;
		SoundManager::GetInstance().PlaySE(SoundManager::SeId::PLAYER_HER, bgm_, SE_VOLUME_RECOVERY);
	}

	// 回復アニメーションが終了したら通常状態に遷移
	if (anim_->IsEnd()) {
		state_ = STATE::IDLE;
		SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_HER);
	}
}

void Player::ProcessDie(void)
{
	// 死亡アニメーション再生
	deathAnimationTime_ += DIE_FADE_SPEED * SceneManager::GetInstance().GetDeltaTime();
	if (deathAnimationTime_ > DIE_END_THRESHOLD) {
		deathAnimationTime_ = DIE_END_THRESHOLD;
		state_ = STATE::END;
	}
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
	effect_->SetEffectPos(static_cast<int>(effType_), transform_.pos);

	// 無敵時間の処理
	if (isVinclible_ && invincibleTimer_ >= 0.0f)
	{
		invincibleTimer_ -= 1.0f * SceneManager::GetInstance().GetDeltaTime();
	}
	else {
		// 無敵時間が終了したら無敵状態を解除
		isVinclible_ = false;
	}

	// スタミナ回復処理
	if (uiSt_->GetSt() <= UISt::MIN_ST) {
		stRecoverTime_ = CT;
	}

	// スタミナ回復処理
	if (state_ == STATE::IDLE
		|| state_ == STATE::RUN
		|| state_ == STATE::DOWN
		|| state_ == STATE::UP
		|| state_ == STATE::RECOVERY) {
		uiSt_->SetHpAbsolute(RECOVERY_ST_SPEED * SceneManager::GetInstance().GetDeltaTime());
	}

	// スタミナ回復クールタイムの処理
	if (stRecoverTime_ > 0.0f) {
		stRecoverTime_ -= 1.0f * SceneManager::GetInstance().GetDeltaTime();
	}

	// 武器のコライダーを攻撃中以外では無効化
	if (wepon_ != nullptr && state_ != STATE::ATTACK)
	{
		wepon_->ClearCollider();
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
	if (wepon_) {
		wepon_->Update();
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

void Player::UpdateProcessPost(void) {}

void Player::UpdateHitCollider(void)
{
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	if (ownColliders_.count(capsuleType) == 0) return;

	for (const auto& col : ownColliders_.at(capsuleType))
	{
		if (!col->GetIsCollier()) continue;

		for (const auto& result : col->GetCollisionResults())
		{
			if (!result.isHit_) continue;

			// --- 敵本体との衝突 ---
			if (result.targetTag_ == ColliderBase::TAG::ENEMY)
			{
				if (!isVinclible_)
				{
					anim_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
					state_ = STATE::DOWN;
					uiHp_->SetHp(DAMAGE_ENEMY_BODY);

					effType_ = EFFECT::BLOOD;
					effect_->Play(static_cast<int>(effType_));
					effect_->SetEffectScl(static_cast<int>(EFFECT::BLOOD), EFFECT_BLOOD_SCALE);

					int seHandle = resMng_.Load(ResourceManager::SRC::SE_PLAYER_DAMAGE).handleId_;
					SoundManager::GetInstance().PlaySE(
						SoundManager::SeId::PLAYER_DMAGE, seHandle, SE_VOLUME_DAMAGE);
					return;
				}
			}

			// --- 敵武器との衝突 ---
			if (result.targetTag_ == ColliderBase::TAG::ENEMY_WEPON)
			{
				if (isVinclible_ || result.isBlocked) continue;

				anim_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
				state_ = STATE::DOWN;
				uiHp_->SetHp(DAMAGE_ENEMY_WEAPON);

				effType_ = EFFECT::BLOOD;
				effect_->Play(static_cast<int>(effType_));
				effect_->SetEffectScl(static_cast<int>(EFFECT::BLOOD), EFFECT_BLOOD_SCALE);

				int seHandle = resMng_.Load(ResourceManager::SRC::SE_PLAYER_DAMAGE).handleId_;
				SoundManager::GetInstance().PlaySE(
					SoundManager::SeId::PLAYER_DMAGE, seHandle, SE_VOLUME_DAMAGE);
				return;
			}
		}
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

	if (targetTrans_ != nullptr)
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