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
	targetTransform_(nullptr),
	mode_(MODE::NONE),
	angles_(AsoUtility::VECTOR_ZERO),
	rotY_(Quaternion::Identity()),
	targetPos_(AsoUtility::VECTOR_ZERO)
{
	// カメラの初期設定
	angleY = 0.0f;
}

Camera::~Camera(void) {}

void Camera::Update(void)
{
	Collision();
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// 更新前情報
	prePos_ = transform_.pos;

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

	// カメラの設定
	if (mode_ == Camera::MODE::TARGET_ROCKE)
	{
		// ロックオン時は常に頭上を上として扱うことで画面の傾きを防ぐ
		SetCameraPositionAndTarget_UpVecY(transform_.pos, targetPos_);
	}
	else {
		// 通常カメラ時は既存の Up ベクトルを使用
		SetCameraPositionAndTargetAndUpVec(
			transform_.pos,
			targetPos_,
			transform_.quaRot.GetUp()
		);
	}

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();
}

void Camera::DrawDebug(void)
{
	// ロックオンUI描画
	if (mode_ != MODE::TARGET_ROCKE) return;

	VECTOR enemyTransform = targetTransform_->pos;
	SetUseZBuffer3D(FALSE);
	DrawBillboard3D(
		enemyTransform,
		0.5f, 0.5f,
		LOCON_UI_SIZE, 0.0f,
		lockOnImg_, true);
	SetUseZBuffer3D(TRUE);
}

void Camera::Release(void) {}

void Camera::InitLoad(void){}

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
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	if (ownColliders_.count(capsuleType) == 0) return;

	for (const auto& col : ownColliders_.at(capsuleType))
	{
		if (!col->GetIsCollier()) continue;

		// カプセルコライダ情報  
		std::shared_ptr<ColliderSphere> colliderSphere =
			std::dynamic_pointer_cast<ColliderSphere>(col);
		if (colliderSphere == nullptr) return;

		for (const auto& hit : col->GetHitColliders())
		{
			// モデル以外は処理を飛ばす  
			if (hit.lock()->GetShape() != ColliderBase::SHAPE::MODEL) continue;

			std::shared_ptr<ColliderModel> colliderModel =
				std::dynamic_pointer_cast<ColliderModel>(hit.lock());

			if (colliderModel == nullptr) continue;

			transform_.pos =
				CollisionUtility::CalcPushSphereModel(
					*colliderSphere, *colliderModel, transform_.pos,
					CNT_TRY_COLLISION_CAMERA, COLLISION_BACK_DIS,
					false, false);
		}
	}
}

