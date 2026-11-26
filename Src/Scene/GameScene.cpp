#include <DxLib.h>
#include "../Object/Actor/Character/Enemy/EnemyManager.h"
#include "../Object/Actor/Character/Enemy/EnemyDragon.h"
#include "../Object/Actor/Character/Player.h"
#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/SkyDome.h"
#include "../Common/ShadowMap.h"
#include "../Manager/Camera.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/FontManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/CollisionManager.h"
#include "../Application.h"
#include "GameScene.h"

GameScene::GameScene(void)
	: SceneBase()
	, isResultUI_(false)
	, resultAlpha_(-1)
	, resultImg_(-1)
	, gameClearImg_(-1)
	, gameOverImg_(-1)
{
}

GameScene::~GameScene(void){}

void GameScene::Load(void)
{
	// カメラのインスタンス取得
	camera_ = SceneManager::GetInstance().GetCamera();
	camera_->ChangeMode(Camera::MODE::FIXED_POINT);

	// スカイドーム読み込み
	skydome_ = std::make_unique<SkyDome>();
	skydome_->Load();

	// ステージ初期化
	stage_ = std::make_unique<Stage>();
	stage_->Load();

	// プレイヤー読み込み
	player_ = std::make_shared<Player>();
	player_->Load();

	// エネミー読み込み
	enemys_ = std::make_shared<EnemyManager>();
	enemys_->Load();

	// シャドーマップ読み込み
	shadowMap_ = std::make_unique<ShadowMap>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

	// カメラモード変更
	camera_->SetFollow(player_);
	camera_->ChangeMode(Camera::MODE::FOLLOW);

	// 入力管理のインスタンス取得
	InputManager::GetInstance().SetMouseFlag(false);

	// ゲームクリア画像ロード
	gameClearImg_ = resMng_.Load(ResourceManager::SRC::IMG_GAMECLEAR).handleId_;
	// ゲームオーバー画像ロード
	gameOverImg_ = resMng_.Load(ResourceManager::SRC::IMG_GAMEOVER).handleId_;
}

void GameScene::LoadEnd(void)
{
	// BGM再生
	bgm_ = resMng_.Load(ResourceManager::SRC::BGM_GAME).handleId_;
	volume_ = BGM_VOLUME;
	SoundManager::GetInstance().PlayBGM(bgm_, volume_);

	// スカイドーム初期化
	skydome_->LoadEnd();
	// ステージ初期化
	stage_->LoadEnd();
	// プレイヤー初期化
	player_->LoadEnd();
	// エネミー初期化
	enemys_->Init();

	// シャドーマップ初期化
	shadowMap_->AddShadowMapLight(GetLightDirection());
	VECTOR playerPos = player_->GetTransform().pos;
	shadowMap_->AddShadowMapDrawArea(
		VGet((playerPos.x - SHADOW_MAP_DIFF),
			SHADOW_MAP_MIN_DRAW,
			(playerPos.z - SHADOW_MAP_DIFF)),
		VGet(
			(playerPos.x + SHADOW_MAP_DIFF),
			SHADOW_MAP_DIFF * SHADOW_MAP_MAX_DRAW,
			(playerPos.z + SHADOW_MAP_DIFF))
	);

	// マウス入力有効化
	InputManager::GetInstance().SetMouseFlag(false);
}

void GameScene::Update(void)
{
	// シーン変更
	UpdateResult();

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();

	// 追尾対象更新
	UpdateAutoLockOn();

	// スカイドーム更新
	skydome_->Update();

	//ステージ更新
	stage_->Update();

	// エネミー更新
	enemys_->Update();

	// プレイヤー更新
	player_->Update();
	
	// シャドーマップ更新
	shadowMap_->AddShadowMapLight(GetLightDirection());
	VECTOR playerPos = player_->GetTransform().pos;
	shadowMap_->AddShadowMapDrawArea(
		VGet((playerPos.x - SHADOW_MAP_DIFF),
			SHADOW_MAP_MIN_DRAW,
			(playerPos.z - SHADOW_MAP_DIFF)),
		VGet((playerPos.x + SHADOW_MAP_DIFF),
			SHADOW_MAP_DIFF * SHADOW_MAP_MAX_DRAW,
			(playerPos.z + SHADOW_MAP_DIFF))
	);
}

