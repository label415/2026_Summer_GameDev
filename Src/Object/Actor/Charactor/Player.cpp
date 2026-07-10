#include <DxLib.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"
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

	DrawFormatString(0, 0, 0x000000, L"スタミナ(%.2f)",st_);
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
				// 敵本体（ENEMY）との当たり判定
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
							return;
						}
						else {
							colliderCapsule1->PushBackAlongNormal(colliderCapsule2, transform_, 20, false, false);
						}
					}
				}

				if (isV_) continue;

				bool isModel = false;
				for (const auto& hitCol : hitColliders_)
				{
					for (const auto& i : hitCol.second)
					{
						if (i->GetShape() == ColliderBase::SHAPE::MODEL
							&& i->GetTag() == ColliderBase::TAG::STAGE) {
							const ColliderModel* colliderModel =
								dynamic_cast<ColliderModel*>(i);
							if (colliderModel == nullptr) continue;

							if (colliderModel->IsHit(enemyPos, playerPos, true, false)) {
								isModel = true;
							}
						}
					}
				}


				// 敵の武器（カプセル）との衝突
				if (i->GetShape() == ColliderBase::SHAPE::CAPSULE
					&& i->GetTag() == ColliderBase::TAG::ENEMY_WEPON) {

					ColliderCapsule* colliderCapsule2 =
						dynamic_cast<ColliderCapsule*>(i);
					if (colliderCapsule2 == nullptr) continue;

					auto hits = HitCheck_Capsule_Capsule(
						colliderCapsule1->GetPosTop(), colliderCapsule1->GetPosDown(), colliderCapsule1->GetRadius(),
						colliderCapsule2->GetPosTop(), colliderCapsule2->GetPosDown(), colliderCapsule2->GetRadius());

					if (hits && !isModel) {
						anim_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
						state_ = STATE::DOWN;
						uiHp_->SetHp(40.0f);
						return;
					}
				}

				// 敵の武器（ブレス球体）との衝突
				if (i->GetShape() == ColliderBase::SHAPE::SPHERE
					&& i->GetTag() == ColliderBase::TAG::ENEMY_WEPON) {

					ColliderSphere* colliderSphere =
						dynamic_cast<ColliderSphere*>(i);
					if (colliderSphere == nullptr) continue;

					auto hits = HitCheck_Sphere_Capsule(
						colliderSphere->GetPos(), colliderSphere->GetRadius(),
						colliderCapsule1->GetPosTop(), colliderCapsule1->GetPosDown(), colliderCapsule1->GetRadius());

					// 事前チェックした isModel を使うので、処理順に左右されず確実に防げる！
					if (hits && !isModel) {
						anim_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
						state_ = STATE::DOWN;
						uiHp_->SetHp(40.0f);
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
	anim_->Add(static_cast<int>(ANIM_TYPE::ATTACK),
		40.0f, resMng_.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLSYER_ATTACK));
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

	//スタミナ
	st_ = MAX_ST;

	//クールタイム
	ct_ = 0.0f;

	uiHp_ = new UIHp(10.0f, 10.0f, 500.0f, 30.0f, 5.0f);
	// スタミナUIを HP の下に表示
	uiSt_ = new UISt(10.0f, 40.0f, 500.0f, 60.0f, 5.0f, MAX_ST);

	uiRecovery_->Init();
}

void Player::ProcessMove(void)
{
	moveSpeed_ = 0.0f;
	movePow_ = AsoUtility::VECTOR_ZERO;
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (state_ == STATE::ATTACK
		|| state_ == STATE::AVOIDANCE
		|| state_ == STATE::DOWN
		|| state_ == STATE::RECOVERY) {
		return;
	}

	auto& ins = InputManager::GetInstance();

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
	// アナログキーの入力値から方向を取得
	VECTOR padDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	// 両対応：ゲームパッドの入力がある場合はゲームパッド優先、なければキーボード
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
	}

	if (!AsoUtility::EqualsVZero(dir))
	{
		state_ = STATE::RUN;

		auto& ins = InputManager::GetInstance();
		bool isR = isR = ins.IsNew(KEY_INPUT_LSHIFT)
			|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

		if (isR && ct_ <= 0.0f) {

			moveSpeed_ = SPEED_DASH;
			state_ = STATE::FAST_RUN;
			st_ -= (CONSUMPTION_ST_FAST_RUN * SceneManager::GetInstance().GetDeltaTime());
			anim_->Play(static_cast<int>(ANIM_TYPE::FAST_RUN));
		}
		else {
			moveSpeed_ = SPEED_MOVE;
			anim_->Play(static_cast<int>(ANIM_TYPE::RUN));
		}

		if (targetTrans_ != nullptr)
		{
			VECTOR targetDir = GetTargetDir();
			float targetAngleY = atan2f(targetDir.x, targetDir.z);
			Quaternion targetRot = Quaternion::AngleAxis(targetAngleY, AsoUtility::AXIS_Y);
			transform_.quaRot = Quaternion::Slerp(transform_.quaRot, targetRot, 0.1f);
			moveDir_ = Quaternion::PosAxis(targetRot, dir);
		}
		else
		{
			// 通常時：カメラ方向を基準に移動
			Quaternion cameraRot = scnMng_.GetCamera()->GetQuaRotY();
			moveDir_ = Quaternion::PosAxis(cameraRot, dir);
		}
		movePow_ = VScale(moveDir_, moveSpeed_);
	}
	else {
		state_ = STATE::IDLE;
		anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
	}

}

