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
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
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
		UP,
		RECOVERY,
		DIE,
		END,
	};

	//各部位のタグ
	enum class EFFECT {
		NONE,
		BLOOD,
		HP_ABSOLUTE,
	};

	// 攻撃コンボ状態
	enum class STATE_ATTACK_COMBO
	{
		COMBO_1,
		COMBO_2,
		COMBO_3,
		MAX
	};

	// 攻撃コンボ制御
	struct ATTACK_COMBO
	{
		// アニメーション種別
		ANIM_TYPE animType = ANIM_TYPE::IDLE;
		// コンボ受付開始ステップ
		float stepInputStart = 0.0f;
		// コンボ受付終了ステップ
		float stepInputEnd = 0.0f;
		// 衝突判定開始ステップ
		float stepCollisionStart = 0.0f;
		// 衝突判定終了ステップ
		float stepCollisionEnd = 0.0f;
		// アニメーション割り込みステップ
		float stepInterrupt = 0.0f;
		// 移動入力時の移動速度
		float moveSpeed = 0.0f;
		// 次のコンボ状態
		STATE_ATTACK_COMBO nextCombo = STATE_ATTACK_COMBO::MAX;
		// 次のコンボに必要なアクション
		std::function<bool(void)> actionNextCombo = nullptr;
		// 次のコンボに繋げるか
		bool isNextCombo = false;
		// ノックバックするか
		bool isKnockBack = false;
		// 追加の初期処理
		std::function<void(void)> extraInit = nullptr;
		// 追加の更新処理
		std::function<void(void)> extraUpdate = nullptr;
		// 追加の割込条件
		std::function<bool(ATTACK_COMBO&)> isExtraInterrupt = nullptr;
		// 追加の終了条件
		std::function<bool(void)> isExtraEnd = nullptr;
		// 更新ステップ
		float step = 0.0f;

		// コンボ受付有効ステップ
		bool IsValidCombo(float step) const
		{
			return step > stepInputStart
				&& step < stepInputEnd;
		}

		// 衝突判定有効ステップ
		bool IsValidCollsion(float step) const
		{
			return step > stepCollisionStart
				&& step < stepCollisionEnd;
		}

		// 割り込み有効ステップ
		bool IsValidInterrupt(float step) const
		{
			return step > stepInterrupt;
		}

	};

	//初期Y軸角度
	static constexpr float ANGLE_AXIS_Y = 180.0f;

	//回避Y軸角度
	static constexpr float AVOIDANCE_ANGLE_AXIS_Y = 100.0f;

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 4.0f;
	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 8.0f;

	//スタミナ回復速度
	static constexpr float RECOVERY_ST_SPEED = 20.0f;

	//ダッシュ時スタミナ消費量
	static constexpr float CONSUMPTION_ST_FAST_RUN = 25.0f;

	//回避時スタミナ消費量
	static constexpr float CONSUMPTION_ST_AVOIDANCE = 40.0f;

	//回避時スタミナ消費量
	static constexpr float CONSUMPTION_ST_ATTACK = 25.0f;

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

	const STATE GetState(void)const { return state_; }

	const float Geti(void)const { return i_; }

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

	// 攻撃コンボデータ
	std::map<STATE_ATTACK_COMBO, ATTACK_COMBO> atkComboData_;

	STATE_ATTACK_COMBO stateAtkCombo_;

	bool isComboNext_;

	//武器
	WeponBase* wepon_;

	//初期角度を保存
	Quaternion lastQrot_;

	EFFECT effType_;

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
	static constexpr VECTOR LOCK_POS2 = { 0.0f, 20.0f, 0.0f };
	static constexpr VECTOR LOCK_POS3 = { 0.0f, 20.0f, 0.0f };
	static constexpr VECTOR LOCK_POS4 = { 0.0f, 30.0f, 0.0f };

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
	void ProcessDie(void);

	// 衝突判定
	void CollisionReserve(void) override;

	//無敵
	bool isV_;

	float ct_;

	float i_;

	VECTOR LockPos;

	VECTOR GetInputDirection(void);

};

