#pragma once
#include "WeponBase.h"
class Transform;
class ColliderModel;
class WeponFlameThrower :
    public WeponBase
{
public:
	//攻撃エフェクト
	enum class EFFECT_TYPE
	{
		NONE,
		BULLET,
		EXPLOSION,
	};

	// コンストラクタ
	WeponFlameThrower(const Transform& followTransform, const ColliderModel* colMod, const VECTOR moverDir, int followFrameId);
	// デストラクタ
	~WeponFlameThrower(void) override;
	// 更新
	void Update(void) override;
	// 描画
	void Draw(void)override;
	// 解放
	void Release(void)override;

protected:

	// 衝突判定用カプセル球体半径
	static constexpr float MIN_RADIUS = 50.0f;
	// 衝突判定用カプセル球体半径
	static constexpr float MAX_RADIUS = 1100.0f;

	//爆破開始時間
	static constexpr float EXPLOSION_STATO_TIME = 1.0f;

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

	static constexpr float SPEED = 30.0f;

	//エフェクトタイプ
	EFFECT_TYPE effectType_;

	// 移動方向
	VECTOR moveDir_;
	// 移動スピード
	float moveSpeed_;

	float radius_;

	//爆発開始時間
	float exState_;

	//移動処理
	void Move(void);

	const ColliderModel* ColMod_;

	//前フレームの座標
	VECTOR prePos_;

	void ChangerEffect(EFFECT_TYPE effectType);

};

