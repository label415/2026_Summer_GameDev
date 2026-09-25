#include "MatrixUtility.h"

MATRIX MatrixUtility::GetMatrixRotateXYZ(const VECTOR& euler)
{
	// オイラー角から回転行列XYZ順を取得
	MATRIX ret = MGetIdent();
	ret = MMult(ret, MGetRotX(euler.x));
	ret = MMult(ret, MGetRotY(euler.y));
	ret = MMult(ret, MGetRotZ(euler.z));
	return ret;
}

MATRIX MatrixUtility::Multiplication(const MATRIX& child, const MATRIX& parent)
{
	// 親子の回転行列を合成する
	return MMult(child, parent);
}

MATRIX MatrixUtility::Multiplication(const VECTOR& childEuler, const VECTOR& parentEuler)
{
	// 親子のオイラー角を合成する
	MATRIX parent = MatrixUtility::GetMatrixRotateXYZ(parentEuler);
	MATRIX child = MatrixUtility::GetMatrixRotateXYZ(childEuler);
	return MMult(child, parent);
}
