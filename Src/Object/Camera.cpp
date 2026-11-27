#include <algorithm>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include "../Utility/CollisionUtility.h"
#include "../Application.h"
#include "../Manager/Common/ResourceManager.h"
#include "../Manager/Common/InputManager.h"
#include "../Manager/Common/SceneManager.h"
#include "../Libs/ImGui/imgui.h"
#include "Common/Transform.h"
#include "Common/Collider/ColliderCapsule.h"
#include "Common/Collider/ColliderModel.h"
#include "Common/Collider/ColliderSphere.h"
#include "Camera.h"

Camera::Camera(void)
	: mode_(MODE::NONE)
	, angles_(AsoUtility::VECTOR_ZERO)
	, rotY_(Quaternion::Identity())
	, fixationPos_(AsoUtility::VECTOR_ZERO)
	, prevStick_(AsoUtility::VECTOR2F_ZERO)
	, lockOnImg_(-1)
{
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// ポーズ中はカメラのモード別更新を停止する
	if (SceneManager::GetInstance().GetSceneID() != SceneManager::SCENE_ID::PAUSE)
	{
		// カメラモードごとの処理
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
		case Camera::MODE::TARGET_LOCK:
			SetBeforeDrawTargetLockOn();
			break;
		}
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
	if (mode_ != MODE::TARGET_LOCK) return;
	auto target = targetCollider_.lock();
	if (!target) return;

	// 3D描画時のZバッファを無効化して、ロックオンUIを描画する
	SetUseZBuffer3D(FALSE);
	DrawBillboard3D(
		target->GetCenter(),
		LOCKON_UI_CENTER.x, LOCKON_UI_CENTER.y,
		LOCKON_UI_SIZE, 0.0f,
		lockOnImg_, true);
	SetUseZBuffer3D(TRUE);
}

void Camera::Release(void)
{
	// 追従対象・ロックオン対象および候補一覧を解放
	followActor_.reset();
	targetCollider_.reset();
	targetColliders_.clear();
}

void Camera::ClearTargetColliders(void)
{
	// ロックオン候補リストのみクリア
	targetColliders_.clear();
}

void Camera::InitLoad(void)
{
	// シーン切り替えでリソースが解放されるため再読み込み
	lockOnImg_ = resMng_.Load(ResourceManager::SRC::UI_LOCKON).handleId_;
}

void Camera::InitCollider(void)
{
	// 衝突対象：ステージモデル
	std::vector<ColliderBase::TAG> targetTags = {
		ColliderBase::TAG::STAGE
	};

	// 球体コライダーを生成して登録
	auto colliderSphere = std::make_shared<ColliderSphere>(
		ColliderBase::TAG::CAMERA,
		targetTags,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		COL_SPHERE_RADIUS
	);

	// コライダーを所有コライダーに登録
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::SPHERE)].push_back(colliderSphere);
}

void Camera::InitPost(void)
{
	// カメラモードの初期化
	ChangeMode(MODE::FIXED_POINT);

	// ロックオン画像ロード
	lockOnImg_ = resMng_.Load(ResourceManager::SRC::UI_LOCKON).handleId_;
}

