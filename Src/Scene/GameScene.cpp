#include <DxLib.h>
#include "../Manager/Camera.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/FontManager.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/SkyDome.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/Enemy/EnemyManager.h"
#include "../Object/Common/Collider/ColliderBase.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	SceneBase()
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	Camera* camera = SceneManager::GetInstance().GetCamera();

	skydome_ = new SkyDome();
	skydome_->Init();

	//ステージ初期化
	stage_ = new Stage();
	stage_->Init();

	player_ = new Player();
	camera->SetFollow(&player_->GetTransform());
	player_->Init();

	enemys_ = new EnemyManager();
	enemys_->Init();

	camera->ChangeMode(Camera::MODE::FOLLOW);

	// ステージモデルのコライダーをプレイヤーに登録
	const ColliderBase* stageCollider =
		stage_->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::MODEL));
	const ColliderBase* PlayerCollider =
		player_->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	const ColliderBase* cameraCollider =
		camera->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::SPHERE));

	player_->AddHitCollider(stageCollider);
	enemys_->AddHitCollider(stageCollider);
	enemys_->AddHitCollider(PlayerCollider);
	camera->AddHitCollider(stageCollider);
	stage_->AddHitCollider(cameraCollider);

	ShadowMapHandle = MakeShadowMap(1024, 1024);

	// シャドウマップが想定するライトの方向もセット
	SetShadowMapLightDirection(ShadowMapHandle, VGet(0.3f, -0.7f, 0.8f));

	// シャドウマップに描画する範囲を設定
	SetShadowMapDrawArea(ShadowMapHandle, VGet(-1000.0f, -200.0f, -1000.0f), VGet(1000.0f, 1000.0f, 1000.0f));
}

void GameScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	skydome_->Update();

	//ステージ更新
	stage_->Update();

	player_->Update();

	enemys_->Update();
}

void GameScene::Draw(void)
{
	skydome_->Draw();

	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup(ShadowMapHandle);
	//ステージ描画
	player_->Draw();
	enemys_->Draw();
	stage_->Draw();
	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();

	// 描画に使用するシャドウマップを設定
	SetUseShadowMap(0, ShadowMapHandle);
	//ステージ描画
	player_->Draw();
	enemys_->Draw();
	stage_->Draw();
	// 描画に使用するシャドウマップの設定を解除
	SetUseShadowMap(0, -1);
}

void GameScene::Release(void)
{
	skydome_->Release();
	delete skydome_;

	//ステージ解放
	stage_->Release();
	delete stage_;

	player_->Release();
	delete player_;

	enemys_->Release();
	delete enemys_;

	// シャドウマップの削除
	DeleteShadowMap(ShadowMapHandle);
}
