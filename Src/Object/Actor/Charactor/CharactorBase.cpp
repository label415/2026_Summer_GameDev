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

	// 線分コライダ情報
	ColliderLine* colliderLine_ =
		dynamic_cast<ColliderLine*>(ownColliders_.at(lineType));

	if (colliderLine_ == nullptr) return;

	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// ステージ以外は処理を飛ばす
		if (hitCol->GetTag() != ColliderBase::TAG::STAGE) continue;

		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) continue;

		bool isHit = colliderLine_->PushBackUp(colliderModel, transform_, 2.0f, true, false);

		if (isHit)
		{
			isJump_ = false;
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

void CharactorBase::CollisionCapsule(void)  
{  
   // カプセルコライダ  
   int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);  
   // カプセルコライダが無ければ処理を抜ける  
   if (ownColliders_.count(capsuleType) == 0) return;  
   // カプセルコライダ情報  
   ColliderCapsule* colliderCapsule =  
       dynamic_cast<ColliderCapsule*>(ownColliders_.at(capsuleType));  

   if (colliderCapsule == nullptr) return;  
   // 登録されている衝突物を全てチェック  
   for (const auto& hitCol : hitColliders_)  
   {  
       // モデル以外は処理を飛ばす  
       if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

       const ColliderModel* colliderModel =  
           dynamic_cast<const ColliderModel*>(hitCol);

       if (colliderModel == nullptr) continue;

	   colliderCapsule->PushBackAlongNormal(
		   colliderModel,
		   transform_,
           CNT_TRY_COLLISION,  
           COLLISION_BACK_DIS,  
           true,  
           false  
       );  
   }  
}