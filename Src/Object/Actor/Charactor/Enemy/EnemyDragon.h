#pragma once
#include <string>
#include <vector>
#include "EnemyBase.h"

class WeponBracelet;
class WeponFlameThrower;

class EnemyDragon : public EnemyBase
{
public:
	// 状態
	enum class STATE
	{
		NONE,			//何もなし
		THINK,			//思考
		IDLE,			//待機
		ROAR,			//咆哮
		CHARGE,			//突進
		PATROL,			//探索
		FLYING,			//飛行中
		FALLING_ATTACK, //落下攻撃
		FLYING_ATTACK,	//空中ブレス
		BRACELET_ATTACK,//地上ブレス
		MELEE_ATTACK,	//噛みつき攻撃
		HOVER,			//空中停止
		TAKEOFF,		//上昇
		LANDS,			//降下
		DEAD,			//死亡
		END				//終了
	};

	enum class ATTRIBUTE
	{
		NONE,
		ABOVE_GROUND,
		AIR
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		ROAR = 1,
		IDLE = 8,
		WALK = 13,
		CHARGE = 13,
		FLYING = 5,
		FALLING_ATTACK = 2,
		FLYING_ATTACK = 6,
		BRACELET_ATTACK = 4,
		MELEE_ATTACK = 2,
		HOVER = 7,
		LANDS = 11,
		TAKEOFF = 12,
		DIE = 3
	};

	//各部位のタグ
	enum class PATR_TAG {
		HEAD,
		TAIL,
		BODY,
		HAND,
		LEG,
		NECK,
	};

	//各部位のタグ
	enum class EFFECT {
		NONE,
		ROAT,
		FALLING_ATTACK,
		CHARGE,
		BLOOD
	};

	//部位のカプセルフレーム
	struct FramePair {
		PATR_TAG patrTag;
		int top;
		int down;
	};

	// アニメーション再生速度
	static constexpr float ANIM_SPEED_IDLE = 30.0f;
	static constexpr float ANIM_SPEED_WALK = 30.0f;
	static constexpr float ANIM_SPEED_CHARGE = 50.0f;
	static constexpr float ANIM_SPEED_FLYING = 30.0f;
	static constexpr float ANIM_SPEED_BRACELET_ATTACK = 17.0f;
	static constexpr float ANIM_SPEED_HOVER = 30.0f;
	static constexpr float ANIM_SPEED_TAKEOFF = 30.0f;
	static constexpr float ANIM_SPEED_LANDS = 30.0f;
	static constexpr float ANIM_SPEED_DIE = 30.0f;
	static constexpr float ANIM_SPEED_FLYING_ATTACK = 15.0f;
	static constexpr float ANIM_SPEED_ROAR = 30.0f;
	static constexpr float ANIM_SPEED_MELEE_ATTACK = 20.0f;

	// サウンド音量・ピッチ
	static constexpr int SE_VOLUME_DEFAULT = 50;
	static constexpr int SE_VOLUME_TAKEOFF = 70;
	static constexpr int SE_VOLUME_PATROL = 80;
	static constexpr int SE_VOLUME_FALLING = 100;
	static constexpr float SE_SPEED_PATROL = 1.2f;

	// エフェクト定数
	static constexpr VECTOR EFFECT_ROAR_SCALE = { 250.0f, 250.0f, 250.0f };
	static constexpr VECTOR EFFECT_CHARGE_SCALE = { 150.0f, 150.0f, 150.0f };
	static constexpr VECTOR EFFECT_FALLING_SCALE = { 150.0f, 150.0f, 150.0f };
	static constexpr VECTOR EFFECT_BLOOD_SCALE = { 7.5f, 7.5f, 7.5f };

	// AI行動判定閾値
	static constexpr float AI_DIST_NEAR = 800.0f;
	static constexpr float AI_DIST_MID = 1000.0f;
	static constexpr float AI_DIST_FAR = 2000.0f;

