#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../../Common/EffectController.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Application.h"
#include "../../../Libs/ImGui/imgui.h"
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
	if (!isAlive_) {
		ClearCollider();
		return;
	}
	UpdateDebugImGui();
	Move();
	effect_->Update(static_cast<int>(EFFECT_TYPE::BRACELET));
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
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_TOP_LOCAL_POS,
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
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	topPos_ = AsoUtility::VECTOR_ZERO;
	downPos_ = AsoUtility::VECTOR_ZERO;

	moveSpeed_ = SPEED;

	effect_ = new EffectController();
	effect_->Add(
		static_cast<int>(EFFECT_TYPE::BRACELET),
		(Application::PATH_EFFECT + L"Breath.efkefc"));
}

void WeponBracelet::Move(void)
{
	transform_.pos = MV1GetFramePosition(followTransform_.modelId, followFrameId_);

	if (isAttack_ && !isEnd_)
	{
		if (VSize(VSub(topPos_, downPos_)) < LENGTH) {
			topPos_ = VAdd(topPos_, VScale(moveDir_, moveSpeed_));
		}
	}
	else if (isEnd_)
	{
		isAttack_ = false;
		downPos_ = VAdd(downPos_, VScale(moveDir_, moveSpeed_));
		if (VSize(VSub(topPos_, downPos_)) <= 50.0f) {
			isAlive_ = false;
		}
	}

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
	ActorBase::Draw();
}

void WeponBracelet::Release(void)
{
	ActorBase::Release();
	delete effect_;
}

void WeponBracelet::SetCollider(void)
{
}

void WeponBracelet::ClearCollider(void)
{
	ownColliders_.erase(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
}

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
			euler, VGet(400.0f, 400.0f, LENGTH / 2.0f));
	}
	isAttack_ = isAttack;
}

void WeponBracelet::UpdateDebugImGui(void)
{
	// ウィンドウタイトル&開始処理
	ImGui::Begin("Wepon");
	// 角度
	VECTOR rotDeg = VECTOR();
	rotDeg.x = AsoUtility::Rad2DegF(localRot_.x);
	rotDeg.y = AsoUtility::Rad2DegF(localRot_.y);
	rotDeg.z = AsoUtility::Rad2DegF(localRot_.z);
	ImGui::Text("angle(deg)");
	ImGui::SliderFloat("RotX", &rotDeg.x, 0.0f, 360.0f);
	ImGui::SliderFloat("RotY", &rotDeg.y, 0.0f, 360.0f);
	ImGui::SliderFloat("RotZ", &rotDeg.z, 0.0f, 360.0f);
	localRot_.x = AsoUtility::Deg2RadF(rotDeg.x);
	localRot_.y = AsoUtility::Deg2RadF(rotDeg.y);
	localRot_.z = AsoUtility::Deg2RadF(rotDeg.z);
	// 終了処理
	ImGui::End();
}
