#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/AnimationController.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
{ 
	animationController_ = nullptr;

	animType_ = 0;

	modelId_ = -1;
	pos_ = { 0.0f,0.0f,0.0f };
	angle_ = { 0.0f,0.0f,0.0f };
	localAngle_ = { 0.0f,0.0f,0.0f };
	scale_ = { 0.0f,0.0f,0.0f };

	startCapsulePos_ = { 0.0f,0.0f,0.0f };
	endCapsulePos_ = { 0.0f,0.0f,0.0f };
	capsuleRadius_ = 0.0f;

	preInputDir_ = { 0.0f,0.0f,0.0f };

	moveDir_ = { 0.0f,0.0f,0.0f };
	jumpPow_ = 0.0f;
	isCollision_ = false;
}

ActorBase::~ActorBase(void)
{
}

void ActorBase::Init(void)
{
	// Transform初期化
	InitTransform();

	// アニメーションの初期化
	InitAnimation();

	// 初期化後の個別処理
	InitPost();
}

void ActorBase::Load(void)
{
	// リソースロード
	InitLoad();
}

void ActorBase::LoadEnd(void)
{
	// 初期化
	Init();
}

void ActorBase::Update(void)
{
	// プレイヤーの遅延回転処理
	DelayRotate();

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = MatrixUtility::Multiplication(localAngle_, angle_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// プレイヤーの移動処理
	Move();

	// 重力(加速度を速度に加算していく)
	jumpPow_ -= 0.8f;

	// プレイヤーの座標に移動量(速度、ジャンプ力)を加算する
	pos_.y += jumpPow_;

	// モデルに座標を設定する
	MV1SetPosition(modelId_, pos_);

	// アニメーションの更新
	animationController_->Update();
}

void ActorBase::Draw(void)
{
	MV1DrawModel(modelId_);

	DrawSphere3D(
		VAdd(pos_,startCapsulePos_),
		capsuleRadius_,
		16,
		0x00ff00,
		0x00ff00,
		false
	);

	DrawSphere3D(
		VAdd(pos_, endCapsulePos_),
		capsuleRadius_,
		16,
		0x00ff00,
		0x00ff00,
		false
	);
}

void ActorBase::Release(void)
{
	MV1DeleteModel(modelId_);
	delete animationController_;
}

void ActorBase::CollisionStage(const VECTOR& pos)
{
	// 衝突判定に指定座標に押し戻す
	pos_ = pos;
	jumpPow_ = 0.0f;
}

void ActorBase::Move(void)
{
}

void ActorBase::DelayRotate(void)
{
	// 移動方向から角度に変換する
	float goal = atan2f(moveDir_.x, moveDir_.z);

	// 常に最短経路で補間
	angle_.y = AsoUtility::LerpAngle(angle_.y, goal, 0.2f);
}
