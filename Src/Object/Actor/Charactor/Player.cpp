#include <DxLib.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
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
{
	wepon_ = nullptr;
	uiSt_ = nullptr;
}

Player::~Player(void)
{
}

void Player::Draw(void)
{
	CharactorBase::Draw();

	wepon_->Draw();
}

void Player::Release(void)
{
	transform_.Release();

	wepon_->Release();
	delete wepon_;
	delete uiHp_;
	delete uiRecovery_;
	if (uiSt_) {
		delete uiSt_;
		uiSt_ = nullptr;
	}
}

void Player::HitDamage(bool isHit)
{
	VECTOR playerPos = transform_.pos;
	VECTOR enemyPos = AsoUtility::VECTOR_ZERO;
	if (targetTrans_ != nullptr)
	{
		enemyPos = *targetTrans_;
	}

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

		if (colliderCapsule1 == nullptr) continue;

		playerPos = colliderCapsule1->GetFollow()->pos;

		for (const auto& hitCol : hitColliders_)
		{
			for (const auto& i : hitCol.second)
			{

				if (i->GetShape() == ColliderBase::SHAPE::CAPSULE
					&& i->GetTag() == ColliderBase::TAG::ENEMY) {

					const ColliderCapsule* colliderCapsule2 =
						dynamic_cast<ColliderCapsule*>(i);
					if (colliderCapsule2 == nullptr) continue;

					auto hits = HitCheck_Capsule_Capsule(
						colliderCapsule1->GetPosTop(), colliderCapsule1->GetPosDown(), colliderCapsule1->GetRadius(),
						colliderCapsule2->GetPosTop(), colliderCapsule2->GetPosDown(), colliderCapsule2->GetRadius());

					if (hits) {
						if (isHit && !isV_) {
							anim_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
							state_ = STATE::DOWN;
							uiHp_->SetHp(30.0f);
							effType_ = EFFECT::BLOOD;
							effect_->Play(static_cast<int>(effType_));
							effect_->SetEffectScl(static_cast<int>(EFFECT::BLOOD), VGet(5.0f, 5.0f, 5.0f));
							int bgm_ = resMng_.Load(ResourceManager::SRC::PLAYER_DMAGE).handleId_;
							int volume_ = 50;
							SoundManager::GetInstance().PlaySE(SoundManager::SeId::PLAYER_DMAGE, bgm_, volume_);
							return;
						}
						else {
							colliderCapsule1->PushBackAlongNormal(colliderCapsule2, transform_, 20, false, false);
						}
					}
				}

				if (isV_) continue;

				bool isBlockedByWall = false;

				for (const auto& stageColPair : hitColliders_)
				{
					for (const auto& stageCol : stageColPair.second)
					{
						if (stageCol->GetShape() == ColliderBase::SHAPE::MODEL
							&& stageCol->GetTag() == ColliderBase::TAG::STAGE) {
							const ColliderModel* colliderModel =
								dynamic_cast<ColliderModel*>(stageCol);
							if (colliderModel == nullptr) continue;

							// 敵とプレイヤーの間の直線上にモデルがあれば true
							if (colliderModel->IsHit(enemyPos, playerPos, false, false)) {
								isBlockedByWall = true;
								break;
							}
						}
					}
					if (isBlockedByWall) break;
				}

				if (isBlockedByWall) continue;

				if (i->GetShape() == ColliderBase::SHAPE::CAPSULE
					&& i->GetTag() == ColliderBase::TAG::ENEMY_WEPON) {

					ColliderCapsule* colliderCapsule2 =
						dynamic_cast<ColliderCapsule*>(i);
					if (colliderCapsule2 == nullptr) continue;

					auto hits = HitCheck_Capsule_Capsule(
						colliderCapsule1->GetPosTop(), colliderCapsule1->GetPosDown(), colliderCapsule1->GetRadius(),
						colliderCapsule2->GetPosTop(), colliderCapsule2->GetPosDown(), colliderCapsule2->GetRadius());

					if (hits) {
						anim_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
						state_ = STATE::DOWN;
						uiHp_->SetHp(40.0f);
						int bgm_ = resMng_.Load(ResourceManager::SRC::PLAYER_DMAGE).handleId_;
						int volume_ = 50;
						SoundManager::GetInstance().PlaySE(SoundManager::SeId::PLAYER_DMAGE, bgm_, volume_);
						return;
					}
				}

				if (i->GetShape() == ColliderBase::SHAPE::SPHERE
					&& i->GetTag() == ColliderBase::TAG::ENEMY_WEPON) {

					ColliderSphere* colliderSphere =
						dynamic_cast<ColliderSphere*>(i);
					if (colliderSphere == nullptr) continue;

					auto hits = HitCheck_Sphere_Capsule(
						colliderSphere->GetPos(), colliderSphere->GetRadius(),
						colliderCapsule1->GetPosTop(), colliderCapsule1->GetPosDown(), colliderCapsule1->GetRadius());

					if (hits) {
						anim_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
						state_ = STATE::DOWN;
						uiHp_->SetHp(40.0f);
						int bgm_ = resMng_.Load(ResourceManager::SRC::PLAYER_DMAGE).handleId_;
						int volume_ = 50;
						SoundManager::GetInstance().PlaySE(SoundManager::SeId::PLAYER_DMAGE, bgm_, volume_);
						return;
					}
				}
			}
		}
	}
}

