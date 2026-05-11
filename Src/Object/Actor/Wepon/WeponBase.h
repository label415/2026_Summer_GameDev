#pragma once
#include <DxLib.h>
#include "../ActorBase.h"
class Transform;
class WeponBase : public ActorBase
{
public:
	// コンストラクタ
	WeponBase(const Transform& followTransform, int followFrameId);
	// デストラクタ
	virtual ~WeponBase(void) override;
	// 更新
	virtual void Update(void) override;

	//更新時当たり判定登録
	virtual void UpdateCollider(void){}

	//当たり判定解除
	virtual void ClearCollider(void) {}

protected:
	// 追従先Transform
	const Transform& followTransform_;
	// 追従対象のフレームID
	int followFrameId_;
	// ローカル座標
	VECTOR localPos_;
	// ローカル回転
	VECTOR localRot_;
};