void Camera::UpdateHitCollider(void)
{
	// 追従対象がいなければ処理しない
	auto follow = followActor_.lock();
	if (!follow) return;

	// カメラ自身の球体コライダーリストを取得
	int sphereType = static_cast<int>(ColliderBase::SHAPE::SPHERE);
	if (ownColliders_.count(sphereType) == 0) return;

	// プレイヤーの座標を取得
	VECTOR startPos = follow->GetTransform().pos;
	for (const auto& col : ownColliders_.at(sphereType))
	{
		// コライダーが有効でなければ処理しない
		if (!col->GetIsCollider()) continue;

		// 形状はSPHEREと確定しているため static_pointer_cast でダウンキャスト
		auto colliderSphere = std::static_pointer_cast<ColliderSphere>(col);
		if (colliderSphere == nullptr) continue;

		// 衝突したコライダーを取得して押し戻し計算
		for (const auto& hit : col->GetHitColliders())
		{
			// weak_ptr を1度だけ lock() して有効性を確認
			auto hitCol = hit.lock();

			// 衝突したコライダーが有効でなければ処理しない
			if (!hitCol || hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

			// 衝突したコライダー
			const auto* colliderModel = static_cast<const ColliderModel*>(hitCol.get());

			// カメラの座標を押し戻す
			transform_.pos = CollisionUtility::CalcCameraPushBack(
				*colliderSphere, *colliderModel,
				startPos, transform_.pos,
				CNT_TRY_COLLISION_CAMERA,
				COLLISION_BACK_DIS,
				false,false
			);
		}
	}
}

std::weak_ptr<ColliderCapsule> Camera::FindLockOnTarget(SWITCH_DIR switchDir) const
{
	// 追従対象が存在しなければ空を返す
	auto follow = followActor_.lock();
	if (!follow) return {};

	// 基準位置・カメラ前方・最大許容視野角および探索距離を初期化
	const VECTOR& playerPos = follow->GetTransform().pos;
	VECTOR camForward = VNorm(GetForward());
	float maxAngleRad = AsoUtility::Deg2RadF(LOCKON_VIEW_ANGLE);
	float diffMin = (switchDir == SWITCH_DIR::UP) ? FLT_MAX : MAX_LOCKON_DIFF;

	auto currentTarget = targetCollider_.lock();
	std::weak_ptr<ColliderCapsule> bestTarget;

	// 全ロックオン候補から視野角内かつ切り替え方向・最短距離のターゲットを選出
	for (const auto& candidateWeak : targetColliders_) {
		// 早期リターン
		auto candidate = candidateWeak.lock();
		if (!candidate || candidate == currentTarget) continue;

		// プレイヤーからの距離が現在の最短候補以上なら除外
		VECTOR toEnemy = VSub(candidate->GetCenter(), playerPos);
		float lockonDiff = VSize(toEnemy);
		if (lockonDiff >= diffMin) continue;

		// 視野角判定
		float dot = VDot(camForward, VNorm(toEnemy));
		dot = std::clamp(dot, -1.0f, 1.0f);
		if (acosf(dot) >= maxAngleRad) continue;

		// 左右切り替え時の外積判定
		if (switchDir == SWITCH_DIR::LEFT 
			&& VCross(camForward, toEnemy).y > 0.0f) continue;
		if (switchDir == SWITCH_DIR::RIGHT 
			&& VCross(camForward, toEnemy).y < 0.0f) continue;

		diffMin = lockonDiff;
		bestTarget = candidate;
	}

	return bestTarget;
}

void Camera::HandleTargetSwitching(void)
{
	// 右スティックおよびマウスホイールの入力を取得
	auto& inp = InputManager::GetInstance();
	InputManager::JOYPAD_IN_STATE padState =
		inp.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
	VECTOR dir = inp.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);
	const float stickThreshold = InputManager::THRESHOLD;

	// 前フレームとのエッジ比較で上下左右の切り替えトリガを判定
	bool isNextUp = (dir.z > stickThreshold)
		&& (prevStick_.y <= stickThreshold);
	bool isNextDown = (dir.z < -stickThreshold)
		&& (prevStick_.y >= -stickThreshold);
	bool isNextRight = ((dir.x > stickThreshold)
		&& (prevStick_.x <= stickThreshold))
		|| (inp.GetMouseWheelRot() > 0.0f);
	bool isNextLeft = ((dir.x < -stickThreshold)
		&& (prevStick_.x >= -stickThreshold))
		|| (inp.GetMouseWheelRot() < 0.0f);

	// 次フレーム判定用にスティック状態を保存
	prevStick_ = { dir.x, dir.z };

	// 入力された切り替え方向を決定
	SWITCH_DIR switchDir = SWITCH_DIR::NONE;
	if (isNextUp)         switchDir = SWITCH_DIR::UP;
	else if (isNextDown)  switchDir = SWITCH_DIR::DOWN;
	else if (isNextLeft)  switchDir = SWITCH_DIR::LEFT;
	else if (isNextRight) switchDir = SWITCH_DIR::RIGHT;

	// 切り替え方向が入力されていれば次のターゲットを検索して差し替え
	if (switchDir != SWITCH_DIR::NONE) {
		auto nextTarget = FindLockOnTarget(switchDir);
		if (!nextTarget.expired()) {
			targetCollider_ = nextTarget;
		}
	}
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
	case Camera::MODE::TARGET_LOCK:
		break;
	}
}

void Camera::AddTargetCollider(const std::weak_ptr<ColliderCapsule>& target)
{
	auto newTarget = target.lock();
	if (!newTarget) return;

	// 期限切れの候補をリストから除外する
	targetColliders_.erase(
		std::remove_if(targetColliders_.begin(), targetColliders_.end(),
			[](const std::weak_ptr<ColliderCapsule>& wp) {
				return wp.expired();
			}),
		targetColliders_.end()
	);

	// すでに同じコライダーが格納されていなければ追加
	auto it = std::find_if(targetColliders_.begin(), targetColliders_.end(),
		[&newTarget](const std::weak_ptr<ColliderCapsule>& wp) {
			return wp.lock() == newTarget;
		});
	if (it == targetColliders_.end())
	{
		targetColliders_.push_back(newTarget);
	}
}

const VECTOR* Camera::GetLockOnTargetPos(void) const
{
	// ロックオン対象が有効ならその中心座標ポインタを返す
	if (auto target = targetCollider_.lock())
	{
		return &target->GetCenter();
	}
	return nullptr;
}

