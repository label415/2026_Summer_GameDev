#include <DxLib.h>
#include "../../Utility/AsoUtility.h"
#include "Transform.h"

Transform::Transform(void)
	: modelId(-1)
	, scl(AsoUtility::VECTOR_ONE)
	, rot(AsoUtility::VECTOR_ZERO)
	, pos(AsoUtility::VECTOR_ZERO)
	, localPos(AsoUtility::VECTOR_ZERO)
	, matScl(MGetIdent())
	, matRot(MGetIdent())
	, matPos(MGetIdent())
	, quaRot(Quaternion())
	, quaRotLocal(Quaternion())

{
}

void Transform::Update(void)
{
	// 大きさ
	matScl = MGetScale(scl);

	// 回転
	rot = quaRot.ToEuler();
	matRot = quaRot.ToMatrix();

	// 位置
	matPos = MGetTranslate(pos);

	// 行列の合成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, matScl);
	Quaternion q = quaRot.Mult(quaRotLocal);
	mat = MMult(mat, q.ToMatrix());
	mat = MMult(mat, matPos);

	// 行列をモデルに判定
	if (modelId != -1)
	{
		MV1SetMatrix(modelId, mat);
	}
}
