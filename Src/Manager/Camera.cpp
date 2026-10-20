#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include "../Application.h"
#include "ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/CollisionManager.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/Collider/ColliderModel.h"
#include "../Object/Common/Collider/ColliderSphere.h"
#include "Camera.h"

Camera::Camera(void)
	:
	followTransform_(nullptr),
	targetPos_(nullptr),
	mode_(MODE::NONE),
	angles_(AsoUtility::VECTOR_ZERO),
	rotY_(Quaternion::Identity()),
	fixationPos_(AsoUtility::VECTOR_ZERO),
	delayedFollowPos_(AsoUtility::VECTOR_ZERO),
	prevFollowPos_(AsoUtility::VECTOR_ZERO),
	prevStickX_(0.0f),
	prevStickZ_(0.0f)
{
}

Camera::~Camera(void) {}

void Camera::Update(void){}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FREE:
		SetBeforeDrawFree();
		break;
	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
	case Camera::MODE::TARGET_ROCKE:
		SetBeforeDrawTargetLockeOn();
		break;
	}

	// 通常カメラ時は既存の Up ベクトルを使用
	SetCameraPositionAndTargetAndUpVec(
		transform_.pos,
		fixationPos_,
		transform_.quaRot.GetUp()
	);

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();
}

void Camera::DrawDebug(void)
{
	// ロックオンUI描画
	if (mode_ != MODE::TARGET_ROCKE) return;

	SetUseZBuffer3D(FALSE);
	DrawBillboard3D(
		*targetPos_,
		0.5f, 0.5f,
		LOCON_UI_SIZE, 0.0f,
		lockOnImg_, true);
	SetUseZBuffer3D(TRUE);
}

void Camera::Release(void) {}

void Camera::InitLoad(void) {}

void Camera::InitCollider(void)
{
	// 衝突対象：ステージモデル
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::STAGE
	};

	auto colliderSphere = std::make_shared<ColliderSphere>(
		ColliderBase::TAG::CAMERA,
		targetTags,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_SPHERE
	);
	colliderSphere->SetIsCollier(true);

	ownColliders_[static_cast<int>(ColliderBase::SHAPE::SPHERE)].push_back(colliderSphere);
	CollisionManager::GetInstance().AddCollider(colliderSphere);
}

void Camera::InitPost(void)
{
	// カメラモードの初期化
	ChangeMode(MODE::FIXED_POINT);
	auto& ins = InputManager::GetInstance();
	mouseX = static_cast<float>(ins.GetMousePos().x);
	mouseY = static_cast<float>(ins.GetMousePos().y);
	preMouseX = static_cast<float>(ins.GetMousePos().x);
	preMouseY = static_cast<float>(ins.GetMousePos().y);

	// ロックオン画像ロード
	lockOnImg_ = resMng_.Load(ResourceManager::SRC::UI_LOCKON).handleId_;

	isMouseInput_ = false;
}

