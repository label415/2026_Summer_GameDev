#pragma once
#include <DxLib.h>
#include "ColliderBase.h"

class Transform;

class ColliderCapsule : public ColliderBase
{
public:

	// コンストラクタ
	ColliderCapsule(
		TAG tag,
		const std::vector<TAG>& targetTags,
		const Transform* follow,
		const VECTOR& localPosTop,
		const VECTOR& localPosDown,
		float radius, int partTag = 0);

	// デストラクタ
	~ColliderCapsule(void) {}

	// 親Transformからの相対位置を取得
	const VECTOR& GetLocalPosTop(void) const { return localPosTop_; }
	const VECTOR& GetLocalPosDown(void) const { return localPosDown_; }

	// 親Transformからの相対位置をセット
	void SetLocalPosTop(const VECTOR& pos) { localPosTop_ = pos; }
	void SetLocalPosDown(const VECTOR& pos) { localPosDown_ = pos; }

	// ワールド座標を取得
	VECTOR GetPosTop(void) const { return GetRotPos(localPosTop_); }
	VECTOR GetPosDown(void) const { return GetRotPos(localPosDown_); }

	// 半径
	float GetRadius(void) const { return radius_; }
	void SetRadius(float radius) { radius_ = radius; }

	// 高さ
	float GetHeight(void) const { return localPosTop_.y; }

	// カプセルの中心座標
	VECTOR& GetCenter(void);

protected:

	// デバッグ用描画
	void DrawDebug(int color) override;

private:

	// 親Transformからの相対位置
	VECTOR localPosTop_;
	VECTOR localPosDown_;

	// カプセルの中心
	VECTOR center_;

	// 半径
	float radius_;

	// デバッグ描画用定数
	static constexpr int DEBUG_SPHERE_DIV_NUM = 5;
	static constexpr float DEBUG_CENTER_SPHERE_RADIUS = 5.0f;
	static constexpr int DEBUG_CENTER_SPHERE_DIV_NUM = 10;

};
