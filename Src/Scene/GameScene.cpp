#include <DxLib.h>
#include "../Manager/Game/EnemyManager.h"
#include "../Object/Actor/Character/Enemy/EnemyDragon.h"
#include "../Object/Actor/Character/Player.h"
#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/SkyDome.h"
#include "../Common/ShadowMap.h"
#include "../Object/Camera.h"
#include "../Manager/Common/SceneManager.h"
#include "../Manager/Common/InputManager.h"
#include "../Manager/Common/ResourceManager.h"
#include "../Manager/Common/SoundManager.h"
#include "../Manager/Common/EffectManager.h"
#include "../Manager/Game/CollisionManager.h"
#include "../Manager/Game/UiManager.h"
#include "../UI/BarUI.h"
#include "../UI/ImgUI.h"
#include "../UI/UIRecovery.h"
#include "../Application.h"
#include "GameScene.h"

GameScene::GameScene(void)
	: SceneBase()
	, gameClearImg_(-1)
	, gameOverImg_(-1)
	, collisionManager_(nullptr)
	, uiManager_(nullptr)
{
}

void GameScene::Load(void)
{
	// 衝突管理・UI管理生成
	collisionManager_ = std::make_unique<CollisionManager>();
	uiManager_ = std::make_unique<UiManager>();
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
	shadowMap_ = std::make_unique<ShadowMap>(SHADOW_MAP_RESOLUTION);

	// カメラモード変更
	camera_->SetFollow(player_);
	camera_->ChangeMode(Camera::MODE::FOLLOW);

	// 入力管理のインスタンス取得
	InputManager::GetInstance().SetMouseFlag(false);

	// ゲームクリア画像ロード
	gameClearImg_ = resMng_.Load(ResourceManager::SRC::IMG_GAMECLEAR).handleId_;
	// ゲームオーバー画像ロード
	gameOverImg_ = resMng_.Load(ResourceManager::SRC::IMG_GAMEOVER).handleId_;

	// ゲーム内UIをUiManagerへ一括登録
	RegisterGameUIs();
}

void GameScene::LoadEnd(void)
{
	// BGM再生
	SoundManager::GetInstance().PlaySoundId(SoundManager::SOUND_ID::BGM_GAME, true);

	// スカイドーム初期化
	skydome_->LoadEnd();
	// ステージ初期化
	stage_->LoadEnd();
	// プレイヤー初期化
	player_->LoadEnd();
	// エネミー初期化
	enemys_->Init();

	// コライダー登録
	collisionManager_->Release();
	RegisterActorColliders(*camera_);
	RegisterActorColliders(*stage_);
	RegisterCharacterColliders(*player_);
	for (const auto& enemy : enemys_->GetEnemies())
	{
		if (enemy)
		{
			RegisterCharacterColliders(*enemy);
		}
	}

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
	// 当たり判定更新
	collisionManager_->Update();

	// シーン変更
	UpdateResult();

	// 再生中のエフェクトを更新する
	EffectManager::GetInstance().Update();

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

	// UI一括更新
	if (uiManager_)
	{
		uiManager_->Update();
	}
	
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

	// 再生中のエフェクトを描画する
	EffectManager::GetInstance().Draw();

	// 当たり判定デバッグ描画
	collisionManager_->DrawDebug();

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

	// 衝突管理解放
	if (collisionManager_)
	{
		collisionManager_->Release();
	}

	// UI管理解放
	if (uiManager_)
	{
		uiManager_->Release();
	}
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

	// 敵死亡時のゲームクリアUI表示更新
	for (const auto& enemy : enemys_->GetEnemies())
	{
		if (enemy->GetState() == static_cast<int>(EnemyDragon::STATE::DEAD) && uiManager_)
		{
			int alpha = static_cast<int>(enemy->GetDeathAnimTime() * RESULT_UI_ALPHA_MAGNIFICATION);
			uiManager_->SetActive(UI_ID_RESULT, true);
			uiManager_->SetImg(UI_ID_RESULT, gameClearImg_);
			uiManager_->SetAlpha(UI_ID_RESULT, alpha);
		}
	}

	// プレイヤー死亡時のゲームオーバーUI表示更新
	if (player_->GetState() == Player::STATE::DIE && uiManager_)
	{
		int alpha = static_cast<int>(player_->GetDeathAnimTime() * RESULT_UI_ALPHA_MAGNIFICATION);
		uiManager_->SetActive(UI_ID_RESULT, true);
		uiManager_->SetImg(UI_ID_RESULT, gameOverImg_);
		uiManager_->SetAlpha(UI_ID_RESULT, alpha);
	}

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
	// 登録された全UIを一括描画する
	if (uiManager_)
	{
		uiManager_->Draw();
	}
}

