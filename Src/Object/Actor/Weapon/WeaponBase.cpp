#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../Common/Transform.h"
#include "WeaponBase.h"

WeaponBase::WeaponBase(WEAPON_TYPE weaponType, const Transform& followTransform, int followFrameId)
	:
	weaponType_(weaponType),
	followTransform_(followTransform),
	followFrameId_(followFrameId),
	localPos_(AsoUtility::VECTOR_ZERO),
	localRot_(AsoUtility::VECTOR_ZERO)
{
	// 武器種別と追従先フレーム情報を初期化
}

void WeaponBase::Update(void)
{
	// 対象フレームの位置にtargetを配置し、
	// 対象フレームの回転に加え、指定した相対座標・回転を加える
	ModelFrameUtility::SetFrameWorldMatrix(
		followTransform_, followFrameId_, transform_,
		localPos_, localRot_ 
	);
	// 上記関数内で更新された行列情報からクォータニオンに変換
	transform_.quaRot = Quaternion::GetRotation(transform_.matRot);
	// 大きさ、回転(クォータニオン)、座標を元にモデルを更新
	transform_.Update();
}

void WeaponBase::Collision(void)
{
	// 衝突したコライダーの結果を更新
	UpdateHitCollider();
}
