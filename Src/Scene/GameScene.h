#pragma once
#include "SceneBase.h"

class SkyDome;
class Stage;
class Player;
class Camera;
class EnemyManager;
class EnemyBase;
class ColliderCapsule;
class ShadowMap;
class PauseScene;

class GameScene : public SceneBase
{
public:
	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	// 読み込み
	void Load(void)override;

	// 読み込み後の初期化
	void LoadEnd(void)override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;
private:
	// BGMの音量
	static constexpr int BGM_VOLUME = 40;

	// シャドウマップ解像度
	static constexpr int SHADOW_MAP_RESOLUTION = 2048;
	// シャドウマップ範囲
	static constexpr float SHADOW_MAP_DIFF = 1000.0f;
	// シャドウマップ最大描画倍率
	static constexpr float SHADOW_MAP_MAX_DRAW = 1.5f;
	// シャドウマップ最低描画倍率
	static constexpr float SHADOW_MAP_MIN_DRAW = -1.0f;

	// ロックオンUIのX座標
	static constexpr float LOCON_UI_C_X = 0.5f;
	//ロックオンUIの大きさ
	static constexpr float LOCON_UI_SIZE = 50.0f;

	// ロックオン対象範囲
	static constexpr float MAX_LOCKON_DIFF = 2000.0f;
	// ロックオン角度
	static constexpr float LOCKON_VIEW_ANGLE = 80.0f;

	// リザルト画像の大きさ
	static constexpr float RESULT_UI_SIZE = 1.0f;
	// リザルトブレンドパラメータ倍率
	static constexpr float RESULT_UI_ALPHA_MAGNIFICATION = 255.0f;

	// リザルトUI表示フラグ
	bool isResultUI_;
	// リザルトハンドル
	int resultImg_;
	// リザルトブレンドパラメータ
	int resultAlpha_;
	// ゲームクリアハンドル
	int gameClearImg_;
	// ゲームオーバーハンドル
	int gameOverImg_;
	// ロックオンUIハンドル
	int lockOnImg_;

	// ポーズシーン
	PauseScene* pauseScene_;

	// スカイドーム
	SkyDome* skydome_;

	// ステージ
	Stage* stage_;

	// プレイヤー
	Player* player_;

	// エネミー
	EnemyManager* enemys_;

	// ロックオン対象のエネミー
	ColliderCapsule* targetEnemy_;

	// カメラ
	Camera* camera_;

	// シャドウマップ
	ShadowMap* shadowMap_;

	// コライダー登録
	void AddCollider(void);
	// コライダー更新
	void UpdateCollider(void);

	// 自動ロックオン対象選別
	void UpdateAutoLockOn(void);
};