void Player::ProcessAttack(void)
{
	bool isHitAttack = false;
	auto& ins = InputManager::GetInstance();
	if (st_ >= 0) {
		isHitAttack = ins.IsClickMouseLeft()
			|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER);
	}

	if (isHitAttack && !isJump_ && state_ != STATE::ATTACK
		&& state_ != STATE::AVOIDANCE
		&& state_ != STATE::DOWN
		&& state_ != STATE::RECOVERY) {
		state_ = STATE::ATTACK;
		st_ -= CONSUMPTION_ST_ATTACK;
	}

	if (state_ != STATE::ATTACK) return;

	anim_->Play(
		static_cast<int>(ANIM_TYPE::ATTACK), false);

	if (anim_->GetPlayAnim().step >= STATE_ATTACK_CILLIDER) {
		isAttack_ = true;
		wepon_->SetCollider();
	}

	if (anim_->IsEnd()) {
		state_ = STATE::IDLE;
		wepon_->ClearCollider();
	}
}

void Player::ProcessAvoidance(void)
{

	bool isP = false;
	auto& ins = InputManager::GetInstance();

	if (st_ >= 0) {
		isP = ins.IsTrgDown(KEY_INPUT_SPACE)
			|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);
	}

	if (isP && !isJump_
		&& state_ != STATE::ATTACK
		&& state_ != STATE::AVOIDANCE
		&& state_ != STATE::DOWN
		&& state_ != STATE::RECOVERY)
	{
		state_ = STATE::AVOIDANCE;
		lastQrot_ = transform_.quaRotLocal;
		transform_.quaRotLocal =
			Quaternion::Mult(transform_.quaRotLocal,
				Quaternion::AngleAxis(AsoUtility::Deg2RadF(100.0f), AsoUtility::AXIS_Y));
		st_ -= CONSUMPTION_ST_AVOIDANCE;
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

		if (anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::DOWN)
			&& anim_->IsEnd()) {
			anim_->Play(
				static_cast<int>(ANIM_TYPE::UP), false);
		}
		else if (anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::UP)
			&& anim_->IsEnd()) {
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

	if (isP) {
		state_ = STATE::RECOVERY;
	}

	if (state_ != STATE::RECOVERY) return;

	anim_->Play(static_cast<int>(ANIM_TYPE::RECOVERY), false);

	if (anim_->GetPlayAnim().step == 10.0f) {
		uiHp_->SetHpAbsolute(40.0f);
		uiRecovery_->SetBottleCnt(1);
	}

	if (anim_->IsEnd()) {
		state_ = STATE::IDLE;
	}


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
	isV_ = false;
	if (st_ < MIN_ST) {
		st_ = MIN_ST;
		ct_ = CT;
	}
	else if (st_ >= MIN_ST && st_ <= MAX_ST
		&& (state_ == STATE::IDLE || state_ == STATE::RUN || state_ == STATE::DOWN)) {
		st_ += (RECOVERY_ST_SPEED * SceneManager::GetInstance().GetDeltaTime());
	}

	if (ct_ > 0.0f) {
		ct_ -= 1.0f * SceneManager::GetInstance().GetDeltaTime();
	}

	// UI に現在のスタミナを反映（絶対値セット）
	if (uiSt_) {
		uiSt_->SetHpAbsolute(st_);
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

	VECTOR LockPos;
	if(STATE::DOWN == state_)
	{
		LockPos = LOCK_POS2;
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