void Player::DrawHp(void)
{
	if (uiHp_) { uiHp_->Draw(); }
	if (uiSt_) { uiSt_->Draw(); }

	if (uiRecovery_) { uiRecovery_->Draw(); }
}

void Player::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();

	//プレイヤー
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));

	wepon_ = new WeponBlade(transform_, 48);
	wepon_->Load();

	uiRecovery_ = new UIRecovery(5);
	uiRecovery_->Load();

	uiHp_ = new UIHp(
		Application::SCREEN_SIZE_X / 4, 20,
		0.7f, 2.9f, 4.0f);
	uiHp_->Load();

	// スタミナUIを HP の下に表示
	uiSt_ = new UISt(
		Application::SCREEN_SIZE_X / 4, 60,
		0.7f, 2.9f, 4.0f);
	uiSt_->Load();
}

void Player::InitTransform(void)
{
	transform_.scl = { 1.0f, 1.0f, 1.0f };
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,   
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));
	transform_.pos = { 0.0f, 0.0f, -500.0f };
	transform_.Update();
}

void Player::InitCollider(void)
{
	// 線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::PLAYER, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);

	std::vector<ColliderBase*> colLines;
	colLines.push_back(colLine);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLines);


	// カプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::PLAYER, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);

	std::vector<ColliderBase*> colCapsules;
	colCapsules.push_back(colCapsule);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsules);
}

