#pragma once
#include "../Application.h"
#include "../Template/Vector2Template.h"
#include "SceneBase.h"

class SkyDome;
class Stage;
class Player;
class Camera;
class EnemyManager;
class EnemyBase;
class ColliderCapsule;
class ShadowMap;
class CollisionManager;
class UiManager;
class ActorBase;
class CharacterBase;

class GameScene : public SceneBase
{
public:

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override = default;

	// 読み込み
	void Load(void)override;

	// 読み込み後の初期化
	void LoadEnd(void)override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// オブジェクト描画
	void ObjectDraw(void);

	// 解放
	void Release(void) override;

private:

	// シャドウマップ範囲
	static constexpr float SHADOW_MAP_DIFF = 1000.0f;

	// シャドウマップ最大描画倍率
	static constexpr float SHADOW_MAP_MAX_DRAW = 1.5f;

	// シャドウマップ最低描画倍率
	static constexpr float SHADOW_MAP_MIN_DRAW = -1.0f;

	// リザルト画像の配置と大きさ
	static constexpr Vector2F RESULT_UI_POS = Application::HALF_SCREEN_SIZE;
	static constexpr Vector2F RESULT_UI_SIZE = { 1.0f, 1.0f };

	// リザルトブレンドパラメータ倍率
	static constexpr float RESULT_UI_ALPHA_MAGNIFICATION = 255.0f;

	// シャドウマップ解像度
	static constexpr Vector2 SHADOW_MAP_RESOLUTION = { 2048, 2048 };

	// UI識別ID
	static constexpr int UI_ID_RESULT = 0;

	// プレイヤー用UI配置・スケール定数
	static constexpr float PLAYER_UI_HP_POS_X_DIV = 4.0f;
	static constexpr Vector2F PLAYER_UI_HP_POS = { Application::SCREEN_SIZE.x / PLAYER_UI_HP_POS_X_DIV, 20.0f };
	static constexpr Vector2F PLAYER_UI_ST_POS = { Application::SCREEN_SIZE.x / PLAYER_UI_HP_POS_X_DIV, 60.0f };
	static constexpr float PLAYER_UI_FRAME_SCALE = 0.7f;
	static constexpr Vector2F PLAYER_UI_BAR_SCALE = { 2.9f, 4.0f };

	// 敵用UI配置・スケール定数
	static constexpr float ENEMY_UI_HP_OFFSET_Y = 75.0f;
	static constexpr Vector2F HALF_SCREEN_SIZE_F = Application::HALF_SCREEN_SIZE;
	static constexpr Vector2F ENEMY_UI_HP_POS = { HALF_SCREEN_SIZE_F.x, Application::SCREEN_SIZE.y - ENEMY_UI_HP_OFFSET_Y };
	static constexpr float ENEMY_UI_FRAME_SCALE = 1.3f;
	static constexpr Vector2F ENEMY_UI_BAR_SCALE = { 1.57f, 2.0f };

	// ゲームクリアハンドル
	int gameClearImg_;

	// ゲームオーバーハンドル
	int gameOverImg_;

	// スカイドーム
	std::unique_ptr<SkyDome> skydome_;

	// シャドウマップ
	std::unique_ptr<ShadowMap> shadowMap_;

	// ステージ
	std::unique_ptr<Stage> stage_;

	// プレイヤー
	std::shared_ptr<Player> player_;

	// エネミー
	std::shared_ptr<EnemyManager> enemys_;

	// カメラ
	std::shared_ptr<Camera> camera_;

	// 衝突管理
	std::unique_ptr<CollisionManager> collisionManager_;

	// UI管理
	std::unique_ptr<UiManager> uiManager_;

	// UI一括登録
	void RegisterGameUIs(void);

	// 追尾対象更新
	void UpdateAutoLockOn(void);

	// シーン変更
	void UpdateResult(void);

	//UI描画
	void DrawUI(void);

	// コライダー登録
	void RegisterActorColliders(const ActorBase& actor);
	void RegisterCharacterColliders(const CharacterBase& character);

};
