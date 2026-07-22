#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
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
	// DxLibの初期設定では、
	// カメラの位置が x = 320.0f, y = 240.0f, z = (画面のサイズによって変化)、
	// 注視点の位置は x = 320.0f, y = 240.0f, z = 1.0f
	// カメラの上方向は x = 0.0f, y = 1.0f, z = 0.0f
	// 右上位置からZ軸のプラス方向を見るようなカメラ
	angleY = 0.0f;
}

Camera::~Camera(void)
{
}

void Camera::Update(void)
{
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
		break;
	case Camera::MODE::TARGET_ROCKE:
		SetBeforeDrawTargetLockeOn();
		break;
	}

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		transform_.pos, 
		targetPos_, 
		transform_.quaRot.GetUp()
	);

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

}

void Camera::DrawDebug(void)
{
}

void Camera::Release(void)
{
}

void Camera::AddHitCollider(int shape, const std::vector<ColliderBase*> hitCollider)
{
	for (const auto& c : hitColliders_)
	{
		if (c.second == hitCollider)
		{
			return;
		}
	}
	hitColliders_.emplace(shape, hitCollider);
}

void Camera::InitCollider(void)
{
	// 主に地面との衝突で使用する球体コライダ
	ColliderSphere* colliderSphere = new ColliderSphere(
		ColliderBase::TAG::CAMERA,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_SPHERE
	);

	std::vector<ColliderBase*> colliderSpheres;
	colliderSpheres.push_back(colliderSphere);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::SPHERE), colliderSpheres);
}

void Camera::InitPost(void)
{
	ChangeMode(MODE::FIXED_POINT);
	auto& ins = InputManager::GetInstance();
	mouseX = ins.GetMousePos().x;
	mouseY = ins.GetMousePos().y;
	preMouseX = ins.GetMousePos().x;
	preMouseY = ins.GetMousePos().y;
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

	// カメラの初期設定
	SetDefault();

	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::FREE:
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

void Camera::SetDefault(void)
{

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
	if(angleY != 0.0f){
		angles_.y = angleY;
		angleY = 0.0f;
	}
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;
	const float MOVE_SPEED = 0.5f;
	// 注視点
	localPos = transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS);
	VECTOR nextTargetPos = VAdd(pos, localPos);
	targetPos_ = AsoUtility::Lerp(transform_.pos, nextTargetPos,  MOVE_SPEED);

	// カメラ位置
	localPos = transform_.quaRot.PosAxis(FOLLOW_CAMERA_LOCAL_POS);
	VECTOR nextCameraPos = VAdd(pos, localPos);
	transform_.pos = AsoUtility::Lerp(transform_.pos, nextCameraPos, MOVE_SPEED);

}

void Camera::SynLockOn(void)
{
	const float ROT_SPEED = 0.1f;
	const float MOVE_SPEED = 0.05f;

	// 同期先の位置
	VECTOR followPos = followTransform_->pos;
	VECTOR TargetPos = *targetTransform_;
	VECTOR toTarget = VSub(TargetPos, followPos);

	float distanceSq = (toTarget.x * toTarget.x) + (toTarget.z * toTarget.z);
	if (distanceSq > 200.0f){

		angleY = atan2f(toTarget.x, toTarget.z);
		angles_.y = angleY;

		Quaternion targetRotY = Quaternion::AngleAxis(angleY, AsoUtility::AXIS_Y);
		rotY_ = Quaternion::Slerp(rotY_, targetRotY, ROT_SPEED);
	}

	// Y軸 + X軸
	Quaternion targetQuaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));
	// クォータニオン補間
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, targetQuaRot, ROT_SPEED);

	VECTOR localPos;
	// 注視点
	localPos = transform_.quaRot.PosAxis(LOCKON_TARGET_LOCAL_POS);
	VECTOR nextTargetPos = VAdd(TargetPos, localPos);
	targetPos_ = AsoUtility::Lerp(transform_.pos, nextTargetPos, MOVE_SPEED);

	// カメラ位置
	localPos = transform_.quaRot.PosAxis(LOCKON_CAMERA_LOCAL_POS);
	VECTOR nextCameraPos = VAdd(followPos, localPos);
	transform_.pos = AsoUtility::Lerp(transform_.pos, nextCameraPos, MOVE_SPEED);

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

		// 移動させたい方向(ベクトル)に変換

		// 現在の向きからの進行方向を取得
		VECTOR direction = VNorm(transform_.quaRot.PosAxis(moveDir));

		// 移動させたい方向に移動量をかける(=移動量)
		VECTOR movePow = VScale(direction, SPEED);

		// カメラ位置も注視点も移動させる
		transform_.pos = VAdd(transform_.pos, movePow);
		targetPos_ = VAdd(targetPos_, movePow);

	}

}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// 何もしない
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
	targetPos_ = VAdd(transform_.pos, transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS));
}

