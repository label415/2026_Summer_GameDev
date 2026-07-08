#pragma once
#include "WeponBase.h"
class Transform;
class WeponFlameThrower :
    public WeponBase
{
public:
	//攻撃エフェクト
	enum class EFFECT_TYPE
	{
		FIREBALL,
	};

	// コンストラクタ
	WeponFlameThrower(const Transform& followTransform, const VECTOR moverDir, int followFrameId);
	// デストラクタ
	~WeponFlameThrower(void) override;
	// 更新
	void Update(void) override;
	// 描画
	void Draw(void)override;
	// 解放
	void Release(void)override;
	void ClearCollider(void) override;

protected:

	// 衝突判定用カプセル球体半径
	static constexpr float MIN_RADIUS = 50.0f;
	// 衝突判定用カプセル球体半径
	static constexpr float MAX_RADIUS = 5000.0f;

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

	static constexpr float SPEED = 15.0f;

	static constexpr float LENGTH = 2000.0f;

	// 移動方向
	VECTOR moveDir_;
	// 移動スピード
	float moveSpeed_;

	float radius_;

	//移動処理
	void Move(void);

};

