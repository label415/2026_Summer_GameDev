#include "../Object/Common/Transform.h"
#include "AsoUtility.h"
#include "MatrixUtility.h"
#include "ModelFrameUtility.h"

void ModelFrameUtility::GetFrameWorldMatrix(
	int modelId, int frameIdx, VECTOR& scl, MATRIX& matRot, VECTOR& pos)
{
	// 対象フレームのローカル座標からワールド座標に変換する行列を得る
	// ( 大きさ、回転、位置 )
	auto mat = MV1GetFrameLocalWorldMatrix(modelId, frameIdx);
	// 拡大縮小成分
	scl = MGetSize(mat);
	// 回転成分＋拡大縮小成分
	matRot = MGetRotElem(mat);
	// 回転成分のみにする
	auto revScl = VGet(1.0f / scl.x, 1.0f / scl.y, 1.0f / scl.z);
	matRot = MMult(matRot, MGetScale(revScl));
	// 移動成分
	pos = MGetTranslateElem(mat);
}

void ModelFrameUtility::SetFrameWorldMatrix(
	const Transform& follow, int followFrameIdx,
	Transform& target, VECTOR localPos, VECTOR localRot)
{
	// 親フレームのワールド合成行列を取得
	MATRIX parentWorldMat = MV1GetFrameLocalWorldMatrix(follow.modelId, followFrameIdx);

	// 親の行列から「ワールド座標」だけを取得
	VECTOR parentWorldPos = MGetTranslateElem(parentWorldMat);

	// 親の行列から「回転成分」のみを取得
	VECTOR parentScl = MGetSize(parentWorldMat);
	if (parentScl.x == 0.0f || parentScl.y == 0.0f || parentScl.z == 0.0f)
	{
		parentScl = AsoUtility::VECTOR_ONE;

	}
	MATRIX parentRotMat = MGetRotElem(parentWorldMat);
	auto revParentScl = VGet(1.0f / parentScl.x, 1.0f / parentScl.y, 1.0f / parentScl.z);
	parentRotMat = MMult(parentRotMat, MGetScale(revParentScl));

	// 指定された回転の行列を作成
	MATRIX matRotLocal = MatrixUtility::GetMatrixRotateXYZ(localRot);

	// ワールド座標からローカル座標に変換する行列を得る
	MATRIX finalRotMat = MMult(matRotLocal, parentRotMat);

	// 最終的なワールド行列を組み立てる
	MATRIX worldMat = MGetIdent();
	worldMat = MMult(worldMat, MGetScale(target.scl));
	worldMat = MMult(worldMat, finalRotMat);
	worldMat = MMult(worldMat, MGetTranslate(parentWorldPos));

	if (VSize(localPos) > 0.0f)
	{
		VECTOR offsetPos = VTransform(localPos, parentRotMat);
		worldMat = MMult(worldMat, MGetTranslate(offsetPos));
	}

	// データを格納
	target.scl = target.scl;
	target.matRot = finalRotMat;
	target.pos = MGetTranslateElem(worldMat);
	target.localPos = localPos;
}