void Player::InitAnimation(void)
{
	anim_ = new AnimationController(transform_.modelId);

	anim_->Add(static_cast<int>(ANIM_TYPE::IDLE),
		30.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_IDLE));
	anim_->Add(static_cast<int>(ANIM_TYPE::RUN),
		25.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_RUN));
	anim_->Add(static_cast<int>(ANIM_TYPE::FAST_RUN),
		30.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_RUN));
	anim_->Add(static_cast<int>(ANIM_TYPE::ATTACK_1),
		40.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLSYER_ATTACK_1));
	anim_->Add(static_cast<int>(ANIM_TYPE::ATTACK_2),
		40.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLSYER_ATTACK_2));
	anim_->Add(static_cast<int>(ANIM_TYPE::ATTACK_3),
		40.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLSYER_ATTACK_3));
	anim_->Add(static_cast<int>(ANIM_TYPE::AVOIDANCE),
		80.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_AVOIDANCE));
	anim_->Add(static_cast<int>(ANIM_TYPE::DOWN),
		50.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_DOWN));
	anim_->Add(static_cast<int>(ANIM_TYPE::UP),
		100.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_UP));
	anim_->Add(static_cast<int>(ANIM_TYPE::RECOVERY),
		40.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_RECOVERY));
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

	//状態
	state_ = STATE::IDLE;

	//武器
	wepon_->Init();

	uiHp_->Init();

	uiSt_->Init();

	//クールタイム
	ct_ = 0.0f;

	i_ = 0.0f;

	uiRecovery_->Init();

	effType_ = EFFECT::NONE;
	effect_ = new EffectController();
	effect_->Add(
		static_cast<int>(EFFECT::BLOOD),
		(Application::PATH_EFFECT + L"Blood.efkefc"));
	effect_->Add(
		static_cast<int>(EFFECT::HP_ABSOLUTE),
		(Application::PATH_EFFECT + L"Absolute.efkefc"));

	ATTACK_COMBO data;
	// 横切り攻撃(アニメーション時間：0.0～72.0)
	// コンボ受付開始、衝突判定開始
	data = {
		ANIM_TYPE::ATTACK_1,
		20.0f, 50.0f, 24.0f, 38.0f, 55.0f, 8.0f,
		STATE_ATTACK_COMBO::COMBO_2, [this](void) { return false; }, false,
		false,
		nullptr, nullptr, nullptr, nullptr,
	};
	atkComboData_.emplace(
		STATE_ATTACK_COMBO::COMBO_1, data);

	// 縦切り攻撃(アニメーション時間：0.0～68.0)
	// コンボ受付開始、衝突判定開始
	data = {
		ANIM_TYPE::ATTACK_2,
		15.0f, 40.0f, 18.0f, 32.0f, 50.0f, 5.0f,
		STATE_ATTACK_COMBO::COMBO_3, [this](void) { return false; }, false,
		false,
		nullptr, nullptr, nullptr, nullptr,
	};
	atkComboData_.emplace(
		STATE_ATTACK_COMBO::COMBO_2, data);

	// 回転攻撃
	// コンボ受付開始、衝突判定開始
	data = {
		ANIM_TYPE::ATTACK_3,
		0.0f, 0.0f, 26.0f, 34.0f, 95.0f, 12.0f,
		STATE_ATTACK_COMBO::MAX, [this]() { return false; }, false,
		true,
		nullptr, nullptr, nullptr, nullptr,
	};
	atkComboData_.emplace(
		STATE_ATTACK_COMBO::COMBO_3, data);

	stateAtkCombo_ = STATE_ATTACK_COMBO::COMBO_1;
	isComboNext_ = false;
}

