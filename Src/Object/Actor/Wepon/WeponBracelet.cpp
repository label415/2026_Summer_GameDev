#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "WeponBracelet.h"
WeponBracelet::WeponBracelet(const Transform& followTransform, const VECTOR moverDir, int followFrameId)
	:
	WeponBase(followTransform, followFrameId),
	moveDir_(moverDir)
{
	isAttack_ = false;
	isEnd_ = false;
	topPos_ = AsoUtility::VECTOR_ZERO;
	downPos_ = AsoUtility::VECTOR_ZERO;
}

WeponBracelet::~WeponBracelet(void)
{
}

void WeponBracelet::Update(void)
{
	Move();
}

void WeponBracelet::InitLoad(void)
{
}

void WeponBracelet::InitTransform(void)
{
}

void WeponBracelet::InitCollider(void)
{
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMY_WEPON, &transform_,
		AsoUtility::VECTOR_ZERO, AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_RADIUS);

	std::vector<ColliderBase*> colCapsules;
	colCapsules.push_back(colCapsule);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsules);
}

void WeponBracelet::InitAnimation(void)
{
}

void WeponBracelet::InitPost(void)
{
	// 初期位置を手元のワールド座標に合わせる
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	// top と down は手元からの相対座標(0,0,0)からスタート
	topPos_ = AsoUtility::VECTOR_ZERO;
	downPos_ = AsoUtility::VECTOR_ZERO;

	moveSpeed_ = SPEED;
}

void WeponBracelet::Move(void)
{
	// 常に手元のボーン位置に追従（ベース座標の更新）
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	// --- 状態ごとの処理 ---
	if (isAttack_ && !isEnd_)
	{
		// 1. 攻撃が伸びていくフェーズ (Topが移動)
		if (VSize(VSub(topPos_, downPos_)) < LENGTH) {
			topPos_ = VAdd(topPos_, VScale(moveDir_, moveSpeed_));
		}
		else {
			// 最大まで伸びたら、自動的に消滅フェーズ(isEnd)へ移行する場合の例
			// (外部のマネージャー等で切り替える場合は削除してください)
			// isEnd_ = true; 
		}
	}
	else if (isEnd_)
	{
		// 2. 攻撃が根元から消えていくフェーズ (Downが追いつく)
		isAttack_ = false;
		downPos_ = VAdd(downPos_, VScale(moveDir_, moveSpeed_));

		// Topに追いついたら（判定が縮みきったら）消滅
		if (VSize(VSub(topPos_, downPos_)) <= 50.0f) {
			isAlive_ = false;
		}
	}

	// --- コライダーの更新 (ローカル座標として設定) ---
	const auto& cols = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	for (const auto& col : cols) {
		if (col->GetTag() != ColliderBase::TAG::ENEMY_WEPON) continue;

		ColliderCapsule* colliderCapsule = dynamic_cast<ColliderCapsule*>(col);
		if (colliderCapsule)
		{
			// ローカル座標（手元からの相対位置）をそのまま渡す
			colliderCapsule->SetLocalPosTop(topPos_);
			colliderCapsule->SetLocalPosDown(downPos_);
		}
	}
}

void WeponBracelet::Draw(void)
{
	ActorBase::Draw();

	if (isAttack_ || isEnd_)
	{
		// DrawCapsule3D はワールド座標を要求するため、
		// ベースとなる transform_.pos（手元の位置）を足してワールド座標に変換して描画する
		VECTOR worldTop = VAdd(transform_.pos, topPos_);
		VECTOR worldDown = VAdd(transform_.pos, downPos_);

		DrawCapsule3D(worldTop, worldDown, COL_CAPSULE_RADIUS, 10, 0xff0000, 0xff0000, true);
	}
}

void WeponBracelet::SetCollider(void)
{
}

void WeponBracelet::ClearCollider(void)
{
	
}
