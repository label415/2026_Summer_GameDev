#include "AsoUtility.h"
#include "MatrixUtility.h"
#include "../Object/Common/Transform.h"
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
	const Transform & follow, int followFrameIdx,
	Transform & target, VECTOR localPos, VECTOR localRot)
{
	// 親フレームのワールド合成行列（スケール・回転・位置）を取得
	MATRIX parentWorldMat = MV1GetFrameLocalWorldMatrix(follow.modelId, followFrameIdx);

	// 子ローカル行列を作成（スケール * ローカル回転 * ローカル移動）
	MATRIX matLocal = MGetIdent();
	matLocal = MMult(matLocal, MGetScale(target.scl));
	matLocal = MMult(matLocal, MatrixUtility::GetMatrixRotateXYZ(localRot));
	matLocal = MMult(matLocal, MGetTranslate(localPos));

	// ワールド行列を合成（子ローカル * 親ワールド）
	MATRIX worldMat = MMult(matLocal, parentWorldMat);

	// ワールド行列から大きさ・回転（行列）・位置を抽出
	VECTOR scl = MGetSize(worldMat);
	// ゼロ防止（万が一）
	if (scl.x == 0.0f) scl.x = 1.0f;
	if (scl.y == 0.0f) scl.y = 1.0f;
	if (scl.z == 0.0f) scl.z = 1.0f;

	target.scl = scl;
	// 回転成分（まずは回転＋スケール成分を抜く）
	target.matRot = MGetRotElem(worldMat);
	// 回転のみを残す（スケールを除去）
	auto revScl = VGet(1.0f / scl.x, 1.0f / scl.y, 1.0f / scl.z);
	target.matRot = MMult(target.matRot, MGetScale(revScl));
	// 位置
	target.pos = MGetTranslateElem(worldMat);

	// ローカル情報は保持しておく（必要なら）
	target.localPos = localPos;
}