void Camera::UpdateLockOn(void)
{
	// 期限切れのロックオン候補をリストから除外する
	targetColliders_.erase(
		std::remove_if(targetColliders_.begin(), targetColliders_.end(),
			[](const std::weak_ptr<ColliderCapsule>& wp) {
				return wp.expired();
			}),
		targetColliders_.end()
	);

	// 早期リターン
	auto follow = followActor_.lock();
	if (!follow)
	{
		if (mode_ == MODE::FOLLOW || mode_ == MODE::TARGET_LOCK)
		{
			targetCollider_.reset();
			ChangeMode(MODE::FIXED_POINT);
		}
		return;
	}

	auto& inp = InputManager::GetInstance();
	bool isLockOnTrg = inp.IsTrgMouseMiddle()
		|| inp.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER);

	// ロックオン中の処理
	if (mode_ == MODE::TARGET_LOCK)
	{
		auto target = targetCollider_.lock();
		// 早期リターン
		if (!target ||
			VSize(VSub(target->GetCenter(), follow->GetTransform().pos)) >= MAX_LOCKON_DIFF ||
			isLockOnTrg)
		{
			targetCollider_.reset();
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
		auto newTarget = FindLockOnTarget(SWITCH_DIR::NONE);
		if (!newTarget.expired()) {
			targetCollider_ = newTarget;
			ChangeMode(MODE::TARGET_LOCK);
		}
	}
}

void Camera::SetDefault(void)
{
	// カメラの初期設定
	transform_.pos = DEFAULT_POS;

	// カメラ角
	angles_ = DEFAULT_ANGLES;
	transform_.quaRot = Quaternion::Identity();

	// 注視点
	fixationPos_ = AsoUtility::VECTOR_ZERO;

	// ロックオン情報をクリア
	// (前のシーンの敵を指したままにならないように)
	targetColliders_.clear();
	targetCollider_.reset();
}

void Camera::SyncFollow(void)
{
	// それぞれの対象が破棄されている時は処理しない
	auto follow = followActor_.lock();
	if (!follow) return;

	// 同期先の位置
	VECTOR pos = follow->GetTransform().pos;

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

void Camera::SyncLockOn(void)
{
	// それぞれの対象が破棄されている時は処理しない
	auto follow = followActor_.lock();
	auto target = targetCollider_.lock();
	if (!follow || !target)
	{
		targetCollider_.reset();
		ChangeMode(follow ? MODE::FOLLOW : MODE::FIXED_POINT);
		return;
	}

	// プレイヤーからターゲットへのベクトル
	VECTOR followPos = follow->GetTransform().pos;
	VECTOR targetPos = target->GetCenter();
	VECTOR toTarget = VSub(targetPos, followPos);

	// プレイヤーからターゲットへのベクトルの水平距離の二乗を計算
	float horizontalSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;
	if (horizontalSq > 1.0f)
	{
		angles_.y = atan2f(toTarget.x, toTarget.z);
	}

	// 回転を角度から生成
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	// 注視点
	VECTOR basePos = transform_.quaRot.PosAxis(LOCKON_TARGET_LOCAL_POS);
	VECTOR nextFixationPos = VAdd(followPos, VAdd(basePos, VScale(toTarget, CAMERA_LOCKON_RATE)));
	fixationPos_ = AsoUtility::Lerp(fixationPos_, nextFixationPos, CAMERA_LOCKON_RATE);

	// カメラ位置
	VECTOR localPos = transform_.quaRot.PosAxis(LOCKON_CAMERA_LOCAL_POS);
	VECTOR nextCameraPos = VAdd(followPos, localPos);
	transform_.pos = AsoUtility::Lerp(transform_.pos, nextCameraPos, CAMERA_LOCKON_RATE);
}

void Camera::ProcessRot(bool isLimit)
{
	// 方向回転によるXYZの移動(キーボード)
	RotMouse(isLimit);
	// 方向回転によるXYZの移動(ゲームパッド)
	RotGamePad(isLimit);
}

void Camera::ProcessMove(void)
{
	auto& ins = InputManager::GetInstance();
	VECTOR moveDir = AsoUtility::VECTOR_ZERO;

	// パッド未接続時は方向キー、接続時は左スティックから移動方向を取得
	if (GetJoypadNum() == 0)
	{
		if (ins.IsNew(KEY_INPUT_UP)) { moveDir = AsoUtility::DIR_F; }
		if (ins.IsNew(KEY_INPUT_DOWN)) { moveDir = AsoUtility::DIR_B; }
		if (ins.IsNew(KEY_INPUT_LEFT)) { moveDir = AsoUtility::DIR_L; }
		if (ins.IsNew(KEY_INPUT_RIGHT)) { moveDir = AsoUtility::DIR_R; }
	}
	else
	{
		// 接続されているゲームパッド１の情報を取得
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
	// 追従対象がいなければ固定視点に戻して処理しない
	if (followActor_.expired())
	{
		ChangeMode(MODE::FIXED_POINT);
		return;
	}

	// カメラ操作(回転)
	ProcessRot(true);

	// 追従対象との相対位置を同期
	SyncFollow();

	// 衝突判定
	Collision();
}

void Camera::SetBeforeDrawTargetLockOn(void)
{
	// 追従対象との相対位置を同期
	SyncLockOn();

	// 衝突判定
	Collision();
}

void Camera::RotMouse(bool isLimit)
{
	// マウスの移動量を取得
	auto& ins = InputManager::GetInstance();
	if (ins.GetMouseFlag()) return;
	auto mouseMove = ins.GetMousePosDistance();

	if (mouseMove.x != 0 || mouseMove.y != 0)
	{
		angles_.x += mouseMove.y * MROT_POW_RAD;
		angles_.y += mouseMove.x * MROT_POW_RAD;
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