void Camera::UpdateHitCollider(void)
{
	// 追従対象がいなければ処理しない
	if (followTransform_ == nullptr) return;

	// カメラ自身の球体コライダーリストを取得
	int sphereType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	if (ownColliders_.count(sphereType) == 0) return;

	// プレイヤーの座標を取得
	VECTOR startPos = followTransform_->pos;
	for (const auto& col : ownColliders_.at(sphereType))
	{
		if (!col->GetIsCollier()) continue;

		// 形状はSPHEREと確定しているため static_pointer_cast でダウンキャスト
		auto colliderSphere = std::static_pointer_cast<ColliderSphere>(col);
		if (colliderSphere == nullptr) continue;

		for (const auto& hit : col->GetHitColliders())
		{
			// weak_ptr を1度だけ lock() して有効性を確認
			auto hitCol = hit.lock();
			if (!hitCol || hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

			const auto* colliderModel = static_cast<const ColliderModel*>(hitCol.get());

			transform_.pos = CollisionUtility::CalcCameraPushBack(
				*colliderSphere,
				*colliderModel,
				startPos,
				transform_.pos,
				CNT_TRY_COLLISION_CAMERA,
				COLLISION_BACK_DIS,
				false,
				false
			);
		}
	}
}

const VECTOR* Camera::FindLockOnTarget(SWITCH_DIR switchDir) const
{
	const VECTOR& playerPos = followTransform_->pos;
	VECTOR camForward = VNorm(GetForward());
	float maxAngleRad = AsoUtility::Deg2RadF(LOCKON_VIEW_ANGLE);
	float diffMin = (switchDir == SWITCH_DIR::UP) ? FLT_MAX : MAX_LOCKON_DIFF;

	const VECTOR* bestTarget = nullptr;

	for (const VECTOR* candidatePos : targetPoss_) {
		// 早期リターン
		if (candidatePos == nullptr || candidatePos == targetPos_) continue;

		VECTOR toEnemy = VSub(*candidatePos, playerPos);
		float lockonDiff = VSize(toEnemy);
		if (lockonDiff >= diffMin) continue;

		// 視野角判定
		float dot = VDot(camForward, VNorm(toEnemy));
		dot = std::clamp(dot, -1.0f, 1.0f);
		if (acosf(dot) >= maxAngleRad) continue;

		// 左右切り替え時の外積判定
		if (switchDir == SWITCH_DIR::LEFT && VCross(camForward, toEnemy).y > 0.0f) continue;
		if (switchDir == SWITCH_DIR::RIGHT && VCross(camForward, toEnemy).y < 0.0f) continue;

		diffMin = lockonDiff;
		bestTarget = candidatePos;
	}

	return bestTarget;
}

void Camera::HandleTargetSwitching(void)
{
	auto& inp = InputManager::GetInstance();
	InputManager::JOYPAD_IN_STATE padState =
		inp.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
	VECTOR dir = inp.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);
	const float stickThreshold = InputManager::THRESHOLD;

	bool isNextUp = (dir.z > stickThreshold)
		&& (prevStickZ_ <= stickThreshold);
	bool isNextDown = (dir.z < -stickThreshold)
		&& (prevStickZ_ >= -stickThreshold);
	bool isNextRight = ((dir.x > stickThreshold)
		&& (prevStickX_ <= stickThreshold))
		|| (inp.GetMouseWheelRot() > 0.0f);
	bool isNextLeft = ((dir.x < -stickThreshold)
		&& (prevStickX_ >= -stickThreshold))
		|| (inp.GetMouseWheelRot() < 0.0f);

	prevStickX_ = dir.x;
	prevStickZ_ = dir.z;

	SWITCH_DIR switchDir = SWITCH_DIR::NONE;
	if (isNextUp)         switchDir = SWITCH_DIR::UP;
	else if (isNextDown)  switchDir = SWITCH_DIR::DOWN;
	else if (isNextLeft)  switchDir = SWITCH_DIR::LEFT;
	else if (isNextRight) switchDir = SWITCH_DIR::RIGHT;

	if (switchDir != SWITCH_DIR::NONE) {
		const VECTOR* nextTarget = FindLockOnTarget(switchDir);
		if (nextTarget != nullptr) {
			targetPos_ = nextTarget;
		}
	}
}

const VECTOR& Camera::GetPos(void) const
{
	return  transform_.pos;
}

const VECTOR& Camera::GetAngles(void) const
{
	return angles_;
}

const VECTOR& Camera::GetTargetPos(void) const
{
	return fixationPos_;
}

const Quaternion& Camera::GetQuaRot(void) const
{
	return transform_.quaRot;
}

const Quaternion& Camera::GetQuaRotY(void) const
{
	return rotY_;
}

VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(fixationPos_, transform_.pos));
}

void Camera::ChangeMode(MODE mode)
{
	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetDefault();
		break;
	case Camera::MODE::FREE:
		SetDefault();
		break;
	case Camera::MODE::FOLLOW:
		break;
	case Camera::MODE::TARGET_ROCKE:
		break;
	}
}

const Camera::MODE& Camera::GetCameraMode(void) const
{
	return mode_;
}

void Camera::AddTargetPos(const VECTOR* targetPos)
{
	if (targetPos == nullptr) return;

	// すでに同じ座標ポインタが格納されていなければ追加
	if (std::find(targetPoss_.begin(),
		targetPoss_.end(),
		targetPos) == targetPoss_.end())
	{
		targetPoss_.push_back(targetPos);
	}
}