void Player::ProcessMove(void)
{
	moveSpeed_ = 0.0f;
	movePow_ = AsoUtility::VECTOR_ZERO;

	if (state_ != STATE::IDLE
		&& state_ != STATE::RUN
		&& state_ != STATE::FAST_RUN) {
		SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_WAKE);
		SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_RAN);
		return;
	}

	// ★ 共通化した関数から方向（カメラ/ターゲット変換済み）を取得
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
			transform_.quaRot = Quaternion::Slerp(transform_.quaRot, targetRot, 0.1f);
		}

		auto& ins = InputManager::GetInstance();
		bool isR = ins.IsNew(KEY_INPUT_LSHIFT)
			|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

		if (isR && ct_ <= 0.0f) {
			moveSpeed_ = SPEED_DASH;
			state_ = STATE::FAST_RUN;
			uiSt_->SetSt(CONSUMPTION_ST_FAST_RUN * SceneManager::GetInstance().GetDeltaTime());
			anim_->Play(static_cast<int>(ANIM_TYPE::FAST_RUN));
			SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_WAKE);
			int bgm_ = resMng_.Load(ResourceManager::SRC::PLAYER_RAN).handleId_;
			int volume_ = 50;
			SoundManager::GetInstance().PlayLoopSE(SoundManager::SeId::PLAYER_RAN, bgm_, volume_);
		}
		else {
			moveSpeed_ = SPEED_MOVE;
			anim_->Play(static_cast<int>(ANIM_TYPE::RUN));
			SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_RAN);
			int bgm_ = resMng_.Load(ResourceManager::SRC::PLAYER_WAKE).handleId_;
			int volume_ = 50;
			SoundManager::GetInstance().PlayLoopSE(SoundManager::SeId::PLAYER_WAKE, bgm_, volume_);
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
	auto& ins = InputManager::GetInstance();
	bool isAttackInput = false;

	// 1. 攻撃入力判定
	if (uiSt_->GetSt() >= 0) {
		isAttackInput = ins.IsClickMouseLeft()
			|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER);
	}

	// 2. 通常状態（移動中など）からの新規攻撃開始
	if (isAttackInput && !isJump_
		&& (state_ == STATE::IDLE || state_ == STATE::RUN || state_ == STATE::FAST_RUN))
	{
		state_ = STATE::ATTACK;
		stateAtkCombo_ = STATE_ATTACK_COMBO::COMBO_1; // 最初からスタート

		// データの参照と初期設定
		auto& comboData = atkComboData_.at(stateAtkCombo_);
		comboData.isNextCombo = false; // 次コンボフラグをリセット

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

	// --- ここから STATE::ATTACK の更新処理 ---

	auto& comboData = atkComboData_.at(stateAtkCombo_);
	float currentStep = anim_->GetPlayAnim().step;

	if (comboData.moveSpeed > 0.0f && currentStep < comboData.stepCollisionStart)
	{
		// 1. 入力方向を取得
		VECTOR inputDir = GetInputDirection();

		// 2. 「入力がある場合」のみ回転と踏み込み移動を行う
		// （攻撃開始直後や入力がない状態では移動・回転させない）
		if (!AsoUtility::EqualsVZero(inputDir))
		{
			// ① 入力方向へ回転（Y軸）
			float targetAngleY = atan2f(inputDir.x, inputDir.z);
			Quaternion targetRot = Quaternion::AngleAxis(targetAngleY, AsoUtility::AXIS_Y);
			transform_.quaRot = targetRot;

			// ② 向いた正面方向へ向かって踏み込み移動（Lerp補間）
			VECTOR forward = transform_.GetForward();
			VECTOR targetPos = VAdd(transform_.pos, VScale(forward, comboData.moveSpeed));

			transform_.pos = AsoUtility::Lerp(transform_.pos, targetPos, 0.2f);
		}
	}

	// 3. 追加更新処理（ガードキャンセルや特殊効果など）
	if (comboData.extraUpdate) {
		comboData.extraUpdate();
	}

	// 4. コンボ入力受付判定
	// 構造体の IsValidCombo() を使用
	if (comboData.IsValidCombo(currentStep)) {
		// 通常の攻撃入力、または固有の条件（actionNextCombo）を満たした場合
		bool isCustomAction = comboData.actionNextCombo ? comboData.actionNextCombo() : false;
		if (isAttackInput || isCustomAction) {
			comboData.isNextCombo = true;
		}
	}

	// 5. 衝突判定（Hitbox）の処理
	// 構造体の IsValidCollsion() を使用
	if (comboData.IsValidCollsion(currentStep)) {
		wepon_->SetCollider();
	}
	else {
		wepon_->ClearCollider();
	}

	// 6. SE再生処理（例：ステップ固定または必要に応じて条件化）
	if (currentStep == 10.0f) {
		int bgm_ = resMng_.Load(ResourceManager::SRC::PLAYER_WEPON_SE1).handleId_;
		int volume_ = 30;
		SoundManager::GetInstance().PlaySE(SoundManager::SeId::PLAYER_WEPON_SE1, bgm_, volume_);
	}

	// 7. コンボ遷移チェック（割り込みタイミング or アニメーション終了時）
	bool canInterrupt = comboData.IsValidInterrupt(currentStep);
	bool isAnimEnd = anim_->IsEnd();

	// 追加割込条件のチェック
	if (comboData.isExtraInterrupt && comboData.isExtraInterrupt(comboData)) {
		canInterrupt = true;
	}

	// 「割り込み可能ステップに達している」かつ「入力済み（isNextCombo）」または「アニメーション終了」の時にチェック
	if ((canInterrupt && comboData.isNextCombo) || isAnimEnd) {
		// 次のコンボへ繋ぐ場合
		if (comboData.isNextCombo && comboData.nextCombo != STATE_ATTACK_COMBO::MAX) {
			// 次のコンボへ状態遷移
			stateAtkCombo_ = comboData.nextCombo;
			wepon_->ClearCollider();

			// 新しいコンボデータのセットアップ
			auto& nextData = atkComboData_.at(stateAtkCombo_);
			nextData.isNextCombo = false; // フラグ初期化

			uiSt_->SetSt(CONSUMPTION_ST_ATTACK);

			if (nextData.extraInit) {
				nextData.extraInit();
			}

			anim_->Play(static_cast<int>(nextData.animType), false);
		}
		// コンボを継続しない / 最終段が終わった場合（かつアニメーションが終了している場合）
		else if (isAnimEnd || (comboData.isExtraEnd && comboData.isExtraEnd())) {
			state_ = STATE::IDLE;
			stateAtkCombo_ = STATE_ATTACK_COMBO::COMBO_1;
			comboData.isNextCombo = false;
			wepon_->ClearCollider();
		}
	}
}