	static constexpr int AI_PROB_TAKEOFF = 30;
	static constexpr int AI_PROB_MELEE_IN_NEAR = 40;
	static constexpr int AI_PROB_PATROL_IN_MID = 20;
	static constexpr int AI_PROB_CHARGE_IN_MID = 60;
	static constexpr int AI_PROB_LANDS = 20;

	// 突進移行ステップ
	static constexpr float CHARGE_SE_START_STEP = 107.0f;
	static constexpr float CHARGE_SE_END_STEP = 109.0f;
	static constexpr float CHARGE_TRANS_STEP = 109.0f;

	// 落下攻撃時のジャンプ加算速度
	static constexpr float FALLING_JUMP_SPEED = 5.0f;
	static constexpr float FALLING_ANIM_LOOP_START = 15.0f;
	static constexpr float FALLING_ANIM_LOOP_END = 20.0f;

	// 空中ブレス
	static constexpr float FLYING_ATTACK_ANIM_STATE_TIME = 90.0f;
	static constexpr float FLYING_ATTACK_FIRE_STEP = 120.0f;
	static constexpr float FLYING_ATTACK_END_STEP = 180.0f;

	// 地上ブレス
	static constexpr float BREATH_SE2_START_STEP = 24.0f;
	static constexpr float BREATH_SE2_END_STEP = 26.0f;
	static constexpr float BREATH_ATTACK_START_STEP = 27.0f;
	static constexpr float BREATH_ANIM_LOOP_START = 27.0f;
	static constexpr float BREATH_ANIM_LOOP_END = 30.0f;
	static constexpr float BREATH_ATTACK_DURATION = 2.0f;
	static constexpr float BREATH_WEAPON_END_STEP = 40.0f;
	static constexpr float BREATH_ATTACK_END_STEP = 60.0f;

	// 噛みつき攻撃
	static constexpr float MELEE_ATTACK_SE_START_STEP = 20.0f;
	static constexpr float MELEE_MOVE_DIR_LOCK_STEP = 20.0f;

	// 上昇時の上昇速度
	static constexpr float TAKEOFF_SPEED = 690.0f;

	// 死亡演出
	static constexpr float DIE_FADE_SPEED = 0.3f;
	static constexpr float DIE_END_THRESHOLD = 1.8f;

	// 口元フレーム
	static constexpr int FRAME_NO_MOUTH = 28;

	// 首・尻尾の半径
	static constexpr float NECK_TAIL_RADIUS = 80.0f;
	// 胴体コライダのY軸オフセット
	static constexpr float BODY_COL_OFFSET_Y = -90.0f;
	// 首・尻尾コライダのY軸オフセット
	static constexpr float NECK_TAIL_COL_OFFSET_Y = -50.0f;

	// プレイヤー攻撃による被ダメージ量
	static constexpr float DAMAGE_HIT_PLAYER_WEAPON = 8.0f;
	// HP UI Y座標オフセット
	static constexpr float UI_HP_OFFSET_Y = 75.0f;
	// HP UI スケール
	static constexpr float UI_HP_SCALE_X = 1.3f;
	static constexpr float UI_HP_SCALE_Y = 1.57f;
	static constexpr float UI_HP_SCALE_Z = 2.0f;

	//行動乱数
	static constexpr int STATE_RAND = 100;

	//行動終了までの乱数
	static constexpr int STATE_END_RAND = 2;

	// コンストラクタ
	EnemyDragon(const EnemyBase::EnemyData& data);

	// デストラクタ
	~EnemyDragon(void) override;

	// 描画処理
	void Draw(void) override;

	// 解放処理
	void Release(void) override;

	// ダメージ処理
	void HitDamage(bool isHit) override;

	// HPUI表示
	void DrawHp(void) override;
protected:
	// ロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// コライダー初期化
	void InitCollider(void) override;

	// アニメーション初期化
	void InitAnimation(void) override;

	// その他初期化
	void InitPost(void) override;

	// 毎フレームの主更新処理
	void UpdateProcess(void) override;

	// 毎フレームの後更新処理
	void UpdateProcessPost(void) override;
	
