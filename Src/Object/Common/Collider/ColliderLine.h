#pragma once
#include <DxLib.h>
#include "ColliderBase.h"

class Transform;
class ColliderModel;

class ColliderLine : public ColliderBase
{
public:

	// コンストラクタ
	ColliderLine(
		TAG tag, const std::vector<TAG>& targetTags, const Transform* follow,
		const VECTOR& localPosStart, const VECTOR& localPosEnd, int partTag = 0);

	// デストラクタ
	~ColliderLine(void) override = default;

	// ローカル座標での設定
	void SetLocalPosStart(const VECTOR& pos) { localPosStart_ = pos; }
	void SetLocalPosEnd(const VECTOR& pos) { localPosEnd_ = pos; }

	// ローカル座標の取得
	const VECTOR& GetLocalPosStart(void) const { return localPosStart_; }
	const VECTOR& GetLocalPosEnd(void) const { return localPosEnd_; }

	// ワールド座標の取得
	VECTOR GetPosStart(void) const;
	VECTOR GetPosEnd(void) const;

	// ラインコライダーの長さの取得
	float GetLineLength(void) const { return VSize(VSub(GetPosEnd(), GetPosStart())); }

protected:

	// デバッグ用描画
    void DrawDebug(int color) override;

private:

	// デバッグ表示の球体半径
	static constexpr float RADIUS = 5.0f;

	// デバッグ表示の球体ポリゴン分割数
	static constexpr int DIV_NUM = 6;

	// 線分の開始座標
	VECTOR localPosStart_;

	// 線分の終了座標
	VECTOR localPosEnd_;

};