void Camera::UpdateLockOn(void)
{
	// 早期リターン
	if (followTransform_ == nullptr) return;

	auto& inp = InputManager::GetInstance();
	bool isLockOnTrg = inp.IsTrgMouseMiddle()
		|| inp.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER);

	// ロックオン中の処理
	if (mode_ == MODE::TARGET_ROCKE)
	{
		// 早期リターン
		if (targetPos_ == nullptr ||
			VSize(VSub(*targetPos_, followTransform_->pos)) >= MAX_LOCKON_DIFF ||
			isLockOnTrg)
		{
			targetPos_ = nullptr;
			ChangeMode(MODE::FOLLOW);
			return;
		}

		// スティック・ホイール入力によるターゲット切り替え処理
		HandleTargetSwitching();
		return;
	}

	// 通常追従中の処理
	if (mode_ == MODE::FOLLOW && isLockOnTrg)
	{
		const VECTOR* newTarget = FindLockOnTarget(SWITCH_DIR::NONE);
		if (newTarget != nullptr) {
			targetPos_ = newTarget;
			delayedFollowPos_ = followTransform_->pos;
			prevFollowPos_ = followTransform_->pos;
			ChangeMode(MODE::TARGET_ROCKE);
		}
	}
}

void Camera::SetDefault(void)
{
	// カメラの初期設定
	transform_.pos = DERFAULT_POS;

	// カメラ角
	angles_ = DERFAULT_ANGLES;
	transform_.quaRot = Quaternion::Identity();

	// 注視点
	fixationPos_ = AsoUtility::VECTOR_ZERO;
}

void Camera::SyncFollow(void)
{
	// 同期先の位置
	VECTOR pos = followTransform_->pos;

	// Y軸 + X軸
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;
	// 注視点
	localPos = transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS);
	VECTOR nextTargetPos = VAdd(pos, localPos);
	fixationPos_ = AsoUtility::Lerp(transform_.pos, nextTargetPos, CAMERA_FOLLOW_RATE);

	// カメラ位置
	localPos = transform_.quaRot.PosAxis(FOLLOW_CAMERA_LOCAL_POS);
	VECTOR nextCameraPos = VAdd(pos, localPos);
	transform_.pos = AsoUtility::Lerp(transform_.pos, nextCameraPos, CAMERA_FOLLOW_RATE);
}

void Camera::SynLockOn(void)
{
	if (followTransform_ == nullptr || targetPos_ == nullptr) return;

	// 現在のプレイヤー座標とターゲット座標の取得
	const VECTOR& currentFollowPos = followTransform_->pos;
	VECTOR targetCenterPos = *targetPos_;

	// プレイヤーの移動量を計測
	VECTOR playerMoveVec = VSub(currentFollowPos, prevFollowPos_);
	playerMoveVec.y = 0.0f;
	float moveSpeed = VSize(playerMoveVec);
	prevFollowPos_ = currentFollowPos;

	bool isPlayerMoving = (moveSpeed > PLAYER_MOVE_THRESHOLD);

	// 仮想のプレイヤー基準点を追従させる
	delayedFollowPos_ = AsoUtility::Lerp(
		delayedFollowPos_, currentFollowPos, LOCKON_PIVOT_FOLLOW_RATE);

	// 目標となる注視点を計算
	VECTOR idealFixationPos = AsoUtility::Lerp(
		delayedFollowPos_, targetCenterPos, LOCKON_LOOKAT_WEIGHT);

	// 現在の注視点を目標注視点へ遅れて移動させる
	fixationPos_ = AsoUtility::Lerp(
		fixationPos_, idealFixationPos, LOCKON_FIXATION_LERP_RATE);

	// 正面にとらえるための目標角度を計算
	VECTOR toTargetDir = VSub(targetCenterPos, delayedFollowPos_);
	if (!AsoUtility::EqualsVZero(toTargetDir))
	{
		toTargetDir = VNorm(toTargetDir);
		float targetAngleY = atan2f(toTargetDir.x, toTargetDir.z);
		Quaternion targetRotY = Quaternion::AngleAxis(targetAngleY, AsoUtility::AXIS_Y);

		float rotLerpRate = isPlayerMoving ? LOCKON_ROT_RATE_MOVING : LOCKON_ROT_RATE_STOPPED;
		rotY_ = Quaternion::Slerp(rotY_, targetRotY, rotLerpRate);

		// X軸もロックオン用の見やすい角度へ緩やかに補間
		angles_.x = AsoUtility::Lerp(angles_.x, LOCKON_DEFAULT_ANGLE_X, rotLerpRate);
		Quaternion targetQuaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));
		transform_.quaRot = Quaternion::Slerp(transform_.quaRot, targetQuaRot, rotLerpRate);

		// ロックオン解除時にカメラがガクッと飛ばないよう、現在のY角度をangles_.yにも同期しておく
		VECTOR forward = transform_.quaRot.PosAxis(AsoUtility::DIR_F);
		angles_.y = atan2f(forward.x, forward.z);
	}

	// カメラ本体の座標も、遅延させた基準点の背後へ遅れて追従させる
	VECTOR localPos = transform_.quaRot.PosAxis(LOCKON_CAMERA_LOCAL_POS);
	VECTOR nextCameraPos = VAdd(delayedFollowPos_, localPos);
	transform_.pos = AsoUtility::Lerp(
		transform_.pos, nextCameraPos, LOCKON_POS_LERP_RATE);
}