const Transform* Camera::FindLockOnTarget(SWITCH_DIR switchDir) const
{
	const VECTOR& playerPos = followTransform_->pos;
	VECTOR camForward = VNorm(GetForward());
	float maxAngleRad = AsoUtility::Deg2RadF(LOCKON_VIEW_ANGLE);
	float diffMin = (switchDir == SWITCH_DIR::UP) ? FLT_MAX : MAX_LOCKON_DIFF;

	const Transform* bestTarget = nullptr;

	for (const Transform* candidate : targetTransforms_) {
		// 早期リターン
		if (candidate == nullptr || candidate == targetTransform_) continue;

		VECTOR toEnemy = VSub(candidate->pos, playerPos);
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
		bestTarget = candidate;
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
		const Transform* nextTarget = FindLockOnTarget(switchDir);
		if (nextTarget != nullptr) {
			targetTransform_ = nextTarget;
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
	return targetPos_;
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
	return VNorm(VSub(targetPos_, transform_.pos));
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

void Camera::AddTargetTransform(const Transform* target)
{
	if (target == nullptr) return;

	// すでに同じTransformが格納されていなければ追加
	if (std::find(targetTransforms_.begin(),
		targetTransforms_.end(),
		target) == targetTransforms_.end())
	{
		targetTransforms_.push_back(target);
	}
}

void Camera::UpdateLockOn(void)
{
	// 早期リターン：追従対象（プレイヤー）がいなければ処理しない
	if (followTransform_ == nullptr) return;

	auto& inp = InputManager::GetInstance();
	bool isLockOnTrg = inp.IsTrgMouseMiddle()
		|| inp.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER);

	// ロックオン中の処理
	if (mode_ == MODE::TARGET_ROCKE)
	{
		// 早期リターン：ターゲットが喪失している、または解除条件を満たした場合
		if (targetTransform_ == nullptr ||
			VSize(VSub(targetTransform_->pos, followTransform_->pos)) >= MAX_LOCKON_DIFF ||
			isLockOnTrg)
		{
			targetTransform_ = nullptr;
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
		const Transform* newTarget = FindLockOnTarget(SWITCH_DIR::NONE);
		if (newTarget != nullptr) {
			targetTransform_ = newTarget;
			ChangeMode(MODE::TARGET_ROCKE);
		}
	}
}

void Camera::SetDefault(void)
{
	// 毎フレーム補間フラグをリセットする
	isCameraLope_ = false;

	// カメラの初期設定
	transform_.pos = DERFAULT_POS;

	// カメラ角
	angles_ = DERFAULT_ANGLES;
	transform_.quaRot = Quaternion::Identity();

	// 注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;
}

void Camera::SyncFollow(void)
{
	// 同期先の位置
	VECTOR pos = followTransform_->pos;

	// Y軸
	if (angleY != 0.0f) {
		angles_.y = angleY;
		angleY = 0.0f;
	}
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;
	// 注視点
	localPos = transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS);
	VECTOR nextTargetPos = VAdd(pos, localPos);
	targetPos_ = AsoUtility::Lerp(transform_.pos, nextTargetPos, MOVE_SPEED);

	// カメラ位置
	localPos = transform_.quaRot.PosAxis(FOLLOW_CAMERA_LOCAL_POS);
	VECTOR nextCameraPos = VAdd(pos, localPos);
	transform_.pos = AsoUtility::Lerp(transform_.pos, nextCameraPos, MOVE_SPEED);
}

void Camera::SynLockOn(void)
{
	// 各位置の取得
	VECTOR followPos = followTransform_->pos;
	VECTOR TargetPos = targetTransform_->pos;

	// 注視点の高さを調整
	VECTOR targetCenterPos = TargetPos;
	targetCenterPos.y += TARGET_CENTER_POS;

	// プレイヤーとボスの距離
	VECTOR toEnemy = VSub(targetCenterPos, followPos);
	float distance = VSize(toEnemy);

	// ボス側基準のズーム
	float baseDist = LOCKON_BOSS_ZOOM;
	float bossFitDist = baseDist + (distance * LOCKON_BOSS_ZOOM_LERP_RATE);

	// プレイヤー側基準のズーム
	float halfHeight = PLAYER_HEIGHT * LOCKON_BOSS_ZOOM_LERP_RATE + PLAYER_MARGIN;
	float playerFitDist = halfHeight / tanf(AsoUtility::Deg2RadF(FOV_Y) * LOCKON_BOSS_ZOOM_LERP_RATE);

	// 両方満たす距離を採用
	float dynamicDist = fmaxf(bossFitDist, playerFitDist);
	dynamicDist = fminf(dynamicDist, LOCKON_BOSS_ZOOM_MAX);

	float baseHeight = LOCKON_BOSS_HEIGHT_ADJUST;
	float dynamicHeight = baseHeight + (distance * LOCKON_BOSS_HEIGHT_LERP_RATE);

	// 注視点
	VECTOR lookAtPoint = AsoUtility::Lerp(followPos, targetCenterPos, LOCKON_TARGET_LERP_RATE);

	// カメラから見た目標の向き
	VECTOR toTargetDir = VNorm(VSub(lookAtPoint, followPos));
	float angleY = atan2f(toTargetDir.x, toTargetDir.z);
	Quaternion targetRotY = Quaternion::AngleAxis(angleY, AsoUtility::AXIS_Y);
	rotY_ = Quaternion::Slerp(rotY_, targetRotY, ROT_SPEED);

	Quaternion targetQuaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, targetQuaRot, ROT_SPEED);

	// 動的な距離と高さを使ってカメラ位置と注視点を確定
	VECTOR backVec = transform_.quaRot.PosAxis(VGet(0.0f, 0.0f, -1.0f));
	transform_.pos = VAdd(followPos, VScale(backVec, dynamicDist));
	transform_.pos.y += dynamicHeight;

	targetPos_ = lookAtPoint;

	// カメラの角度を更新
	VECTOR forward = transform_.quaRot.PosAxis(VGet(0.0f, 0.0f, 1.0f));
	angles_.y = atan2f(forward.x, forward.z);
	float horizontalLen = sqrtf(forward.x * forward.x + forward.z * forward.z);
	angles_.x = atan2f(-forward.y, horizontalLen);

	prePos_ = transform_.pos;
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
		targetPos_ = VAdd(targetPos_, movePow);
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
	targetPos_ = VAdd(transform_.pos, transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS));
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

	// カメラ位置の補間
	if (isCameraLope_) {
		transform_.pos =
			AsoUtility::Lerp(prePos_, transform_.pos, LERP_RATE_MOVE);
	}
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
