#pragma once
#include <map>
#include <memory>
#include <vector>
#include "../Common/Quaternion.h"
#include "../Template/Vector2Template.h"
#include "Actor/ActorBase.h"

class ColliderCapsule;

class Camera : public ActorBase
{
public:
	
	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,
		FREE,
		FOLLOW,
		TARGET_LOCK,
	};

	// コンストラクタ
	Camera(void);

	// デストラクタ
	~Camera(void) override = default;

	// 更新
	void Update(void) override {}

	// 描画前のカメラ設定
	void SetBeforeDraw(void);

	// デバッグ用描画
	void DrawDebug(void);

	// 解放
	void Release(void)override;

	// 座標の取得
	const VECTOR& GetPos(void) const { return transform_.pos; }

	// 角度の取得
	const VECTOR& GetAngles(void) const { return angles_; }
	const Quaternion& GetQuaRot(void) const { return transform_.quaRot; }

	// X回転を抜いたY軸のみのカメラ角度
	const Quaternion& GetQuaRotY(void) const { return rotY_; }

	// 注視点の取得
	const VECTOR& GetTargetPos(void) const { return fixationPos_; }

	// カメラの前方方向
	VECTOR GetForward(void) const { return VNorm(VSub(fixationPos_, transform_.pos)); }

	// カメラモードの変更
	void ChangeMode(MODE mode);

	//カメラモードの取得
	const MODE& GetCameraMode(void) const { return mode_; }

	// 追従対象の設定
	void SetFollow(const std::weak_ptr<const ActorBase>& follow) { followActor_ = follow; }

	// ロックオン候補のコライダーを格納する
	void AddTargetCollider(const std::weak_ptr<ColliderCapsule>& target);

	// ロックオン候補リストをクリアする
	void ClearTargetColliders(void);

	// 現在ロックオンしている対象の座標ポインタを取得（非ロックオン時は nullptr）
	const VECTOR* GetLockOnTargetPos(void) const;

	// ロックオン対象の選別・切り替え更新
	void UpdateLockOn(void);

protected:

	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override {}

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override {}

	// 初期化後の個別処理
	void InitPost(void) override;

	// 当たり判定衝突時の更新処理
	void UpdateHitCollider(void) override;

private:

	// カメラの初期座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 200.0f, -500.0f };

	// カメラの初期角度
	static constexpr VECTOR DEFAULT_ANGLES = { 0.0f, 0.0f, 0.0f };

	// ロックオン最大距離・視野角
	static constexpr float MAX_LOCKON_DIFF = 2000.0f;
	static constexpr float LOCKON_VIEW_ANGLE = 80.0f;

	// ロックオンUIの中心位置（UV基準）
	static constexpr Vector2F LOCKON_UI_CENTER = { 0.5f, 0.5f };
	//ロックオンUIの大きさ
	static constexpr float LOCKON_UI_SIZE = 50.0f;

	// カメラの回転量（キーボード・パッド）
	const float ROT_POW_DEG = 2.0f;
	const float ROT_POW_RAD = ROT_POW_DEG * DX_PI_F / 180.0f;

	// カメラの回転量（マウス）
	const float MROT_POW_DEG = 0.1f;
	const float MROT_POW_RAD = MROT_POW_DEG * DX_PI_F / 180.0f;

	// カメラの移動スピード
	static constexpr float SPEED = 20.0f;

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 10.0f;
	static constexpr float VIEW_FAR = 20000.0f;

	// 追従位置からカメラ位置までの相対座標
	static constexpr VECTOR FOLLOW_CAMERA_LOCAL_POS = { 0.0f, 200.0f, -350.0f };
	// 追従位置から注視点までの相対座標
	static constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 0.0f, 0.0f, 500.0f };

	// 追従位置からカメラ位置までの相対座標
	static constexpr VECTOR LOCKON_CAMERA_LOCAL_POS = { 0.0f, 200.0f, -350.0f };
	// 追従位置から注視点までの相対座標
	static constexpr VECTOR LOCKON_TARGET_LOCAL_POS = { 0.0f, 100.0f, 0.0f };

	// カメラのX回転上限度角
	static constexpr float LIMIT_X_UP_RAD = 60.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DW_RAD = 40.0f * (DX_PI_F / 180.0f);

	// ロックオン切り替え方向
	enum class SWITCH_DIR {
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	// 追尾時のカメラ補間率
	static constexpr float CAMERA_FOLLOW_RATE = 0.5f;

	// ロックオン時のカメラ補間率
	static constexpr float CAMERA_LOCKON_RATE = 0.1f;

	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION_CAMERA = 10;
	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 1.0f;
	// 衝突判定用球体半径
	static constexpr float COL_SPHERE_RADIUS = 50.0f;

	// ロックオン候補のカプセルコライダーリスト
	std::vector<std::weak_ptr<ColliderCapsule>> targetColliders_;

	// 現在ロックオンしている対象のカプセルコライダー
	std::weak_ptr<ColliderCapsule> targetCollider_;

	// カメラが追従対象とするアクター
	std::weak_ptr<const ActorBase> followActor_;

	// カメラモード
	MODE mode_;

	// カメラ角度(rad)
	VECTOR angles_;

	// カメラ角度(Y軸のみ)
	Quaternion rotY_;

	// 注視点
	VECTOR fixationPos_;

	// ロックオンUIハンドル
	int lockOnImg_;

	// 右スティックの前回入力保持用
	Vector2F prevStick_;

	// カメラを初期位置に戻す
	void SetDefault(void);

	// 追従対象との位置同期を取る
	void SyncFollow(void);

	// ターゲットとの位置同期を取る
	void SyncLockOn(void);

	// カメラ操作
	void ProcessRot(bool isLimit);
	void ProcessMove(void);

	// カメラ回転(キーボード)
	void RotMouse(bool isLimit);

	// カメラ回転(ゲームパッド)
	void RotGamePad(bool isLimit);

	// モード別更新ステップ
	void SetBeforeDrawFixedPoint(void) {}
	void SetBeforeDrawFree(void);
	void SetBeforeDrawFollow(void);
	void SetBeforeDrawTargetLockOn(void);

	// 格納されたコライダーリストから条件に合う対象を検索する
	std::weak_ptr<ColliderCapsule> FindLockOnTarget(SWITCH_DIR switchDir) const;

	// ターゲット切り替え入力
	void HandleTargetSwitching(void);

};
