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

	// 衝突判定の設定
	virtual void SetCollider(void) {}

	// 衝突判定のクリア
	virtual void ClearCollider(void) {}

	// 攻撃フラグの設定
	virtual void SetIsAttack(bool isAttack){}

	// 終了フラグの設定
	virtual void SetIsEnd(bool isEnd){}
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
