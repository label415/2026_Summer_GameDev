#pragma once
#include "CharactorBase.h"

class Camara;

class Player :
    public CharactorBase
{
public:

	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		JUMP,
	};

	enum class STATE
	{
		IDLE,
		RUN,
		FAST_RUN
	};

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 5.0f;
	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 10.0f;

	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };
	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };


	// コンストラクタ
	Player(void);

	// デストラクタ
	~Player(void);

	void Draw(void)override;

	// 解放
	void Release(void)override;

protected:

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

	// 衝突判定用カプセル上部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS =
	{ 0.0f, 160.0f, 0.0f };
	// 衝突判定用カプセル下部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS =
	{ 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };
	// 衝突判定用線分終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };
	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };
	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;
	// ジャンプ力
	static constexpr float POW_JUMP_INIT = 3500.0f;
	// 持続ジャンプ力
	static constexpr float POW_JUMP_KEEP = 400.0f;
	// ジャンプ受付時間
	static constexpr float TIME_JUMP_INPUT = 0.5f;


	//武器衝突判定用カプセル上部球体
	static constexpr VECTOR WEAPON_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };
	//武器衝突判定用カプセル下部球体
	static constexpr VECTOR WEAPON_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };
	// 衝突判定用カプセル球体半径
	static constexpr float WEAPON_CAPSULE_RADIUS = 100.0f;
	//武器フレーム
	static constexpr int WEAPON_FRAME = 58;
	//武器情報
	Transform  weponTrans_;

	// 操作
	void ProcessMove(void);
	void ProcessJump(void);

	// 衝突判定
	void CollisionReserve(void) override;

};

