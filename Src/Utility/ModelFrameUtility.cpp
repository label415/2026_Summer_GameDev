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
	// 1. 親フレームのワールド合成行列を取得
	MATRIX parentWorldMat = MV1GetFrameLocalWorldMatrix(follow.modelId, followFrameIdx);

	// 2. 親の行列から「ワールド座標」だけを純粋に抽出
	VECTOR parentWorldPos = MGetTranslateElem(parentWorldMat);

	// 3. 親の行列から「回転成分」のみを抽出（スケールを排除）
	VECTOR parentScl = MGetSize(parentWorldMat);
	if (parentScl.x == 0.0f || parentScl.y == 0.0f || parentScl.z == 0.0f) { parentScl = AsoUtility::VECTOR_ONE; }
	MATRIX parentRotMat = MGetRotElem(parentWorldMat);
	auto revParentScl = VGet(1.0f / parentScl.x, 1.0f / parentScl.y, 1.0f / parentScl.z);
	parentRotMat = MMult(parentRotMat, MGetScale(revParentScl));

	// 4. 指定された回転（Z軸-90度など）の行列を作成
	MATRIX matRotLocal = MatrixUtility::GetMatrixRotateXYZ(localRot);

	// 5. 【ここを修正】世界（ワールド）基準の回転に対して、親の回転を乗せる
	// 順番を「子 × 親」にすることで、親のローカル軸ではなく、世界の軸を基準に回ります
	MATRIX finalRotMat = MMult(matRotLocal, parentRotMat);

	// 6. 最終的なワールド行列を組み立てる
	MATRIX worldMat = MGetIdent();
	worldMat = MMult(worldMat, MGetScale(target.scl)); // 子のスケール
	worldMat = MMult(worldMat, finalRotMat);           // 合成した回転
	worldMat = MMult(worldMat, MGetTranslate(parentWorldPos)); // 親のワールド位置

	// もし localPos が指定されている場合は、親の向きに合わせて位置をオフセット
	if (VSize(localPos) > 0.0f) {
		VECTOR offsetPos = VTransform(localPos, parentRotMat);
		worldMat = MMult(worldMat, MGetTranslate(offsetPos));
	}

	// 7. target にデータを格納
	target.scl = target.scl;
	target.matRot = finalRotMat;              // 呼び出し側の GetRotation に渡る行列
	target.pos = MGetTranslateElem(worldMat); // 正しいワールド座標

	// ローカル情報は保持しておく
	target.localPos = localPos;
}