#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../Common/Transform.h"
#include "WeponBase.h"
WeponBase::WeponBase(const Transform& followTransform, int followFrameId)
	:
	followTransform_(followTransform),
	followFrameId_(followFrameId),
	localPos_(AsoUtility::VECTOR_ZERO),
	localRot_(AsoUtility::VECTOR_ZERO)
{
}
WeponBase::~WeponBase(void)
{
}
void WeponBase::Update(void)
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