void Player::ProcessAvoidance(void)
{

	bool isP = false;
	auto& ins = InputManager::GetInstance();

	if (uiSt_->GetSt() >= 0) {
		isP = ins.IsTrgDown(KEY_INPUT_SPACE)
			|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);
	}

	if (isP && !isJump_
		&& (state_ == STATE::IDLE
		|| state_ == STATE::RUN
		|| state_ == STATE::FAST_RUN))
	{
		state_ = STATE::AVOIDANCE;
		lastQrot_ = transform_.quaRotLocal;
		transform_.quaRotLocal =
			Quaternion::Mult(transform_.quaRotLocal,
				Quaternion::AngleAxis(AsoUtility::Deg2RadF(100.0f), AsoUtility::AXIS_Y));
		uiSt_->SetSt(CONSUMPTION_ST_AVOIDANCE);
		int bgm_ = resMng_.Load(ResourceManager::SRC::PLAYER_AVE).handleId_;
		int volume_ = 50;
		SoundManager::GetInstance().PlaySE(SoundManager::SeId::PLAYER_AVE, bgm_, volume_);

	}

	if (state_ != STATE::AVOIDANCE) return;

	anim_->Play(
		static_cast<int>(ANIM_TYPE::AVOIDANCE), false);

	moveSpeed_ = 10.0f;
	movePow_ = VScale(moveDir_, moveSpeed_);
	isV_ = true;

	if (anim_->IsEnd()) {
		transform_.quaRotLocal = lastQrot_;
		state_ = STATE::IDLE;
	}

}

void Player::ProcessDownUp(void)
{
	if (state_ == STATE::DOWN)
	{
		isV_ = true;
		state_ = STATE::DOWN;

		effect_->SetEffectPos(static_cast<int>(effType_), MV1GetFramePosition(transform_.modelId, 2));

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

	if(state_ == STATE::UP)
	{
		isV_ = true;
		state_ = STATE::UP;
		if (anim_->IsEnd()) {
			state_ = STATE::IDLE;
		}
	}

}

void Player::ProcessRecovery(void)
{
	bool isP = false;
	auto& ins = InputManager::GetInstance();
	if (state_ != STATE::IDLE
		&& state_ != STATE::RUN
		&& state_ != STATE::FAST_RUN
		&& state_ != STATE::RECOVERY) {
		return;
	}

	isP = ins.IsTrgDown(KEY_INPUT_R)
		|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT);

	if (isP && uiRecovery_->GetBottlcCnt() > 0
		&& anim_ ->GetPlayType() != static_cast<int>(EFFECT::HP_ABSOLUTE)
		&& state_ != STATE::RECOVERY) {

		state_ = STATE::RECOVERY;
		effect_->Play(
			static_cast<int>(EFFECT::HP_ABSOLUTE),
			VGet(transform_.pos.x, transform_.pos.y + 100.0f, transform_.pos.z),
			AsoUtility::VECTOR_ZERO,
			VScale(AsoUtility::VECTOR_ONE, 10.0f));
	}

	if (state_ != STATE::RECOVERY) return;

	anim_->Play(static_cast<int>(ANIM_TYPE::RECOVERY), false);

	if (anim_->GetPlayAnim().step == 10.0f) {
		uiHp_->SetHpAbsolute(40.0f);
		uiRecovery_->SetBottleCnt(1);
		int bgm_ = resMng_.Load(ResourceManager::SRC::PLAYER_HER).handleId_;
		int volume_ = 50;
		SoundManager::GetInstance().PlaySE(SoundManager::SeId::PLAYER_HER, bgm_, volume_);
	}

	if (anim_->IsEnd()) {
		state_ = STATE::IDLE;
		SoundManager::GetInstance().StopSE(SoundManager::SeId::PLAYER_HER);
	}


}

