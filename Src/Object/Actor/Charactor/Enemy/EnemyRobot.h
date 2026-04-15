#pragma once
#include <DxLib.h>
#include "EnemyBase.h"

class UISurprise;

class EnemyRobot : public EnemyBase
{
public:
	// 状態
	enum class STATE
	{
		NONE,
		THINK,
		IDLE,
		PATROL,
		SURPRISE,
		ALERT,
		CHASE,
		ATTACK_KICK,
		ATTACK_SHOOT,
		ESCAPE,
		DEAD,
		KNOCKBACK,
		END
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		DANCE = 0,
		DIE = 1,
		HIT = 3,
		IDLE = 5,
		KICK = 9,
		RUN = 12,
		SHOOT = 13,
		WALK = 15,
	};

	struct AttackShere
	{
		VECTOR pos_;
		VECTOR moveDir_;
		bool isAlive;
		float radius_;
		float moveSpeed_;
	};

	// コンストラクタ
	EnemyRobot(const EnemyBase::EnemyData& data);
	// デストラクタ
	~EnemyRobot(void) override;
	// 描画
	void Draw(void) override;
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
	static constexpr float SCALE = 0.5f;
	// モデルの回転調整
	static constexpr VECTOR DEFAULT_LOCAL_ROT =
	{ 0.0f, 180.0f * DX_PI_F / 180.0f, 0.0f };
	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS =
	{ 0.0f, 80.0f, 0.0f };
	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS =
	{ 0.0f, -10.0f, 0.0f };
	// 衝突判定(移動範囲)用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_MOVE_POS =
	{ 0.0f, 80.0f, 400.0f };
	// 衝突判定(移動範囲)用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_MOVE_POS =
	{ 0.0f, -10.0f, 400.0f };
	// 衝突判定用カプセル上部球体
		static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS =
	{ 0.0f, 40.0f, 80.0f };
	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS =
	{ 0.0f, 40.0f, -40.0f };
	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 30.0f;

	// 巡回時の視野モデルの大きさ
	static constexpr VECTOR VIEW_RANGE_SCL = { 8.0f, 4.0f, 2.0f };
	// 巡回時の視野モデルの傾きX
	static constexpr float VIEW_RANGE_ROT_X = 26.0f * DX_PI_F / 180.0f;
	static constexpr float VIEW_RANGE_LOCAL_ROT_X = 90.0f * DX_PI_F / 180.0f;
	// 巡回時の視野モデル位置同期用フレーム番号
	static constexpr int VIEW_RANGE_SYNC_FRAME_IDX = 6;
	// 巡回時の視野の広さ
	static constexpr float VIEW_RANGE_PATROL = 600.0f;
	// 巡回時の視野角
	static constexpr float VIEW_ANGLE_PATROL = 16.0f;

	//近接攻撃時の視野角
	static constexpr float VIEW_ANGLE_ATTACK = 180.0f;
	//攻撃時の視野の広さ
	static constexpr float VIEW_RANGE_ATTACK = 360.0f;
	//攻撃のクールタイム
	static constexpr float ATTACK_CT_TIME = 0.8f;
	float attackKickCnt;
	bool IsCt;

	//遠距離攻撃用球体速度
	static constexpr float ATTACK_SHOT_SPEED = 50.0f;
	//遠距離攻撃用球体半径
	static constexpr float ATTACK_SHOT_RADIUS = 40.0f;
	//遠距離攻撃開始時間
	static constexpr int ATTACK_SHOT_TIME = 2.0f;
	AttackShere attackShere;
	float changeShotCnt;

	// 状態
	STATE state_;
	// 更新ステップ
	float step_;

	// 現在、移動先としている巡回先ルートのインデックス
	int activeWayPointIndex_;
	// 現在、移動先としている巡回先座標
	VECTOR nextWayPoint_;

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStateThink(void);
	void ChangeStateIdle(void);
	void ChangeStatePatrol(void);
	void ChangeStateSurprise(void);
	void ChangeStateAlert(void);
	void ChangeStateChase(void);
	void ChangeStateAttackKick(void);
	void ChangeStateAttackShoot(void);
	void ChangeStateEscape(void);
	void ChangeStateDead(void);
	void ChangeStateKnockBack(void);
	void ChangeStateEnd(void);
	// 更新系
	void UpdateNone(void);
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdatePatrol(void);
	void UpdateSurprise(void);
	void UpdateAlert(void);
	void UpdateChase(void);
	void UpdateAttackKick(void);
	void UpdateAttackShoot(void);
	void UpdateEscape(void);
	void UpdateDead(void);
	void UpdateKnockBack(void);
	void UpdateEnd(void);

	// 巡回ルートの移動方向を設定する
	void SetMoveDirPatrol(void);

	//視野角とプレイヤーの衝突判定
	bool InSearchConeModel(void);

	// 視野範囲用トランスフォーム
	Transform viewRangeTransform_;

	Quaternion rot1;
	Quaternion rot2;
	float Cnt;

	UISurprise* uiSurprise_;
};