#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Application.h"
#include "../../../Libs/ImGui/imgui.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../../Common/EffectController.h"
#include "WeponBracelet.h"

WeponBracelet::WeponBracelet(const Transform& followTransform, const ColliderModel* colMod, const VECTOR moverDir, int followFrameId)
	:
	WeponBase(followTransform, followFrameId),
	ColMod_(colMod),
	moveDir_(moverDir)
{
	// 初期化
	isAttack_ = false;
	isEnd_ = false;
	topPos_ = AsoUtility::VECTOR_ZERO;
	downPos_ = AsoUtility::VECTOR_ZERO;
}

WeponBracelet::~WeponBracelet(void){}

void WeponBracelet::Update(void)
{
	// 移動処理
	Move();

	// エフェクト更新
	effect_->Update(static_cast<int>(EFFECT_TYPE::BRACELET));
}

void WeponBracelet::InitLoad(void){}

void WeponBracelet::InitTransform(void){}

void WeponBracelet::InitCollider(void)
{
	// 衝突判定用カプセルの作成
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMY_WEPON, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_RADIUS);

	// カプセルコライダを自身の衝突情報に登録
	std::vector<ColliderBase*> colCapsules;
	colCapsules.push_back(colCapsule);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsules);
}

void WeponBracelet::InitAnimation(void)
{
}

void WeponBracelet::InitPost(void)
{
	// 追従先のフレーム座標を取得して自身の座標に設定
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	// 衝突判定用座標の初期化
	topPos_ = AsoUtility::VECTOR_ZERO;
	downPos_ = AsoUtility::VECTOR_ZERO;

	// 移動スピードの初期化
	moveSpeed_ = SPEED;

	// エフェクト初期化
	effect_ = new EffectController();
	// エフェクトの追加
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::BRACELET),
		(Application::PATH_EFFECT + L"Breath.efkefc"));
}

void WeponBracelet::Move(void)
{
	// 追従先のフレーム座標を取得して自身の座標に設定
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	// 衝突判定用座標の更新
	if (isAttack_ && !isEnd_)
	{
		if (VSize(VSub(topPos_, downPos_)) < LENGTH) {
			topPos_ = VAdd(topPos_, VScale(moveDir_, moveSpeed_));
		}
	}
	// 終了フラグが立っている場合は、下方向に移動させる
	else if (isEnd_)
	{
		isAttack_ = false;
		downPos_ = VAdd(downPos_, VScale(moveDir_, moveSpeed_));
		if (VSize(VSub(topPos_, downPos_)) <= 50.0f) {
			isAlive_ = false;
		}
	}

	// カプセルコライダの位置を更新
	const auto& cols = ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	for (const auto& col : cols) {
		if (col->GetTag() != ColliderBase::TAG::ENEMY_WEPON) continue;

		ColliderCapsule* colliderCapsule = dynamic_cast<ColliderCapsule*>(col);
		if (colliderCapsule)
		{
			colliderCapsule->SetLocalPosTop(topPos_);
			colliderCapsule->SetLocalPosDown(downPos_);
		}
	}
}

void WeponBracelet::Draw(void)
{
	// エフェクト描画
	ActorBase::Draw();
}

void WeponBracelet::Release(void)
{
	// 基底クラスの解放処理
	ActorBase::Release();
	delete effect_;
}

void WeponBracelet::SetCollider(void){}

void WeponBracelet::SetIsAttack(bool isAttack)
{
	if (!isAttack_) {
		float yaw = atan2f(moveDir_.x, moveDir_.z);
		float pitch = -asinf(moveDir_.y);
		VECTOR euler = { pitch, yaw, 0.0f };
		euler = VAdd(euler, WeponBracelet::DEFAULT_ROT);
		VECTOR effPos = transform_.pos;
		effPos.y -= 250.0f;

		effect_->Play(
			static_cast<int>(EFFECT_TYPE::BRACELET),
			effPos,
			euler, VGet(400.0f, 400.0f, LENGTH / 2.2f));
	}
	isAttack_ = isAttack;
}