void Camera::ProcessRot(bool isLimit)
{
	// 方向回転によるXYZの移動(キーボード)
	RotKeyboard(isLimit);
	// 方向回転によるXYZの移動(ゲームパッド)
	RotGamePad(isLimit);
}

void Camera::ProcessMove(void)
{
	auto& ins = InputManager::GetInstance();

	VECTOR moveDir = AsoUtility::VECTOR_ZERO;

	if (GetJoypadNum() == 0)
	{
		if (ins.IsNew(KEY_INPUT_UP)) { moveDir = AsoUtility::DIR_F; }
		if (ins.IsNew(KEY_INPUT_DOWN)) { moveDir = AsoUtility::DIR_B; }
		if (ins.IsNew(KEY_INPUT_LEFT)) { moveDir = AsoUtility::DIR_L; }
		if (ins.IsNew(KEY_INPUT_RIGHT)) { moveDir = AsoUtility::DIR_R; }
	}
	else
	{
		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// 左スティックの傾き
		moveDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
	}

	// 移動処理
	if (!AsoUtility::EqualsVZero(moveDir))
	{
		// 現在の向きからの進行方向を取得
		VECTOR direction = VNorm(transform_.quaRot.PosAxis(moveDir));

		// 移動させたい方向に移動量をかける(=移動量)
		VECTOR movePow = VScale(direction, SPEED);

		// カメラ位置も注視点も移動させる
		transform_.pos = VAdd(transform_.pos, movePow);
		fixationPos_ = VAdd(fixationPos_, movePow);
	}
}

void Camera::SetBeforeDrawFixedPoint(void) {}

void Camera::SetBeforeDrawFree(void)
{
	// カメラ操作(回転)
	ProcessRot(false);

	// カメラ操作(移動)
	ProcessMove();

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	// 注視点更新
	fixationPos_ = VAdd(transform_.pos, transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS));
}

void Camera::SetBeforeDrawFollow(void)
{
	if (!isMouseInput_)return;

	// カメラ操作(回転)
	ProcessRot(true);

	// 追従対象との相対位置を同期
	SyncFollow();

	// 衝突判定
	Collision();
}

void Camera::SetBeforeDrawTargetLockeOn(void)
{
	// 追従対象との相対位置を同期
	SynLockOn();

	// 衝突判定
	Collision();
}

void Camera::RotKeyboard(bool isLimit)
{
	auto& ins = InputManager::GetInstance();

	auto mouseMove = ins.GetMousePosDistance();
	if (mouseMove.x != 0.0f || mouseMove.y != 0.0f)
	{
		angles_.x += mouseMove.y * MROT_POW_RAD;
		angles_.y += mouseMove.x * MROT_POW_RAD;
	}

	if (SceneManager::GetInstance().GetSceneID() == SceneManager::SCENE_ID::GAME
		&& isMouseInput_)
	{
		ins.SetMousePos(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);
	}

	// 角度制限
	if (isLimit && angles_.x < -LIMIT_X_DW_RAD)
	{
		angles_.x = -LIMIT_X_DW_RAD;
	}
	if (isLimit && angles_.x > LIMIT_X_UP_RAD)
	{
		angles_.x = LIMIT_X_UP_RAD;
	}
}

void Camera::RotGamePad(bool isLimit)
{
	auto& ins = InputManager::GetInstance();

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 右スティックの傾き
	VECTOR dir = ins.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);

	// 右スティック左右の傾き
	angles_.y += dir.x * ROT_POW_RAD;

	// 右スティック上下の傾き
	angles_.x -= dir.z * ROT_POW_RAD;

	// 角度制限
	if (isLimit && angles_.x < -LIMIT_X_DW_RAD)
	{
		angles_.x = -LIMIT_X_DW_RAD;
	}
	if (isLimit && angles_.x > LIMIT_X_UP_RAD)
	{
		angles_.x = LIMIT_X_UP_RAD;
	}
}