void Player::ProcessDie(void)
{
	i_ += 0.3 * SceneManager::GetInstance().GetDeltaTime();
	if (i_ > 1.8f) {
		i_ = 1.8f;
		state_ = STATE::END;
	}
	//MV1SetOpacityRate(transform_.modelId, 1.0f - i_);
}

void Player::CollisionReserve(void)
{
	
	if (anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::AVOIDANCE)
		|| anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::DOWN))
	{
		if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::LINE)) != 0)
		{
			const auto& vec = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::LINE));
			if (!vec.empty())
			{
				ColliderLine* colLine = dynamic_cast<ColliderLine*>(vec.front());
				if (colLine)
				{
					colLine->SetLocalPosStart(COL_LINE_AVOIDANCE_START_LOCAL_POS);
					colLine->SetLocalPosEnd(COL_LINE_AVOIDANCE_END_LOCAL_POS);
				}
			}
		}
		if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::CAPSULE)) != 0)
		{
			const auto& vec = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
			if (!vec.empty())
			{
				ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(vec.front());
				if (colCapsule)
				{
					colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_AVOIDANCE_LOCAL_POS);
					colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_AVOIDANCE_LOCAL_POS);
				}
			}
		}
	}
	else
	{
		if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::LINE)) != 0)
		{
			const auto& vec = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::LINE));
			if (!vec.empty())
			{
				ColliderLine* colLine = dynamic_cast<ColliderLine*>(vec.front());
				if (colLine)
				{
					colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
					colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
				}
			}
		}
		if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::CAPSULE)) != 0)
		{
			const auto& vec = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
			if (!vec.empty())
			{
				ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(vec.front());
				if (colCapsule)
				{
					colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
					colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
				}
			}
		}
	}
}

void Player::UpdateProcess(void)
{
	if (state_ == STATE::DIE)
	{
		ProcessDie();
		return;
	}

	effect_->SetEffectPos(static_cast<int>(effType_), transform_.pos);

	isV_ = false;
	if (uiSt_->GetSt() <= UISt::MIN_ST) {
		ct_ = CT;
	}

	if (state_ == STATE::IDLE 
		|| state_ == STATE::RUN 
		|| state_ == STATE::DOWN 
		|| state_ == STATE::UP
		|| state_ == STATE::RECOVERY) {
		uiSt_->SetHpAbsolute(RECOVERY_ST_SPEED * SceneManager::GetInstance().GetDeltaTime());
	}

	if (ct_ > 0.0f) {
		ct_ -= 1.0f * SceneManager::GetInstance().GetDeltaTime();
	}

	if(wepon_ != nullptr && state_ != STATE::ATTACK)
	{
		wepon_->ClearCollider();
	}

	ProcessRecovery();

	//攻撃処理
	ProcessAttack();

	// 移動操作
	ProcessMove();

	//回避処理
	ProcessAvoidance();

	ProcessDownUp();


	// 武器処理
	if (wepon_){
		wepon_->Update();
	}

	if(STATE::DOWN == state_)
	{
		LockPos = LOCK_POS2;
	}
	else if (STATE::DIE == state_) {
		LockPos = LOCK_POS4;
	}
	else if (STATE::UP == state_)
	{
		LockPos.y += 0.7f;
	}
	else
	{
		LockPos = LOCK_POS1;
	}

	//アニメーションの移動量を無効
	SetFrameUserLocalPos(LockPos, LOCK_FRAME_NO);

}

void Player::UpdateProcessPost(void)
{
}

VECTOR Player::GetInputDirection(void)
{
	VECTOR dir = AsoUtility::VECTOR_ZERO;
	auto& ins = InputManager::GetInstance();

	// 1. ゲームパッドの入力を取得
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

		// 斜め入力の正規化（必要に応じて）
		if (!AsoUtility::EqualsVZero(dir)) {
			dir = VNorm(dir);
		}
	}

	// 入力がない場合はゼロベクトルを返す
	if (AsoUtility::EqualsVZero(dir))
	{
		return AsoUtility::VECTOR_ZERO;
	}

	// 2. ターゲットまたはカメラ基準のワールド方向に変換
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
