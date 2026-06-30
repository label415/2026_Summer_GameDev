#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/Collider/ColliderLine.h"
#include "../../Common/Collider/ColliderModel.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../Stage/Stage.h"
#include "CharactorBase.h"

CharactorBase::CharactorBase(void)
	:
	ActorBase()
{
}

CharactorBase::~CharactorBase(void)
{
}

void CharactorBase::Update(void)
{
	// 移動前座標を更新
	prevPos_ = transform_.pos;
	isAttack_ = false;
	isDamage_ = false;

	// 各キャラクターごとの更新処理
	UpdateProcess();

	// 移動方向に応じた遅延回転
	DelayRotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定前準備
	CollisionReserve();

	// 衝突判定
	Collision();

	// モデル制御更新
	transform_.Update();

	// アニメーション再生
	anim_->Update();

	// 各キャラクターごとの更新後処理
	UpdateProcessPost();
}

void CharactorBase::Draw(void)
{
	// 基底クラスの描画処理
	ActorBase::Draw();
}

void CharactorBase::Release(void)
{
	transform_.Release();

	anim_->Release();
	delete anim_;
	delete effect_;
}

VECTOR CharactorBase::GetTargetDir(void)
{
	//自分と相手のベクトルを計算
	VECTOR targetVec = VSub(*targetTrans_, transform_.pos);
	//ベクトルを正規化
	VECTOR targetdir = VNorm(targetVec);
	//Y座標を無効
	targetdir.y = 0.0f;

	return targetdir;
}

void CharactorBase::InitLoad(void)
{
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
}

void CharactorBase::CollisionGravity(void)
{
	// 落下中しか判定しない
	if (!(VDot(AsoUtility::DIR_D, jumpPow_) > 0.9f))
	{
		return;
	}

	// 線分コライダ
	int lineType = static_cast<int>(ColliderBase::SHAPE::LINE);

	// 線分コライダが無ければ処理を抜ける
	if (ownColliders_.count(lineType) == 0) return;


	const auto& vecs = ownColliders_.at(lineType);
	for (const auto& vec : vecs)
	{
		// 線分コライダ情報
		ColliderLine* colliderLine_ =
			dynamic_cast<ColliderLine*>(vec);

		if (colliderLine_ == nullptr) return;

		// 登録されている衝突物を全てチェック
		for (const auto& hitCol : hitColliders_)
		{
			for (const auto& i : hitCol.second)
			{
				// ステージ以外は処理を飛ばす
				if (i->GetTag() != ColliderBase::TAG::STAGE) continue;

				// 派生クラスへキャスト
				const ColliderModel* colliderModel =
					dynamic_cast<const ColliderModel*>(i);

				if (colliderModel == nullptr) continue;

				bool isHit = colliderLine_->PushBackUp(colliderModel, transform_, 2.0f, false, false);

				if (isHit)
				{
					isJump_ = false;
				}
			}
		}
		if (!isJump_)
		{
			// ジャンプリセット
			jumpPow_ = AsoUtility::VECTOR_ZERO;

			// ジャンプの入力受付時間をリセット
			stepJump_ = 0.0f;
		}
	}
}

void CharactorBase::CollisionCapsule(void)
{
	// カプセルコライダ  
	int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);

	// カプセルコライダが無ければ処理を抜ける  
	if (ownColliders_.count(capsuleType) == 0) return;

	const auto& vecs = ownColliders_.at(capsuleType);
	for (const auto& vec : vecs)
	{
		// カプセルコライダ情報  
		ColliderCapsule* colliderCapsule =
			dynamic_cast<ColliderCapsule*>(vec);

		if (colliderCapsule == nullptr) return;

		// 登録されている衝突物を全てチェック  
		for (const auto& hitCol : hitColliders_)
		{
			for (const auto& i : hitCol.second)
			{
				// モデル以外は処理を飛ばす  
				if (i->GetShape() != ColliderBase::SHAPE::MODEL) continue;

				const ColliderModel* colliderModel =
					dynamic_cast<const ColliderModel*>(i);

				if (colliderModel == nullptr) continue;

				colliderCapsule->PushBackAlongNormal(
					colliderModel,
					transform_,
					CNT_TRY_COLLISION,
					COLLISION_BACK_DIS,
					false,
					false
				);
			}
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