void GameScene::RegisterGameUIs(void)
{
	if (!uiManager_)
	{
		return;
	}

	// ゲージ用画像ハンドルを取得
	int frameImg = resMng_.Load(ResourceManager::SRC::UI_BAR_FRAME).handleId_;
	int hpImg = resMng_.Load(ResourceManager::SRC::UI_HP_BAR).handleId_;
	int stImg = resMng_.Load(ResourceManager::SRC::UI_ST_BAR).handleId_;

	// プレイヤーのHP・スタミナ・回復瓶UIをモデルにバインドして登録
	if (player_)
	{
		std::weak_ptr<Player> wpPlayer = player_;

		auto playerHp = std::make_unique<BarUI>(
			hpImg,
			frameImg,
			PLAYER_UI_HP_POS,
			PLAYER_UI_BAR_SCALE,
			PLAYER_UI_FRAME_SCALE);
		playerHp->BindRate([wpPlayer](void)
			{
				if (auto p = wpPlayer.lock()) { return p->GetHpRate(); }
				return 0.0f;
			});
		uiManager_->Add(std::move(playerHp));

		auto playerSt = std::make_unique<BarUI>(
			stImg,
			frameImg,
			PLAYER_UI_ST_POS,
			PLAYER_UI_BAR_SCALE,
			PLAYER_UI_FRAME_SCALE);
		playerSt->BindRate([wpPlayer](void)
			{
				if (auto p = wpPlayer.lock()) { return p->GetStRate(); }
				return 0.0f;
			});
		uiManager_->Add(std::move(playerSt));

		auto playerRecovery = std::make_unique<UIRecovery>(player_->GetRecoveryBottleCnt());
		playerRecovery->BindBottleCnt([wpPlayer](void)
			{
				if (auto p = wpPlayer.lock()) { return p->GetRecoveryBottleCnt(); }
				return 0;
			});
		uiManager_->Add(std::move(playerRecovery));
	}

	// 各エネミーのHP UIをモデルにバインドして登録
	if (enemys_)
	{
		for (const auto& enemy : enemys_->GetEnemies())
		{
			if (!enemy)
			{
				continue;
			}
			std::weak_ptr<EnemyBase> wpEnemy = enemy;
			auto enemyHp = std::make_unique<BarUI>(
				hpImg,
				frameImg,
				ENEMY_UI_HP_POS,
				ENEMY_UI_BAR_SCALE,
				ENEMY_UI_FRAME_SCALE);
			enemyHp->BindRate([wpEnemy](void)
				{
					if (auto e = wpEnemy.lock()) { return e->GetHpRate(); }
					return 0.0f;
				});
			uiManager_->Add(std::move(enemyHp));
		}
	}

	// リザルト用画像UIを初期非表示で登録
	auto resultUi = std::make_unique<ImgUI>(
		-1,
		RESULT_UI_POS,
		RESULT_UI_SIZE);
	resultUi->SetActive(false);
	uiManager_->Add(UI_ID_RESULT, std::move(resultUi));
}

void GameScene::RegisterActorColliders(const ActorBase& actor)
{
	// アクターが所有する全形状のコライダーを衝突管理へ登録
	for (const auto& [shape, colliders] : actor.GetOwnColliders())
	{
		for (const auto& collider : colliders)
		{
			if (collider)
			{
				collisionManager_->AddCollider(collider);
			}
		}
	}
}

void GameScene::RegisterCharacterColliders(const CharacterBase& character)
{
	// キャラクター本体のコライダーを登録
	RegisterActorColliders(character);
	// 所有している全武器のコライダーを登録
	for (const auto& weapon : character.GetWeapons())
	{
		if (weapon)
		{
			RegisterActorColliders(*weapon);
		}
	}
}
