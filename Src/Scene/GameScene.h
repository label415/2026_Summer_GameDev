#pragma once
#include <memory>
#include "SceneBase.h"

class SkyDome;
class Stage;
class Player;
class Camera;
class EnemyManager;
class EnemyBase;
class ColliderCapsule;
class ShadowMap;

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

	// オブジェクト描画
	void ObjectDraw(void);

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

	// スカイドーム
	std::unique_ptr<SkyDome> skydome_;

	// ステージ
	std::unique_ptr<Stage> stage_;

	// プレイヤー
	std::shared_ptr<Player> player_;

	// エネミー
	std::shared_ptr<EnemyManager> enemys_;

	// カメラ
	std::shared_ptr<Camera> camera_;

	// シャドウマップ
	std::unique_ptr<ShadowMap> shadowMap_;

	// 追尾対象更新
	void UpdateAutoLockOn(void);
	// シーン変更
	void UpdateResult(void);

	//UI描画
	void DrawUI(void);
};
