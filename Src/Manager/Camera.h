#pragma once
#include <map>
#include <vector>
#include "../Common/Quaternion.h"
#include "../Object/Actor/ActorBase.h"

class Transform;

class Camera : public ActorBase
{
public:
	// カメラの初期座標
	static constexpr VECTOR DERFAULT_POS = { 0.0f, 200.0f, -500.0f };
	
	// カメラの初期角度
	static constexpr VECTOR DERFAULT_ANGLES = { 
		0.0f, 0.0f, 0.0f
	};

	// カメラの回転量
	const float ROT_POW_DEG = 2.0f;
	const float ROT_POW_RAD = ROT_POW_DEG * DX_PI_F / 180.0f;

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
	static constexpr VECTOR LOCKON_TARGET_LOCAL_POS = { 0.0f, 0.0f, 500.0f };

	// カメラのX回転上限度角
	static constexpr float LIMIT_X_UP_RAD = 60.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DW_RAD = 40.0f * (DX_PI_F / 180.0f);
	
	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,
		FREE,
		FOLLOW,
		TARGET_ROCKE,
	};

	// コンストラクタ
	Camera(void);

	// デストラクタ
	~Camera(void)override;

	// 更新
	void Update(void)override;

	// 描画前のカメラ設定
	void SetBeforeDraw(void);

	// デバッグ用描画
	void DrawDebug(void);

	// 解放
	void Release(void)override;

	// 座標の取得
	const VECTOR& GetPos(void) const;

	// 角度の取得
	const VECTOR& GetAngles(void) const;
	const Quaternion& GetQuaRot(void) const;
	
	// X回転を抜いたY軸のみのカメラ角度
	const Quaternion& GetQuaRotY(void) const;
	
	// 注視点の取得
	const VECTOR& GetTargetPos(void) const;

	// カメラの前方方向
	VECTOR GetForward(void) const;

	// カメラモードの変更
	void ChangeMode(MODE mode);

	//カメラモードの取得
	const MODE& GetCameraMode(void)const;

	// 追従対象の設定
	void SetFollow(const Transform* follow) { followTransform_ = follow; }

	//ロックオン対象の設定
	void SetTargetFollow(const VECTOR* target) { targetTransform_ = target; }

	// 衝突対象となるコライダを登録
    void AddHitCollider(int shape, const std::vector<ColliderBase*> hitCollider);

	// マウス入力の有効無効設定
	void SetIsMouseInput(bool isMouseInput) { isMouseInput_ = isMouseInput; }

	// マウス入力の有効無効取得
	bool GetIsMouseInput(void) { return isMouseInput_; }
protected:
	// リソースロード
	void InitLoad(void) override{}

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override{}

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override{}

	// 初期化後の個別処理
	void InitPost(void) override;
private:
	// カメラの回転スピード
	static constexpr float ROT_SPEED = 0.1f;
	// ロックオンカメラの高さ
	static constexpr float PLAYER_HEIGHT = 180.0f;
	// ロックオンカメラのプレイヤーとの距離
	static constexpr float PLAYER_MARGIN = 40.0f;
	// ロックオンカメラの視野角
	static constexpr float FOV_Y = 60.0f;

	// カメラが追従対象とするTransform
	const Transform* followTransform_;

	//ロックオン対象のTransform
	const VECTOR* targetTransform_;

	// ロックオン時のボスズーム
	static constexpr float LOCKON_BOSS_ZOOM = 100.0f;
	// ロックオン時のボスズームの最大値
	static constexpr float LOCKON_BOSS_ZOOM_MAX = 500.0f;
	// ロックオン時の高さ補正
	static constexpr float LOCKON_BOSS_HEIGHT_ADJUST = 30.0f;
	// ロックオン時の高さ調整値
	static constexpr float LOCKON_BOSS_HEIGHT_LERP_RATE = 0.2f;
	// ロックオン時のボスズームの補間率
	static constexpr float LOCKON_BOSS_ZOOM_LERP_RATE = 0.5f;

	//注視点の補間率
	static constexpr float LOCKON_TARGET_LERP_RATE = 0.4f;

	// ターゲットの中心位置
	static constexpr float TARGET_CENTER_POS = 30.0f;

	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION_CAMERA = 10;
	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 2.0f;
	// 衝突判定用球体半径
	static constexpr float COL_CAPSULE_SPHERE = 50.0f;

	//カメラ速度
	static constexpr float MOVE_SPEED = 0.5f;

	// カメラ補間フラグ
	bool isCameraLope_;

	// カメラモード
	MODE mode_;

	// カメラ角度(rad)
	VECTOR angles_;

	// カメラ角度(Y軸のみ)
	Quaternion rotY_;

	float angleY;

	// 注視点
	VECTOR targetPos_;
	
	// カメラを初期位置に戻す
	void SetDefault(void);

	// 追従対象との位置同期を取る
	void SyncFollow(void);

	// ターゲットとの位置同期を取る
	void SynLockOn(void);

	// カメラ操作
	void ProcessRot(bool isLimit);
	void ProcessMove(void);

	// カメラ回転(キーボード)
	void RotKeyboard(bool isLimit);

	// カメラ回転(ゲームパッド)
	void RotGamePad(bool isLimit);

	// モード別更新ステップ
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFree(void);
	void SetBeforeDrawFollow(void);
	void SetBeforeDrawTargetLockeOn(void);

	// 衝突判定
	void Collision(void);

	// カメラの補間移動率
	static constexpr float LERP_RATE_MOVE = 0.1f;

	// カメラの更新前位置
	VECTOR prePos_;

	// マウス入力の有効無効
	float mouseX;
	float mouseY;

	// マウス入力の前回位置
	float preMouseX;
	float preMouseY;

	// マウス入力の有効無効フラグ
	bool isMouseInput_;
};
