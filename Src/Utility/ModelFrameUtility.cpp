#include "AsoUtility.h"
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
	// 対象フレームのローカル座標からワールド座標に変換する行列を得る
	// ( 大きさ、回転、位置 )
	MATRIX worldMatMix =
		MV1GetFrameLocalWorldMatrix(follow.modelId, followFrameIdx);


}
