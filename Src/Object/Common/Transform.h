#pragma once
#include <DxLib.h>
#include "../../Common/Quaternion.h"
#include "../../Utility/AsoUtility.h"

class Transform
{
public:

	// モデルのハンドルID
	int modelId;

	// 大きさ
	VECTOR scl;

	// 回転
	VECTOR rot;

	// 位置
	VECTOR pos;

	// ローカル大きさ
	VECTOR localPos;

	// 行列
	MATRIX matScl;
	MATRIX matRot;
	MATRIX matPos;

	// 回転
	Quaternion quaRot;

	// ローカル回転
	Quaternion quaRotLocal;

	// コンストラクタ
	Transform(void);

	// デストラクタ
	~Transform(void) {}

	// モデル制御の基本情報更新
	void Update(void);
	
	// 解放
	void Release(void) {}

	// モデルのハンドルIDを設定
	void SetModel(int modelHId) { modelId = modelHId; }

	// 前方方向を取得
	VECTOR GetForward(void) const { return GetDir(AsoUtility::DIR_F); }

	// 後方方向を取得
	VECTOR GetBack(void) const { return GetDir(AsoUtility::DIR_B); }

	// 右方向を取得
	VECTOR GetRight(void) const { return GetDir(AsoUtility::DIR_R); }

	// 左方向を取得
	VECTOR GetLeft(void) const { return GetDir(AsoUtility::DIR_L); }

	// 上方向を取得
	VECTOR GetUp(void) const { return GetDir(AsoUtility::DIR_U); }

	// 下方向を取得
	VECTOR GetDown(void) const { return GetDir(AsoUtility::DIR_D); }

	// 対象方向を取得
	VECTOR GetDir(const VECTOR& dir) const { return quaRot.PosAxis(dir); }

};
