#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/CollisionUtility.h"
#include "../../../Manager/Common/SceneManager.h"
#include "../../../Manager/Common/ResourceManager.h"
#include "../../Common/Collider/ColliderLine.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../Stage/Stage.h"
#include "CharacterBase.h"

CharacterBase::CharacterBase(void)
	: ActorBase()
	, targetPos_(nullptr)
	, hp_(DEFAULT_MAX_HP)
	, maxHp_(DEFAULT_MAX_HP)
	, animationController_(nullptr)
	, moveDir_(AsoUtility::VECTOR_ZERO)
	, movePow_(AsoUtility::VECTOR_ZERO)
	, prevPos_(AsoUtility::VECTOR_ZERO)
	, jumpPow_(AsoUtility::VECTOR_ZERO)
	, moveSpeed_(0.0f)
	, stepJump_(0.0f)
	, isJump_(false)
{
	// 所有武器配列を初期化
	weapons_.clear();
}

void CharacterBase::Update(void)
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

void CharacterBase::Draw(void)
{
	// 基底クラスの描画処理
	ActorBase::Draw();

	// 武器描画
	for (const auto& weapon : weapons_) {
		weapon->Draw();
	}
}

void CharacterBase::Release(void)
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

VECTOR CharacterBase::GetTargetDir(void)
{
	//自分と相手のベクトルを計算
	VECTOR targetVec = VSub(*targetPos_, transform_.pos);
	//ベクトルを正規化
	VECTOR targetdir = VNorm(targetVec);
	//Y座標を無効
	targetdir.y = 0.0f;

	return targetdir;
}

void CharacterBase::DelayRotate(void)
{
	// 移動方向から回転に変換する
	Quaternion goalRot = Quaternion::LookRotation(moveDir_);
	// 回転の補間
	transform_.quaRot =
		Quaternion::Slerp(transform_.quaRot, goalRot, ROT_SLERP_RATE);
}

void CharacterBase::CalcGravityPow(void)
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

void CharacterBase::Collision(void)
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

void CharacterBase::CollisionGravity(void)
{
	// 落下中しか判定しない
	if (!(VDot(AsoUtility::DIR_D, jumpPow_) > FALL_DOT_THRESHOLD))return;

	// 線分コライダーが存在しなければ終了
	int lineType = static_cast<int>(ColliderBase::SHAPE::LINE);
	if (ownColliders_.count(lineType) == 0) return;

	// 各線分コライダーとステージモデルの接地判定を実施
	for (const auto& col : ownColliders_.at(lineType))
	{
		if (!col->GetIsCollider()) continue;

		// カプセルコライダ情報  
		std::shared_ptr<ColliderLine> colliderLine =
			std::dynamic_pointer_cast<ColliderLine>(col);
		if (colliderLine == nullptr) return;

		// ステージとの交差点を検出して地面へ押し上げる
		for (const auto& hit : col->GetHitColliders())
		{
			if (hit.lock()->GetIsCollider()
				&& hit.lock()->GetTag() == ColliderBase::TAG::STAGE)
			{
				std::shared_ptr<ColliderModel> colliderModel =
					std::dynamic_pointer_cast<ColliderModel>(hit.lock());
				if (colliderModel == nullptr) continue;

				// 地面押し上げ処理
				transform_.pos =
					CollisionUtility::CalcPushUpLineModel(
						*colliderLine, *colliderModel,
						transform_.pos.y, GROUND_PUSH_UP_OFFSET,
						false, true);

				// 着地に伴いジャンプ状態と落下速度をリセット
				isJump_ = false;
				jumpPow_ = AsoUtility::VECTOR_ZERO;
				stepJump_ = 0.0f;
			}
		}
	}
}

void CharacterBase::CollisionCapsule(void)
{
	// カプセルコライダーが存在しなければ終了
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
	if (ownColliders_.count(capsuleType) == 0) return;

	// 有効なカプセルコライダーごとに衝突モデルからの押し戻しを実行
	for (const auto& col : ownColliders_.at(capsuleType))
	{
		if (!col->GetIsCollider()) continue;

		// カプセルコライダ情報  
		std::shared_ptr<ColliderCapsule> colliderCapsule =
			std::dynamic_pointer_cast<ColliderCapsule>(col);
		if (colliderCapsule == nullptr) return;

		// 接触したモデルコライダーに対して押し出し補正を適用
		for (const auto& hit : col->GetHitColliders())
		{
			// モデル以外は処理を飛ばす  
			if (hit.lock()->GetShape() != ColliderBase::SHAPE::MODEL) continue;

			std::shared_ptr<ColliderModel> colliderModel =
				std::dynamic_pointer_cast<ColliderModel>(hit.lock());
			if (colliderModel == nullptr) continue;

			// 壁・障害物からのカプセル押し戻し計算
			transform_.pos =
				CollisionUtility::CalcPushCapsuleModel(
					*colliderCapsule, *colliderModel, transform_.pos,
					CNT_TRY_COLLISION, COLLISION_BACK_DIS,
					true, false);
		}
	}
}

void CharacterBase::SetFrameUserLocalPos(VECTOR localPos, int frameNo)
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
	mix = MMult(mix, MGetTranslate(localPos));
	// 合成した行列を対象フレームにセットし直して、
	// アニメーションの移動値を無効化
	MV1SetFrameUserLocalMatrix(transform_.modelId, frameNo, mix);
}

void CharacterBase::TakeDamage(float amount)
{
	// HPを減算し、下限0.0fでクランプする
	hp_ -= amount;
	if (hp_ < 0.0f)
	{
		hp_ = 0.0f;
	}
}

void CharacterBase::HealHp(float amount)
{
	// HPを加算し、最大HPでクランプする
	hp_ += amount;
	if (hp_ > maxHp_)
	{
		hp_ = maxHp_;
	}
}

