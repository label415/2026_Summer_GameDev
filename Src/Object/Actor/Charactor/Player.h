#pragma once
#include "CharactorBase.h"

class Camara;
class WeponBase;
class UISt;
class UIRecovery;

class Player :
    public CharactorBase
{
public:

	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		ROLLING,
		ATTACK,
		AVOIDANCE,
		DOWN,
		UP,
		RECOVERY,
	};

	enum class STATE
	{
		IDLE,
		RUN,
		FAST_RUN,
		ATTACK,
		AVOIDANCE,
		DOWN,
		RECOVERY
	};

	//初期Y軸角度
	static constexpr float ANGLE_AXIS_Y = 180.0f;

	//回避Y軸角度
	static constexpr float AVOIDANCE_ANGLE_AXIS_Y = 100.0f;

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 4.0f;
	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 8.0f;

	//スタミナ最大値
	static constexpr float MAX_ST = 100.0f;

	//スタミナ最小値
	static constexpr float MIN_ST = -15.0f;
	//スタミナ最小値
	static constexpr float MIN_ST_FAST_RUN = 10.0f;

	//スタミナ回復速度
	static constexpr float RECOVERY_ST_SPEED = 20.0f;

	//ダッシュ時スタミナ消費量
	static constexpr float CONSUMPTION_ST_FAST_RUN = 25.0f;

	//回避時スタミナ消費量
	static constexpr float CONSUMPTION_ST_AVOIDANCE = 40.0f;

	//回避時スタミナ消費量
	static constexpr float CONSUMPTION_ST_ATTACK = 30.0f;

	//攻撃判定発生時間
	static constexpr float STATE_ATTACK_CILLIDER = 18.0f;

	// コンストラクタ
	Player(void);

	// デストラクタ
	~Player(void);

	void Draw(void)override;

	// 解放
	void Release(void)override;

	//ダメージ判定
	void HitDamage(bool isHit)override;

	//武器情報取得
	const WeponBase* GetWepon(void)const { return wepon_; }

	//HP描画
	void DrawHp(void)override;

protected:

	// スタミナ UI
	UISt * uiSt_;

	// 回復
	UIRecovery* uiRecovery_;

	//状態
	STATE state_;

	// リソースロード
	void InitLoad(void)override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void)override;

	// 衝突判定の初期化
	void InitCollider(void)override;

	// アニメーションの初期化
	void InitAnimation(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

	// 更新系
	void UpdateProcess(void)override;
	void UpdateProcessPost(void)override;

private:

	//武器
	WeponBase* wepon_;

	//スタミナ
	float st_;

	//初期角度を保存
	Quaternion lastQrot_;

	// 衝突判定用カプセル上部球体(回避時)
	static constexpr VECTOR COL_CAPSULE_TOP_AVOIDANCE_LOCAL_POS =
	{ 0.0f, 80.0f, 0.0f };
	// 衝突判定用カプセル下部球体(回避時)
	static constexpr VECTOR COL_CAPSULE_DOWN_AVOIDANCE_LOCAL_POS =
	{ 0.0f, 30.0f, 0.0f };
	// 衝突判定用線分開始(回避時)
	static constexpr VECTOR COL_LINE_AVOIDANCE_START_LOCAL_POS =
	{ 0.0f, 40.0f, 0.0f };
	// 衝突判定用線分終了(回避時)
	static constexpr VECTOR COL_LINE_AVOIDANCE_END_LOCAL_POS =
	{ 0.0f, -10.0f, 0.0f };

	//アニメーションを固定化する座標
	static constexpr VECTOR LOCK_POS1 = { 0.0f, 78.0f, 0.0f };
	static constexpr VECTOR LOCK_POS2 = { 0.0f, 50.0f, 0.0f };

	//アニメーションを固定化するフレーム
	static constexpr int LOCK_FRAME_NO = 0;

	//スタミナ枯渇時のクールタイム
	static constexpr float CT = 2.0f;

	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };
	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };
	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };
	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;

	// 操作
	void ProcessMove(void);
	void ProcessAttack(void);
	void ProcessAvoidance(void);
	void ProcessDownUp(void);
	void ProcessRecovery(void);

	// 衝突判定
	void CollisionReserve(void) override;

	//無敵
	bool isV_;

	float ct_;

};