void Camera::SetBeforeDrawFollow(void)
{

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

	// カメラ位置の補間
	if (isCameraLope_) {
		transform_.pos =
			AsoUtility::Lerp(prePos_, transform_.pos, LERP_RATE_MOVE);
	}
}

void Camera::Collision(void)
{
	// プレイヤーのルートフレーム
	VECTOR start = MV1GetFramePosition(followTransform_->modelId, 1);

	for (const auto& hitCol : hitColliders_)
	{
		for (const auto& i : hitCol.second)
		{
			// モデル以外は処理を飛ばす
			if (i->GetShape() != ColliderBase::SHAPE::MODEL) continue;

			// 派生クラスへキャスト
			const ColliderModel* colliderModel =
				dynamic_cast<const ColliderModel*>(i);

			if (colliderModel == nullptr) continue;

			// 線分で衝突判定
			auto hits = MV1CollCheck_LineDim(
				colliderModel->GetFollow()->modelId,
				-1,
				transform_.pos,
				start
			);

			// 追従対象に一番近い衝突点を探す
			bool isCollision_ = false;
			isCameraLope_ = false;
			MV1_COLL_RESULT_POLY hitPoly;
			double minDist = DBL_MAX;
			for (int i = 0; i < hits.HitNum; i++)
			{
				const auto& hit = hits.Dim[i];

				// 対象フレーム以外は無視する
				if (!colliderModel->IsTargetFrame(hit.FrameIndex))
				{
					continue;
				}

				// 衝突判定
				isCollision_ = true;
				isCameraLope_ = true;

				// 距離判定
				double dist = AsoUtility::Distance(start, hit.HitPosition);
				if (minDist > dist)
				{
					// 追従対象に一番近い衝突点を優先
					minDist = dist;
					hitPoly = hit;
				}
			}

			// 検出した地面ポリゴン情報の後始末
			MV1CollResultPolyDimTerminate(hits);

			if (!isCollision_)
			{
				// 衝突していなければ次のコライダへ
				continue;
			}

			// カメラ位置から注視点への方向
			VECTOR dirToTarget = VNorm(VSub(targetPos_, transform_.pos));

			// 衝突点の少し手前にカメラを置く
			transform_.pos =
				VAdd(hitPoly.HitPosition, VScale(dirToTarget, COLLISION_BACK_DIS));

			// カメラ位置の球体コライダ
			int typeSphere = static_cast<int>(ColliderBase::SHAPE::SPHERE);

			// 球体コライダが無ければ処理を抜ける
			if (ownColliders_.count(typeSphere) == 0) continue;

			const auto& vecs = ownColliders_.at(typeSphere);
			for (const auto& vec : vecs)
			{
				// 指定された回数と距離で三角形の法線方向に押し戻す
				transform_.pos =
					vec->GetPosPushBackAlongNormal(
						hitPoly, CNT_TRY_COLLISION_CAMERA, COLLISION_BACK_DIS);
			}
		}
	}
}

void Camera::RotKeyboard(bool isLimit)
{

	auto& ins = InputManager::GetInstance();

	auto mouseMove = ins.GetMousePosDistance();
	if (mouseMove.x != 0.0f || mouseMove.y != 0.0f) // より明示的にチェック
	{
		angles_.x += mouseMove.y * MROT_POW_RAD;
		angles_.y += mouseMove.x * MROT_POW_RAD;
	}

	if(SceneManager::GetInstance().GetSceneID() == SceneManager::SCENE_ID::GAME)
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
