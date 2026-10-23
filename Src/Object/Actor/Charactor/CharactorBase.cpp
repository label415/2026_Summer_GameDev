#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ColliderUtility.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/Collider/ColliderLine.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../Stage/Stage.h"
#include "CharactorBase.h"

CharactorBase::CharactorBase(void)
	: ActorBase()
	, targetPos_(nullptr)
	, uiHp_(nullptr)
	, animationController_(nullptr)
	, moveDir_(AsoUtility::VECTOR_ZERO)
	, movePow_(AsoUtility::VECTOR_ZERO)
	, prevPos_(AsoUtility::VECTOR_ZERO)
	, jumpPow_(AsoUtility::VECTOR_ZERO)
	, moveSpeed_(0.0f)
	, stepJump_(0.0f)
	, isJump_(false)
{
	weapons_.clear();
}

CharactorBase::~CharactorBase(void) {}

void CharactorBase::Update(void)
{
	// 生存していなければ更新しない
	if (!isAlive_)return;

	// 移動前座標を更新
	prevPos_ = transform_.pos;

	// 各キャラクターごとの更新処理
	UpdateProcess();

	// 移動方向に応じた遅延回転
	DelayRotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	// モデル制御更新
	transform_.Update();

	// アニメーション再生
	animationController_->Update();

	// 各キャラクターごとの更新後処理
	UpdateProcessPost();
}

void CharactorBase::Draw(void)
{
	// 基底クラスの描画処理
	ActorBase::Draw();

	// 武器描画
	for (const auto& weapon : weapons_) {
		weapon->Draw();
	}
}

void CharactorBase::Release(void)
{
	// 基底クラスの解放処理
	transform_.Release();

	// 各ポインタ変数開放
	animationController_->Release();

	// 武器の解放処理
	for (const auto& weapon : weapons_) {
		weapon->Release();
	}
	weapons_.clear();
}

VECTOR CharactorBase::GetTargetDir(void)
{
	//自分と相手のベクトルを計算
	VECTOR targetVec = VSub(*targetPos_, transform_.pos);
	//ベクトルを正規化
	VECTOR targetdir = VNorm(targetVec);
	//Y座標を無効
	targetdir.y = 0.0f;

	return targetdir;
}

void CharactorBase::DelayRotate(void)
{
	// 移動方向から回転に変換する
	Quaternion goalRot = Quaternion::LookRotation(moveDir_);
	// 回転の補間
	transform_.quaRot =
		Quaternion::Slerp(transform_.quaRot, goalRot, 0.2f);
}

void CharactorBase::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;
	// 重力の強さ
	float gravityPow = Application::GetInstance().GetGravityPow() *
		scnMng_.GetDeltaTime();
	// 重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 重力速度の制限
	if (jumpPow_.y < MAX_FALL_SPEED)
	{
		jumpPow_.y = MAX_FALL_SPEED;
	}
}

void CharactorBase::Collision(void)
{
	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow_);

	// 衝突(カプセル)
	CollisionCapsule();

	// ジャンプ量を加算
	transform_.pos = VAdd(transform_.pos, jumpPow_);

	// 衝突(重力)
	CollisionGravity();

	// 衝突判定後の更新処理
	UpdateHitCollider();
}

void CharactorBase::CollisionGravity(void)
{
	// 落下中しか判定しない
	if (!(VDot(AsoUtility::DIR_D, jumpPow_) > 0.9f))return;

	int lineType = static_cast<int>(ColliderBase::SHAPE::LINE);
	if (ownColliders_.count(lineType) == 0) return;

	for (const auto& col : ownColliders_.at(lineType))
	{
		if (!col->GetIsCollier()) continue;

		// カプセルコライダ情報  
		std::shared_ptr<ColliderLine> colliderLine =
			std::dynamic_pointer_cast<ColliderLine>(col);
		if (colliderLine == nullptr) return;

		for (const auto& hit : col->GetHitColliders())
		{
			if (hit.lock()->GetIsCollier()
				&& hit.lock()->GetTag() == ColliderBase::TAG::STAGE)
			{
				std::shared_ptr<ColliderModel> colliderModel =
					std::dynamic_pointer_cast<ColliderModel>(hit.lock());
				if (colliderModel == nullptr) continue;

				// 地面押し上げ処理
				transform_.pos =
					CollisionUtility::CalcPushUpLineModel(
						*colliderLine, *colliderModel,
						transform_.pos.y, 2.0f,
						false, true);

				isJump_ = false;
				jumpPow_ = AsoUtility::VECTOR_ZERO;
				stepJump_ = 0.0f;
			}
		}
	}
}

void CharactorBase::CollisionCapsule(void)
{
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	if (ownColliders_.count(capsuleType) == 0) return;

	for (const auto& col : ownColliders_.at(capsuleType))
	{
		if (!col->GetIsCollier()) continue;

		// カプセルコライダ情報  
		std::shared_ptr<ColliderCapsule> colliderCapsule =
			std::dynamic_pointer_cast<ColliderCapsule>(col);
		if (colliderCapsule == nullptr) return;

		for (const auto& hit : col->GetHitColliders())
		{
			// モデル以外は処理を飛ばす  
			if (hit.lock()->GetShape() != ColliderBase::SHAPE::MODEL) continue;

			std::shared_ptr<ColliderModel> colliderModel =
				std::dynamic_pointer_cast<ColliderModel>(hit.lock());
			if (colliderModel == nullptr) continue;

			transform_.pos =
				CollisionUtility::CalcPushCapsuleModel(
					*colliderCapsule, *colliderModel, transform_.pos,
					CNT_TRY_COLLISION, COLLISION_BACK_DIS,
					true, false);
		}
	}
}

void CharactorBase::SetFrameUserLocalPos(VECTOR locakPos, int frameNo)
{
	// 対象フレームのローカル行列を初期値にリセットする
	MV1ResetFrameUserLocalMatrix(transform_.modelId, frameNo);
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(transform_.modelId, frameNo);
	auto scl = MGetSize(mat);
	// 行列から大きさを取り出す
	auto rot = MGetRotElem(mat);
	auto pos = MGetTranslateElem(mat);
	// 行列から回転を取り出す
	// 行列から移動値を取り出す
	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale(scl)); // 大きさ
	mix = MMult(mix, rot); // 回転
	// ここでローカル座標を行列に、そのまま戻さず、
	// 調整したローカル座標を設定する
	mix = MMult(mix, MGetTranslate(locakPos));
	// 合成した行列を対象フレームにセットし直して、
	// アニメーションの移動値を無効化
	MV1SetFrameUserLocalMatrix(transform_.modelId, frameNo, mix);
}