void GameScene::Draw(void)
{
	// シャドウ描画のセットアップ
	shadowMap_->DrawSetup();
	ObjectDraw();
	// シャドウ描画の終了
	shadowMap_->DrawEnd();

	// シャドウマップを使用して描画する
	shadowMap_->SetShadow();
	ObjectDraw();
	// シャドウマップの使用を終了する
	shadowMap_->EndShadow();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();

	// 各UI描画
	DrawUI();
}

void GameScene::ObjectDraw(void)
{
	// スカイドーム描画
	skydome_->Draw();

	// プレイヤー描画
	player_->Draw();

	// エネミー描画
	enemys_->Draw();

	// ステージ描画
	stage_->Draw();
}

void GameScene::Release(void)
{
	// スカイドーム解放
	skydome_->Release();

	// ステージ解放
	stage_->Release();

	//プレイヤー解放
	player_->Release();

	//各エネミー解放
	enemys_->Release();

	//シャドウマップ解放
	shadowMap_->Release();
}

void GameScene::UpdateAutoLockOn(void)
{
	// カメラのインスタンス取得
	std::shared_ptr<Camera> camera = SceneManager::GetInstance().GetCamera();

	// カメラ側でロックオン対象の選別・切り替えを更新
	camera->UpdateLockOn();

	// カメラが現在ロックオンしている対象のTransformをプレイヤーに渡す
	player_->SetTargetPos(camera->GetLockOnTargetPos());

	// 追尾対象追加
	for (auto& enemy : enemys_->GetEnemies())
	{
		// 敵にプレイヤーの位置を渡す
		enemy->SetTargetPos(&player_->GetTransform().pos);
		for (const auto& collider
			: enemy->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE)))
		{
			// カプセルコライダーをカメラに渡す
			auto enemyCapsule = std::dynamic_pointer_cast<ColliderCapsule>(collider);
			if (enemyCapsule)
			{
				camera_->AddTargetCollider(enemyCapsule);
			}
		}
	}
}

void GameScene::UpdateResult(void)
{
	bool isResult = false;

	// ポーズメニューの表示切り替え
	bool isSelect = InputManager::GetInstance().IsTrgDown(KEY_INPUT_ESCAPE)
		|| InputManager::GetInstance().IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START);

	// プレイヤー死亡判定
	if (player_->GetState() == Player::STATE::END)isResult = true;

	// 敵全滅判定
	for (const auto& enemy : enemys_->GetEnemies())
	{
		if (enemy->GetState() == static_cast<int>(EnemyDragon::STATE::END))isResult = true;
	}

	// タイトルシーンに遷移
	if (isResult) { sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE); }
	// ポーズメニューの表示
	if (isSelect) { sceMng_.PushScene(SceneManager::SCENE_ID::PAUSE); }
}

void GameScene::DrawUI(void)
{
	// プレイヤーとエネミーのHP描画
	player_->DrawUI();
	for (const auto& enemy : enemys_->GetEnemies()) {
		enemy->DrawUI();
	}

	// すべての敵が死亡した時、ゲームクリアUIを表示する
	for (const auto& enemy : enemys_->GetEnemies())
	{
		if (enemy->GetState() == static_cast<int>(EnemyDragon::STATE::DEAD))
		{
			resultAlpha_ = static_cast<int>(
				enemy->GetDeathAnimTime() * RESULT_UI_ALPHA_MAGNIFICATION);
			resultImg_ = gameClearImg_;
			isResultUI_ = true;
		}
	}

	// プレイヤーが死亡した時、ゲームオーバーUIを表示する
	if (player_->GetState() == Player::STATE::DIE)
	{
		resultAlpha_ = static_cast<int>(
			player_->GetDeathAnimTime() * RESULT_UI_ALPHA_MAGNIFICATION);
		resultImg_ = gameOverImg_;
		isResultUI_ = true;
	}

	// リザルトフラグがtrueの時、UIを表示する
	if (isResultUI_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, resultAlpha_);
		DrawRotaGraph(
			Application::HALF_SCREEN_SIZE_X,
			Application::HALF_SCREEN_SIZE_Y,
			RESULT_UI_SIZE,
			0.0f,
			resultImg_,
			true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}
