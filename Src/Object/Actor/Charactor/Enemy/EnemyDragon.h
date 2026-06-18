#pragma once
#include "EnemyBase.h"

class EnemyDragon :
    public EnemyBase
{
public:
	// 状態
	enum class STATE
	{
		NONE,			//何もなし
		THINK,			//思考
		IDLE,			//待機
		ROAR,			//咆哮
		PATROL,			//探索
		CHASE,			//追跡
		FLYING,			//飛行中
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
		FLYING = 5,
		FLYING_ATTACK = 6,
		BRACELET_ATTACK = 4,
		MELEE_ATTACK = 2, 
		HOVER = 7,
		LANDS = 11,
		TAKEOFF = 12,
		DIE = 3
	};

	struct AttackShere
	{
		VECTOR pos_;
		VECTOR moveDir_;
		bool isAlive;
		float radius_;
		float moveSpeed_;
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

	//部位のカプセルフレーム
	struct FramePair {
		PATR_TAG patrTag;
		int top;
		int down;
	};


	// コンストラクタ
	EnemyDragon(const EnemyBase::EnemyData& data);
	// デストラクタ
	~EnemyDragon(void) override;
	// 描画
	void Draw(void) override;
	// 解放
	void Release(void)override;

	void HitDamage(bool isHit)override;
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
	// 更新系
	void UpdateProcess(void) override;
	void UpdateProcessPost(void) override;

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
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };
	// 地面衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };
	// 地面衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 30.0f;

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
	static constexpr float SPEED_MOVE = 15.0f;
	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 10.0f;

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

	// 状態
	STATE state_;

	//属性
	ATTRIBUTE attribute_;

	//攻撃時間
	float attackCnt_;

	// 更新ステップ
	float step_;

	//攻撃対象の情報を当たり判定から取得
	const ColliderBase* targetCollider_;

	// 状態遷移
	void ChangeState(STATE state);
	//何もなし
	void ChangeStateNone(void);
	//思考
	void ChangeStateThink(void);
	//待機
	void ChangeStateIdle(void);
	//咆哮
	void ChangeStateRoar(void);
	//探索
	void ChangeStatePatrol(void);
	//追跡
	void ChangeStateChase(void);
	//飛行中
	void ChangeStateFlying(void);
	//空中ブレス
	void ChangeStateFlyingAttack(void);
	//地上ブレス
	void ChangeStateBreathAttack(void);
	//噛みつき攻撃
	void ChangeStateMeleeAttack(void);
	//空中停止
	void ChangeStateHover(void);
	//上昇
	void ChangeStateTakeOff(void);
	//降下
	void ChangeStateLands(void);
	//死亡
	void ChangeStateDead(void);
	//終了
	void ChangeStateEnd(void);

	// 更新系
	void UpdateNone(void);
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdateRoar(void);
	void UpdatePatrol(void);
	void UpdateChase(void);
	void UpdateFlying(void);
	void UpdateFlyingAttack(void);
	void UpdateBreathAttack(void);
	void UpdateMeleeAttack(void);
	void UpdateHover(void);
	void UpdateTakeOff(void);
	void UpdateLands(void);
	void UpdateDead(void);
	void UpdateEnd(void);

	//対象コライダーの設定
	void SetTargetCollider(void);

	Transform colTransform_;

	void UpdateDebugImGui(void);
	
	VECTOR preMoverDir_;
};

