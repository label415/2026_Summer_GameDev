#pragma once
#include "SceneBase.h"

class SkyDome;
class Stage;
class Player;
class Camera;
class EnemyManager;

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

	//スカイドーム
	SkyDome* skydome_;

	//ステージ
	Stage* stage_;

	//プレイヤー
	Player* player_;

	//エネミー
	EnemyManager* enemys_;

	//カメラ
	Camera* camera_;

	int ShadowMapHandle;

	//コライダー登録
	void RegistCollider(void);

};
