#pragma once
#include <DxLib.h>
#include "WeaponBase.h"

class Transform;

class WeaponBlade : public WeaponBase
{
public:

	// コンストラクタ
	WeaponBlade(const Transform& followTransform, int followFrameId);

	// デストラクタ
	~WeaponBlade(void) override = default;

	// 更新
	void Update(void) override;

	// コライダーを設定
	void SetCollider(void) override;

	// コライダーをクリア
	void ClearCollider(void) override;

protected:

	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override {}

	// 初期化後の個別処理
	void InitPost(void) override {}

private:

	// モデルの大きさ
	static constexpr float SCALE = 1.3f;

	// 初期ローカル座標・回転
	static constexpr VECTOR DEFAULT_LOCAL_POS = { -2.0f, 0.0f, -3.0f };
	static constexpr float DEFAULT_LOCAL_ROT_Z_DEG = -90.0f;

	// 衝突判定用カプセルの初期値
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 150.0f, 0.0f };
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 20.0f, 0.0f };
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;

};

