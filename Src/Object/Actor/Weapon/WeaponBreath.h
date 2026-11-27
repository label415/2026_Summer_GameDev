#pragma once
#include <DxLib.h>
#include "WeaponBase.h"

class Transform;

class WeaponBreath : public WeaponBase
{
public:

	// コンストラクタ
	WeaponBreath(
		const Transform& followTransform,
		const VECTOR moveDir, int followFrameId);

	// デストラクタ
	~WeaponBreath(void) override = default;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void)override;

	// 解放
	void Release(void)override;

	// 衝突判定の設定
	void SetCollider(void) override;

	// 衝突判定のクリア
	void ClearCollider(void) override;

	// 攻撃フラグの設定
	void SetIsAttack(bool isAttack)override;

	// 終了フラグの設定
	void SetIsEnd(bool isEnd) override { isEnd_ = isEnd; }

	// 移動方向の設定
	void SetMoveDir(const VECTOR& dir) { moveDir_ = dir; }

protected:

	// リソースロード
	void InitLoad(void) override {}

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override {}

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override {}

	// 初期化後の個別処理
	void InitPost(void) override;

private:

	// 衝突判定用カプセル初期ローカル座標
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 0.0f, 300.0f };
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 0.0f, 300.0f };

	// 初期角度
	static constexpr VECTOR DEFAULT_ROT = { 0.0f, -(90.0f * DX_PI_F / 180.0f), 0.0f };

	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 200.0f;

	// 移動スピード
	static constexpr float SPEED = 150.0f;

	// 移動距離
	static constexpr float LENGTH = 2000.0f;

	// モデルの大きさ
	static constexpr float SCALE = 0.4f;

	// エフェクトYオフセット・スケール・長さ除数
	static constexpr float EFFECT_OFFSET_Y = 250.0f;
	static constexpr float EFFECT_SCALE_XY = 400.0f;
	static constexpr float EFFECT_LENGTH_DIV = 2.2f;

	// 衝突判定用座標
	VECTOR topPos_;
	VECTOR downPos_;

	// 移動方向
	VECTOR moveDir_;

	// 移動スピード
	float moveSpeed_;

	// 攻撃フラグ
	bool isAttack_;

	// 終了フラグ
	bool isEnd_;

	// 再生中エフェクトハンドル
	int effHandle_;

	//移動処理
	void Move(void);
};