	// 地面とのカプセル衝突判定と押し出し処理
	void CollisionCapsule(void) override;
private:
	// モデルの大きさ
	static constexpr float SCALE = 0.4f;
	// モデルの回転調整
	static constexpr VECTOR DEFAULT_LOCAL_ROT =
	{ 0.0f, 180.0f * DX_PI_F / 180.0f, 0.0f };

	// 地面衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };
	// 地面衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 地面衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 300.0f };
	// 地面衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 110.0f, -300.0f };
	// 地面衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 200.0f;

	// 攻撃判定用カプセル球体半径
	static constexpr float HIT_RADIUS = 50.0f;

	// 胴体衝突判定用カプセル球体半径
	static constexpr float BODY_RADIUS = 100.0f;

	// 最高高度
	static constexpr float MAX_TAKE = 500.0f;

	//噛みつき攻撃判定発生時間
	static constexpr float MELEE_ATTACK_CILLIDER = 22.0f;

	//アニメーションを固定化する座標
	static constexpr VECTOR LOCK_POS = { 0.0f, 2.0f, 0.0f };
	//アニメーションを固定化するフレーム
	static constexpr int LOCK_FRAME_NO = 1;

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 10.0f;
	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 20.0f;

	//各部位のフレーム
	static constexpr FramePair ENEMY_CAPSULE_FRAMES[] =
	{
		// 胴体
		{ PATR_TAG::BODY, 1, 8 },
		// 首上
		{ PATR_TAG::NECK, 10, 14 },
		// 頭
		{ PATR_TAG::HEAD, 15, 30 },
		// 尻尾
		{ PATR_TAG::TAIL, 117, 125 }, { PATR_TAG::TAIL, 125, 132 },
		// 手
		{ PATR_TAG::HAND, 75, 76 }, { PATR_TAG::HAND, 37, 38 },
		// 足
		{ PATR_TAG::LEG, 2, 4 }, { PATR_TAG::LEG, 111, 113 }
	};

	// ボスの選択距離
	static constexpr float ENEMY_ATTACK[] = { 500.0f,1000.0f, 1500.0f };

	// 状態
	STATE state_;

	//属性
	ATTRIBUTE attribute_;

	//エフェクト
	EFFECT effectType_;

	//攻撃時間
	float attackCnt_;

	// 更新ステップ
	float step_;

	//攻撃対象の情報を当たり判定から取得
	const ColliderBase* targetCollider_;

	// 行動遷移
	void ChangeState(STATE state);

    // 各ステート初期化ハンドラ
	void ChangeStateNone(void);
	void ChangeStateThink(void);
	void ChangeStateIdle(void);
	void ChangeStateRoar(void);
	void ChangeStateCharge(void);
	void ChangeStatePatrol(void);
	void ChangeStateFlying(void);
	void ChangeStateFallingAttack(void);
	void ChangeStateFlyingAttack(void);
	void ChangeStateBreathAttack(void);
	void ChangeStateMeleeAttack(void);
	void ChangeStateHover(void);
	void ChangeStateTakeOff(void);
	void ChangeStateLands(void);
	void ChangeStateDead(void);
	void ChangeStateEnd(void);

	
	// 各ステート更新ハンドラ
	void UpdateNone(void);
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdateRoar(void);
	void UpdateCharge(void);
	void UpdatePatrol(void);
	void UpdateFallingAttack(void);
	void UpdateFlying(void);
	void UpdateFlyingAttack(void);
	void UpdateBreathAttack(void);
	void UpdateMeleeAttack(void);
	void UpdateHover(void);
	void UpdateTakeOff(void);
	void UpdateLands(void);
	void UpdateDead(void);
	void UpdateEnd(void);

    // プレイヤーのコライダー情報をキャッシュする
	void SetTargetCollider(void);

	// コライダートランスフォーム
	Transform colTransform_;

	// ImGuiデバッグ表示の更新
	void UpdateDebugImGui(void);

	// 前回の移動方向
	VECTOR preMoverDir_;

	// 被弾後の無敵時間（秒）
	static constexpr float INVINCIBLE_TIME = 1.0f;

	// 無敵フラグとタイマー
	bool isInvincible_ = false;
	float invincibleTimer_ = 0.0f;
};
