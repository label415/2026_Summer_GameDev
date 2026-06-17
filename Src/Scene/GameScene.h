#pragma once
#include "SceneBase.h"

class SkyDome;
class Stage;
class Player;
class Camera;
class EnemyManager;
class EnemyBase;
class ColliderCapsule;

class GameScene : public SceneBase
{

public:
	
	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	//ロックオン対象範囲
	static constexpr float MAX_LOCKON_DIFF = 1500.0f;

	//ロックオン角度
	static constexpr float VIEW_ANGLE = 80.0f;

	//スカイドーム
	SkyDome* skydome_;

	//ステージ
	Stage* stage_;

	//プレイヤー
	Player* player_;

	//エネミー
	EnemyManager* enemys_;

	//ロックオン対象のエネミー
	ColliderCapsule* targetEnemy_;

	//カメラ
	Camera* camera_;

	int ShadowMapHandle;

	//コライダー登録
	void AddCollider(void);
	void UpdateCollider(void);

	//自動ロックオン対象選別
	void UpdateAutoLockOn(void);

};
