#pragma once
#include <DxLib.h>
#include "../ActorBase.h"

class Transform;

class WeaponBase : public ActorBase
{
public:

	// 武器種別
	enum class WEAPON_TYPE
	{
		BLADE,
		BREATH,
		FIREBALL,
		MAX
	};

	// コンストラクタ
	WeaponBase(WEAPON_TYPE weaponType, const Transform& followTransform, int followFrameId);

	// デストラクタ
	~WeaponBase(void) override = default;

	// 更新
	virtual void Update(void) override;

	// 衝突判定の設定
	virtual void SetCollider(void) = 0;

	// 衝突判定のクリア
	virtual void ClearCollider(void) = 0;

	// 攻撃フラグの設定
	virtual void SetIsAttack(bool isAttack) {};

	// 終了フラグの設定
	virtual void SetIsEnd(bool isEnd) {};

	// 武器種
	WEAPON_TYPE GetWeaponType(void) const { return weaponType_; }

protected:

	// 武器種
	WEAPON_TYPE weaponType_;

	// 追従先Transform
	const Transform& followTransform_;

	// ローカル座標
	VECTOR localPos_;

	// ローカル回転
	VECTOR localRot_;

	// 追従対象のフレームID
	int followFrameId_;

	// 当たり判定衝突時の更新処理
	void Collision(void) override;

};
