#pragma once
#include <map>
#include <vector>
#include <functional>
#include "CharactorBase.h"

class Camara;
class WeponBase;
class UISt;
class UIRecovery;

class Player :
	public CharactorBase
{
public:
	// アニメーションタイプ
	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		ROLLING,
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
		EVASION,
		DOWN,
		UP,
		RECOVERY,
	};

	// プレイヤー状態
	enum class STATE
	{
		IDLE,
		RUN,
		FAST_RUN,
		ATTACK,
		EVASION,
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

	// 初期Y軸角度
	static constexpr float ANGLE_AXIS_Y = 180.0f;
	// 回避Y軸角度
	static constexpr float AVOIDANCE_ANGLE_AXIS_Y = 100.0f;

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 6.0f;
	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 12.0f;
	// 移動速度(回避)
	static constexpr float SPEED_EVASION = 10.0f;

	// スタミナ回復速度
	static constexpr float RECOVERY_ST_SPEED = 20.0f;
	// ダッシュ時スタミナ消費量
	static constexpr float CONSUMPTION_ST_FAST_RUN = 20.0f;
	// 回避時スタミナ消費量
	static constexpr float CONSUMPTION_ST_EVASION = 40.0f;
	// 攻撃時スタミナ消費量
	static constexpr float CONSUMPTION_ST_ATTACK = 23.0f;
	//スタミナ枯渇時のクールタイム
	static constexpr float CT = 2.0f;

	// 敵本体接触時の被ダメージ
	static constexpr float DAMAGE_ENEMY_BODY = 20.0f;
	// 敵武器接触時の被ダメージ
	static constexpr float DAMAGE_ENEMY_WEAPON = 35.0f;

	// HP回復アイテム使用時の回復量
	static constexpr float HEAL_HP_AMOUNT = 40.0f;
	// 回復ボトル初期所持数
	static constexpr int INITIAL_RECOVERY_BOTTLE_COUNT = 6;
	// 1回あたりの回復ボトル消費量
	static constexpr int CONSUME_RECOVERY_BOTTLE_COUNT = 1;

	// プレイヤー被ダメージSE音量
	static constexpr int SE_VOLUME_DAMAGE = 50;
	// 歩きSE音量
	static constexpr int SE_VOLUME_WALK = 50;
	// 走りSE音量
	static constexpr int SE_VOLUME_RUN = 50;
	// 回避SE音量
	static constexpr int SE_VOLUME_EVASION = 50;
	// 攻撃SE音量
	static constexpr int SE_VOLUME_ATTACK = 30;
	// 回復SE音量
	static constexpr int SE_VOLUME_RECOVERY = 50;

	// 被弾時出血エフェクトスケール
	static constexpr VECTOR EFFECT_BLOOD_SCALE = { 5.0f, 5.0f, 5.0f };
	// 回復エフェクトスケール
	static constexpr float EFFECT_RECOVERY_SCALE = 10.0f;
	// 回復エフェクトY軸オフセット
	static constexpr float EFFECT_RECOVERY_OFFSET_Y = 100.0f;

	// 敵接触時の押し出し量
	static constexpr int PUSH_BACK_POWER = 20;

	// ターゲット追従時の回転Slerp補間係数
	static constexpr float ROT_TARGET_SLERP_RATE = 0.1f;
	// 攻撃踏み込み時のLerp補間係数
	static constexpr float ATTACK_STEP_LERP_RATE = 0.2f;

	// 死亡時のフェード進行速度
	static constexpr float DIE_FADE_SPEED = 0.3f;
	// 死亡ステート完了し終了遷移する閾値
	static constexpr float DIE_END_THRESHOLD = 1.8f;

	// アニメーション再生速度(FPS)
	static constexpr float ANIM_SPEED_IDLE = 30.0f;
	static constexpr float ANIM_SPEED_RUN = 25.0f;
	static constexpr float ANIM_SPEED_FAST_RUN = 30.0f;
	static constexpr float ANIM_SPEED_ATTACK_1 = 40.0f;
	static constexpr float ANIM_SPEED_ATTACK_2 = 40.0f;
	static constexpr float ANIM_SPEED_ATTACK_3 = 40.0f;
	static constexpr float ANIM_SPEED_EVASION = 80.0f;
	static constexpr float ANIM_SPEED_DOWN = 50.0f;
	static constexpr float ANIM_SPEED_UP = 100.0f;
	static constexpr float ANIM_SPEED_RECOVERY = 40.0f;

	// 攻撃時SE再生ステップ
	static constexpr float ATTACK_SE_TRIGGER_STEP = 10.0f;
	// 回復アクション発動ステップ
	static constexpr float RECOVERY_TRIGGER_STEP = 10.0f;

	// コンボ1 (横切り)
	static constexpr float COMBO1_STEP_INPUT_START = 20.0f;
	static constexpr float COMBO1_STEP_INPUT_END = 50.0f;
	static constexpr float COMBO1_STEP_COL_START = 15.0f;
	static constexpr float COMBO1_STEP_COL_END = 38.0f;
	static constexpr float COMBO1_STEP_INTERRUPT = 55.0f;
	static constexpr float COMBO1_MOVE_SPEED = 8.0f;

	// コンボ2 (縦切り)
	static constexpr float COMBO2_STEP_INPUT_START = 15.0f;
	static constexpr float COMBO2_STEP_INPUT_END = 40.0f;
	static constexpr float COMBO2_STEP_COL_START = 18.0f;
	static constexpr float COMBO2_STEP_COL_END = 32.0f;
	static constexpr float COMBO2_STEP_INTERRUPT = 50.0f;
	static constexpr float COMBO2_MOVE_SPEED = 5.0f;

	// コンボ3 (回転)
	static constexpr float COMBO3_STEP_INPUT_START = 0.0f;
	static constexpr float COMBO3_STEP_INPUT_END = 0.0f;
	static constexpr float COMBO3_STEP_COL_START = 26.0f;
	static constexpr float COMBO3_STEP_COL_END = 34.0f;
	static constexpr float COMBO3_STEP_INTERRUPT = 95.0f;
	static constexpr float COMBO3_MOVE_SPEED = 12.0f;

	// 武器アタッチ先フレーム番号
	static constexpr int WEAPON_ATTACH_FRAME_NO = 48;
	// 被弾エフェクト発生フレーム番号
	static constexpr int HIT_EFFECT_FRAME_NO = 2;
	// 起き上がり時LockPosのY軸上昇速度
	static constexpr float UP_LOCK_POS_Y_ADD = 0.7f;

	// HP UI X座標 (画面幅に対する除数)
	static constexpr int UI_HP_POS_X_DIV = 4;
	// HP UI Y座標
	static constexpr int UI_HP_POS_Y = 20;
	// スタミナ UI Y座標
	static constexpr int UI_ST_POS_Y = 60;
	// UIスケール
	static constexpr float UI_SCALE_X = 0.7f;
	static constexpr float UI_SCALE_Y = 2.9f;
	static constexpr float UI_SCALE_Z = 4.0f;

	// 初期スポーン位置
	static constexpr VECTOR INITIAL_POS = { 0.0f, 0.0f, -500.0f };

	// 攻撃判定発生時間
	static constexpr float STATE_ATTACK_CILLIDER = 18.0f;

	// コンストラクタ
	Player(void);

	// デストラクタ
	~Player(void);

	// 描画処理
	void Draw(void) override;

	// リソースの解放処理
	void Release(void) override;

	// 装備中の武器ポインタを取得
	const WeponBase* GetWepon(void) const { return wepon_; }

	// HPおよび各種UIの描画
	void DrawHp(void) override;

	// 状態を取得
	const STATE GetState(void) const { return state_; }

	// 無敵状態かどうかを取得
	const bool GetIsInvincible(void) const { return isVinclible_; }

	// 死亡演出タイム取得
	const float GetDesath(void)const { return deathAnimationTime_; }
protected:
	// スタミナ UI
	UISt* uiSt_;

	// 回復
	UIRecovery* uiRecovery_;

	// 状態
	STATE state_;

	// プレイヤーリソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標のトランスフォーム初期化
	void InitTransform(void) override;

	// コライダー（線分・カプセル）の初期化
	void InitCollider(void) override;

	// アニメーションコントローラーの構築とアニメーション登録
	void InitAnimation(void) override;

	// ロード後のコンボパラメータや内部変数の初期化
	void InitPost(void) override;

	// プレイヤーの主更新処理
	void UpdateProcess(void) override;

	// プレイヤー更新後の後処理
	void UpdateProcessPost(void) override;

	// 当たり判定衝突時の更新処理
	void UpdateHitCollider(void) override;
private:
	// 攻撃コンボデータ
	std::map<STATE_ATTACK_COMBO, ATTACK_COMBO> atkComboData_;

	// 現在の攻撃コンボ状態
	STATE_ATTACK_COMBO stateAtkCombo_;

	// 攻撃コンボの更新ステップ
	bool isComboNext_;

	// 武器
	WeponBase* wepon_;

	// 初期角度を保存
	Quaternion lastQrot_;

	// エフェクト種別
	EFFECT effType_;

	//アニメーションを固定化する座標
	static constexpr VECTOR LOCK_POS1 = { 0.0f, 78.0f, 0.0f };
	static constexpr VECTOR LOCK_POS2 = { 0.0f, 20.0f, 0.0f };
	static constexpr VECTOR LOCK_POS3 = { 0.0f, 20.0f, 0.0f };
	static constexpr VECTOR LOCK_POS4 = { 0.0f, 30.0f, 0.0f };

	//アニメーションを固定化するフレーム
	static constexpr int LOCK_FRAME_NO = 0;

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

	// 無敵時間
	static constexpr float INVINCIBLE_TIME = 10.0f;

	// 移動入力判定および移動処理（通常歩行・ダッシュ）
	void ProcessMove(void);

	// 攻撃アクションおよびコンボ継続処理
	void ProcessAttack(void);

	// 回避アクション処理
	void ProcessEvasion(void);

	// ダウンおよび起き上がり復帰処理
	void ProcessDownUp(void);

	// 回復処理
	void ProcessRecovery(void);

	// 死亡演出処理
	void ProcessDie(void);

	// 無敵
	bool isVinclible_;
	// 無敵時間
	float invincibleTimer_;

	// スタミナ回復時間
	float stRecoverTime_;

	// 死亡演出時間
	float deathAnimationTime_;

	// アニメーションを固定化する座標
	VECTOR animLockPos_;

	// 入力方向を取得
	VECTOR GetInputDirection(void);
};
