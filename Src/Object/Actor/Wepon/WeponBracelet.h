#pragma once
#pragma once
#include <DxLib.h>
#include "WeponBase.h"
class Transform;
class WeponBracelet : public WeponBase
{
public:
	// コンストラクタ
	WeponBracelet(const Transform& followTransform, const VECTOR moverDir, int followFrameId);
	// デストラクタ
	~WeponBracelet(void) override;
	// 更新
	void Update(void) override;
	// 描画
	void Draw(void)override;
	void SetCollider(void) override;
	void ClearCollider(void) override;

	void SetIsAttack(bool isAttack) { isAttack_ = isAttack; }
	void SetIsEnd(bool isEnd) { isEnd_ = isEnd; }

protected:
	// リソースロード
	void InitLoad(void) override;
	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;
	// 衝突判定の初期化
	void InitCollider(void) override;
	// アニメーションの初期化
	void InitAnimation(void) override;
	// 初期化後の個別処理
	void InitPost(void) override;

private:
	// 	// モデルの大きさ
	static constexpr float SCALE = 0.4f;
	// モデルの回転調整
	static constexpr VECTOR DEFAULT_LOCAL_ROT =
	{ 0.0f, 180.0f * DX_PI_F / 180.0f, 0.0f };

	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };
	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 20.0f, 0.0f };
	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 200.0f;

	static constexpr float SPEED = 100.0f;

	static constexpr float LENGTH = 3000.0f;

	//後ろ座標
	VECTOR topPos_;
	VECTOR downPos_;
	// 移動方向
	VECTOR moveDir_;
	// 移動量
	VECTOR movePow_;
	// 移動前の座標
	VECTOR prevPos_;

	//長さ
	float length_;
	// 移動スピード
	float moveSpeed_;

	bool isAttack_;
	bool isEnd_;

	//移動処理
	void Move(void);

};

