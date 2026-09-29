#include <DxLib.h>
#include "../Manager/Camera.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/FontManager.h"
#include "../Object/Actor/Wepon/WeponBase.h"
#include "../Object/Actor/Wepon/WeponBracelet.h"
#include "../Object/Actor/Wepon/WeponFlameThrower.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/SkyDome.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/Enemy/EnemyManager.h"
#include "../Object/Actor/Charactor/Enemy/EnemyDragon.h"
#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Application.h"
#include "../Utility/MatrixUtility.h"
#include "../Object/Actor/UI/UIHp.h"
#include "../Common/ShadowMap.h"
#include "PauseScene.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	SceneBase(),
	isResultUI_(false),
	resultAlpha_(0.0f),
	resultImg_(-1),
	gameClearImg_(-1),
	gameOverImg_(-1),
	lockOnImg_(-1)
{
}

GameScene::~GameScene(void) {}

void GameScene::Load(void)
{
	// カメラのインスタンス取得
	camera_ = SceneManager::GetInstance().GetCamera();
	camera_->ChangeMode(Camera::MODE::FIXED_POINT);

	// スカイドーム読み込み
	skydome_ = new SkyDome();
	skydome_->Load();

	// ステージ初期化
	stage_ = new Stage();
	stage_->Load();

	// プレイヤー読み込み
	player_ = new Player();
	player_->Load();

	// エネミー読み込み
	enemys_ = new EnemyManager();
	enemys_->Load();
	targetEnemy_ = nullptr;

	// シャドーマップ読み込み
	shadowMap_ = new ShadowMap(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
	pauseScene_ = new PauseScene();

	// カメラモード変更
	camera_->SetFollow(&player_->GetTransform());
	camera_->ChangeMode(Camera::MODE::FOLLOW);

	// 入力管理のインスタンス取得
	InputManager::GetInstance().SetMouseFlage(false);

	// ゲームクリア画像ロード
	gameClearImg_ = resMng_.Load(ResourceManager::SRC::IMG_GAMECLEAR).handleId_;
	// ゲームオーバー画像ロード
	gameOverImg_ = resMng_.Load(ResourceManager::SRC::IMG_GAMEOVER).handleId_;
	// ロックオン画像ロード
	lockOnImg_ = resMng_.Load(ResourceManager::SRC::UI_LOCKON).handleId_;
}

void GameScene::LoadEnd(void)
{
	// BGM再生
	bgm_ = resMng_.Load(ResourceManager::SRC::BGM_GAME).handleId_;
	volume_ = BGM_VOLUME;
	SoundManager::GetInstance().PlayBGM(bgm_, volume_);

	// スカイドーム初期化
	skydome_->Init();
	// ステージ初期化
	stage_->Init();
	// プレイヤー初期化
	player_->Init();
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

	// ポーズメニュー読み取り後の処理
	pauseScene_->LoadEnd();

	// コライダ登録
	AddCollider();
}

void GameScene::Update(void)
{
	// プレイヤーが死亡していた時,タイトルシーンに遷移
	if (player_->GetState() == Player::STATE::END) {
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
		return;
	}

	// エネミーが死亡していた時,タイトルシーンに遷移
	for (const auto& enemy : enemys_->GetEnemys())
	{
		if (enemy->GetState() == static_cast<int>(EnemyDragon::STATE::END)) {
			sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
			return;
		}
	}

	// ポーズメニューの表示切り替え
	bool isSelect = InputManager::GetInstance().IsTrgDown(KEY_INPUT_ESCAPE)
		|| InputManager::GetInstance().IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START);
	if (isSelect) {
		bool currentStatus = pauseScene_->GetIsAlive();
		pauseScene_->SetIsAlive(!currentStatus);
	}

	// ポーズメニューが表示されている時は,ポーズメニューの更新を行う
	if (pauseScene_->GetIsAlive()) {
		pauseScene_->Update();
		camera_->SetIsMouseInput(false);
		InputManager::GetInstance().SetMouseFlage(true);
		SoundManager::GetInstance().AllStopSE();
		return;
	}
	else {
		camera_->SetIsMouseInput(true);
		InputManager::GetInstance().SetMouseFlage(false);
	}

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();

	// スカイドーム更新
	skydome_->Update();
	//ステージ更新
	stage_->Update();

	// 自動ロックオン対象選別
	UpdateAutoLockOn();

	// エネミー更新
	enemys_->Update();
	// プレイヤー更新
	player_->Update();

	// コライダー更新
	UpdateCollider();

	// プレイヤーのダメージ判定
	for (const auto& enemy : enemys_->GetEnemys()) {
		player_->HitDamage(enemy->GetIsAttack());
	}
	// エネミーのダメージ判定
	enemys_->HitDamegr(player_->GetIsAttack());

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
	// スカイドーム描画
	skydome_->Draw();
	// シャドウ描画のセットアップ
	shadowMap_->DrawSetup();
	// プレイヤー描画
	player_->Draw();
	// エネミー描画
	enemys_->Draw();
	// ステージ描画
	stage_->Draw();
	// シャドウ描画の終了
	shadowMap_->DrawEnd();

	// シャドウマップを使用して描画する
	shadowMap_->SetShadow();
	// プレイヤー描画
	player_->Draw();
	// エネミー描画
	enemys_->Draw();
	// ステージ描画
	stage_->Draw();
	// シャドウマップの使用を終了する
	shadowMap_->EndShadow();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();

	// プレイヤーとエネミーのHP描画
	player_->DrawHp();
	for (const auto& enemy : enemys_->GetEnemys()) {
		enemy->DrawHp();
	}

	// ロックオンUI描画
	if (targetEnemy_ != nullptr) {
		VECTOR enemyTransform = targetEnemy_->GetCenter();
		SetUseZBuffer3D(FALSE);
		DrawBillboard3D(
			enemyTransform,
			0.5f, 0.5f,
			LOCON_UI_SIZE, 0.0f,
			lockOnImg_, true);
		SetUseZBuffer3D(TRUE);
	}

	// ポーズメニューが表示されている時は,ポーズメニューの描画を行う
	if (pauseScene_->GetIsAlive()) {
		pauseScene_->Draw();
	}

	// すべての敵が死亡した時、ゲームクリアUIを表示する
	for (const auto& enemy : enemys_->GetEnemys())
	{
		if (enemy->GetState() == static_cast<int>(EnemyDragon::STATE::DEAD))
		{
			resultAlpha_ = static_cast<int>(
				enemy->Geti() * RESULT_UI_ALPHA_MAGNIFICATION);
			resultImg_ = gameClearImg_;
			isResultUI_ = true;
		}
	}

	// プレイヤーが死亡した時、ゲームオーバーUIを表示する
	if (player_->GetState() == Player::STATE::DIE)
	{
		resultAlpha_ = static_cast<int>(
			player_->Geti() * RESULT_UI_ALPHA_MAGNIFICATION);
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

void GameScene::Release(void)
{
	// スカイドーム解放
	skydome_->Release();
	delete skydome_;

	// ステージ解放
	stage_->Release();
	delete stage_;

	//プレイヤー解放
	player_->Release();
	delete player_;

	//各エネミー解放
	enemys_->Release();
	delete enemys_;
	delete targetEnemy_;

	//シャドウマップ解放
	shadowMap_->Release();
	delete shadowMap_;
}

void GameScene::AddCollider(void)
{
	// 各クラスにステージコライダーを初期化時に登録
	const std::vector<ColliderBase*> stageCollider =
		stage_->GetOwnCollider(
			static_cast<int>(ColliderBase::SHAPE::MODEL));
	player_->AddHitCollider(
		static_cast<int>(ColliderBase::SHAPE::MODEL), stageCollider);
	enemys_->AddHitCollider(
		static_cast<int>(ColliderBase::SHAPE::MODEL), stageCollider);
	camera_->AddHitCollider(
		static_cast<int>(ColliderBase::SHAPE::MODEL), stageCollider);

	// 各クラスにプレイヤーコライダーを初期化時に登録
	const std::vector<ColliderBase*> pColliders =
		player_->GetOwnCollider(
			static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	enemys_->AddHitCollider(
		static_cast<int>(ColliderBase::SHAPE::CAPSULE), pColliders);

	// 各クラスにカメラコライダーを初期化時に登録
	const std::vector<ColliderBase*> cameraCollider =
		camera_->GetOwnCollider(
			static_cast<int>(ColliderBase::SHAPE::SPHERE));
	stage_->AddHitCollider(
		static_cast<int>(ColliderBase::SHAPE::SPHERE), cameraCollider);

	//各クラスにエネミーコライダーを初期化時に登録
	const auto& enemys = enemys_->GetEnemys();
	for (auto& enemy : enemys)
	{
		if (enemy == nullptr)continue;

		const std::vector<ColliderBase*> enemyColliders =
			enemy->GetOwnCollider(
				static_cast<int>(ColliderBase::SHAPE::CAPSULE));

		player_->AddHitCollider(
			static_cast<int>(ColliderBase::SHAPE::CAPSULE), enemyColliders);
	}
}

void GameScene::UpdateCollider(void)
{
	// 各クラスに武器コライダーを更新に登録
	const WeponBase* wepon = player_->GetWepon();
	const std::vector<ColliderBase*> weponColliders =
		wepon->GetOwnCollider(
			static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	enemys_->AddHitCollider(
		static_cast<int>(ColliderBase::SHAPE::CAPSULE), weponColliders);

	// プレイヤーと各エネミーにコライダーを更新
	const auto& enemys = enemys_->GetEnemys();
	for (auto& enemy : enemys)
	{
		if (enemy == nullptr) continue;

		const WeponBase* enemyWepon = enemy->GetWepon();
		if (enemyWepon != nullptr) {

			const std::vector<ColliderBase*> weponCollider1 =
				enemyWepon->GetOwnCollider(
					static_cast<int>(ColliderBase::SHAPE::CAPSULE));
			player_->AddHitCollider(
				static_cast<int>(ColliderBase::SHAPE::CAPSULE), weponCollider1);

			const std::vector<ColliderBase*> weponCollider2 =
				enemyWepon->GetOwnCollider(
					static_cast<int>(ColliderBase::SHAPE::SPHERE));
			player_->AddHitCollider(
				static_cast<int>(ColliderBase::SHAPE::SPHERE), weponCollider2);

		}
		else {
			player_->RemoveHitColliderByShapeAndTag(
				ColliderBase::SHAPE::CAPSULE, ColliderBase::TAG::ENEMY_WEPON);
			player_->RemoveHitColliderByShapeAndTag(
				ColliderBase::SHAPE::SPHERE, ColliderBase::TAG::ENEMY_WEPON);
		}
	}

	// 生存していない時は削除
	if (!wepon->GetIsAlive())
	{
		enemys_->RemoveCollider(
			ColliderBase::SHAPE::CAPSULE, ColliderBase::TAG::PLAYER_WEPON);
	}
}

void GameScene::UpdateAutoLockOn(void)
{
	Camera* camera = SceneManager::GetInstance().GetCamera();
	auto& enemys = enemys_->GetEnemys();
	auto& inp = InputManager::GetInstance();
	VECTOR playerPos = player_->GetTransform().pos;
	float diffMin = MAX_LOCKON_DIFF;
	bool isChanger = false;

	for (const auto& enemy : enemys) {
		enemy->SetTargetTransform(&player_->GetTransform().pos);
	}

	bool isLockOn = inp.IsTrgMouseMiddle()
		|| inp.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER);

	if (camera_->GetCameraMode() == Camera::MODE::TARGET_ROCKE) {

		VECTOR dir = AsoUtility::VECTOR_ZERO;
		InputManager::JOYPAD_IN_STATE padState =
			inp.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// アナログスティック方向
		dir = inp.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);

		static float prevStickX = 0.0f;
		static float prevStickZ = 0.0f;
		const float stickThreshold = InputManager::THRESHOLD;

		// ロックオン対象選択フラグ
		bool isNextUp = (dir.z > stickThreshold)
			&& (prevStickZ <= stickThreshold);
		bool isNextDown = (dir.z < -stickThreshold)
			&& (prevStickZ >= -stickThreshold);
		bool isNextRight = (dir.x > stickThreshold)
			&& (prevStickX <= stickThreshold)
			|| inp.GetMouseWheelRot() > 0.0f;
		bool isNextLeft = (dir.x < -stickThreshold)
			&& (prevStickX >= -stickThreshold)
			|| inp.GetMouseWheelRot() < 0.0f;

		prevStickX = dir.x;
		prevStickZ = dir.z;

		ColliderCapsule* lastTagerEnemy = targetEnemy_;

		VECTOR targetPos = targetEnemy_->GetCenter();
		float diff = VSize(VSub(targetPos, playerPos));

		if (diff >= MAX_LOCKON_DIFF || isLockOn) {
			camera_->ChangeMode(Camera::MODE::FOLLOW);
			targetEnemy_ = nullptr;
			isChanger = true;
			player_->SetTargetTransform(nullptr);
		}

		if (isNextUp) {
			diffMin = FLT_MAX;
			for (auto& enemy : enemys) {
				for (const auto& collider
					: enemy->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE))) {

					if (collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::HAND)
						|| collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::NECK)) continue;

					ColliderCapsule* colliderCapsule =
						dynamic_cast<ColliderCapsule*>(collider);

					if (colliderCapsule == nullptr
						|| lastTagerEnemy == colliderCapsule) continue;

					VECTOR enemyPos = colliderCapsule->GetCenter();

					float lockonDiff = VSize(VSub(enemyPos, playerPos));
					if (lockonDiff >= diffMin) continue;

					float dot = VDot(camera_->GetForward(), VNorm(VSub(enemyPos, playerPos)));
					float angle = acosf(dot);
					float a = AsoUtility::Deg2RadF(LOCKON_VIEW_ANGLE);
					if (angle >= a) continue;

					diffMin = lockonDiff;
					targetEnemy_ = colliderCapsule;
					camera->SetTargetFollow(&targetEnemy_->GetCenter());
					player_->SetTargetTransform(&targetEnemy_->GetCenter());
				}
			}
		}

		if (isNextDown) {
			for (auto& enemy : enemys) {
				for (const auto& collider
					: enemy->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE))) {
					if (collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::HAND)
						|| collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::NECK))continue;
					// カプセルコライダ情報  
					ColliderCapsule* colliderCapsule =
						dynamic_cast<ColliderCapsule*>(collider);

					if (colliderCapsule == nullptr
						|| lastTagerEnemy == colliderCapsule)continue;

					VECTOR enemyPos = colliderCapsule->GetCenter();

					//プレイヤーと敵のベクトルの大きさ
					float lockonDiff = VSize(VSub(enemyPos, playerPos));
					if (lockonDiff >= diffMin)continue;

					float dot = VDot(camera_->GetForward(), VNorm(VSub(enemyPos, playerPos)));
					float angle = acosf(dot);
					float a = AsoUtility::Deg2RadF(LOCKON_VIEW_ANGLE);
					if (angle >= a)continue;

					diffMin = lockonDiff;
					targetEnemy_ = colliderCapsule;
					camera->SetTargetFollow(&targetEnemy_->GetCenter());
					player_->SetTargetTransform(&targetEnemy_->GetCenter());
				}
			}
		}

		if (isNextLeft) {
			for (auto& enemy : enemys) {
				for (const auto& collider
					: enemy->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE))) {
					if (collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::HAND)
						|| collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::NECK))continue;
					// カプセルコライダ情報  
					ColliderCapsule* colliderCapsule =
						dynamic_cast<ColliderCapsule*>(collider);

					if (colliderCapsule == nullptr
						|| lastTagerEnemy == colliderCapsule)continue;

					VECTOR enemyPos = colliderCapsule->GetCenter();

					//プレイヤーと敵のベクトルの大きさ
					float lockonDiff = VSize(VSub(enemyPos, playerPos));
					if (lockonDiff >= diffMin)continue;

					float dot = VDot(camera_->GetForward(), VNorm(VSub(enemyPos, playerPos)));
					float angle = acosf(dot);
					float a = AsoUtility::Deg2RadF(LOCKON_VIEW_ANGLE);
					if (angle >= a)continue;

					VECTOR cross = VCross(VNorm(camera_->GetForward()), VSub(enemyPos, playerPos));
					if (cross.y > 0.0f)continue;

					diffMin = lockonDiff;
					targetEnemy_ = colliderCapsule;
					camera->SetTargetFollow(&targetEnemy_->GetCenter());
					player_->SetTargetTransform(&targetEnemy_->GetCenter());
				}
			}
		}

		if (isNextRight) {
			for (auto& enemy : enemys) {
				for (const auto& collider
					: enemy->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE))) {
					if (collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::HAND)
						|| collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::NECK))continue;
					// カプセルコライダ情報  
					ColliderCapsule* colliderCapsule =
						dynamic_cast<ColliderCapsule*>(collider);

					if (colliderCapsule == nullptr
						|| lastTagerEnemy == colliderCapsule)continue;

					VECTOR enemyPos = colliderCapsule->GetCenter();

					//プレイヤーと敵のベクトルの大きさ
					float lockonDiff = VSize(VSub(enemyPos, playerPos));
					if (lockonDiff >= diffMin)continue;

					float dot = VDot(camera_->GetForward(), VNorm(VSub(enemyPos, playerPos)));
					float angle = acosf(dot);
					float a = AsoUtility::Deg2RadF(LOCKON_VIEW_ANGLE);
					if (angle >= a)continue;

					VECTOR cross = VCross(VNorm(camera_->GetForward()), VSub(enemyPos, playerPos));
					if (cross.y < 0.0f)continue;

					diffMin = lockonDiff;
					targetEnemy_ = colliderCapsule;
					camera->SetTargetFollow(&targetEnemy_->GetCenter());
					player_->SetTargetTransform(&targetEnemy_->GetCenter());
				}
			}
		}
	}

	if (camera_->GetCameraMode() == Camera::MODE::FOLLOW
		&& isChanger == false) {
		if (!isLockOn)return;
		for (auto& enemy : enemys) {
			for (const auto& collider
				: enemy->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE))) {
				if (collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::TAIL)
					|| collider->GetPatrTag() == static_cast<int>(EnemyDragon::PATR_TAG::NECK))continue;
				// カプセルコライダ情報  
				ColliderCapsule* colliderCapsule =
					dynamic_cast<ColliderCapsule*>(collider);

				if (enemy == nullptr)continue;

				//プレイヤーと敵のベクトルの大きさ
				VECTOR enemyPos = colliderCapsule->GetCenter();

				float lockonDiff = VSize(VSub(enemyPos, playerPos));
				if (lockonDiff >= diffMin)continue;

				float dot = VDot(camera_->GetForward(), VNorm(VSub(enemyPos, playerPos)));
				float angle = acosf(dot);
				float a = AsoUtility::Deg2RadF(LOCKON_VIEW_ANGLE);
				if (angle >= a)continue;

				diffMin = lockonDiff;
				targetEnemy_ = colliderCapsule;
				// コライダが保持する座標の参照先を直接渡す（有効なライフタイムが保証される）
				const VECTOR* enemyCenter = &targetEnemy_->GetCenter();
				camera->SetTargetFollow(enemyCenter);
				player_->SetTargetTransform(enemyCenter);
				camera->ChangeMode(Camera::MODE::TARGET_ROCKE);
			}
		}
	}
}
