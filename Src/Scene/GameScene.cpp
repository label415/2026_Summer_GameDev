#include <DxLib.h>
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/Enemy/EnemyManager.h"
#include "../Object/Actor/Charactor/Enemy/EnemyDragon.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/SkyDome.h"
#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Common/ShadowMap.h"
#include "../Manager/Camera.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/FontManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Application.h"
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
	targetColliderCapsule_ = nullptr;

	// シャドーマップ読み込み
	shadowMap_ = std::make_unique<ShadowMap>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

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
}

void GameScene::Update(void)
{
	camera_->SetIsMouseInput(true);
	InputManager::GetInstance().SetMouseFlage(false);

	// プレイヤーが死亡していた時,タイトルシーンに遷移
	if (player_->GetState() == Player::STATE::END) {
		/*sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);*/
		return;
	}

	// エネミーが死亡していた時,タイトルシーンに遷移
	for (const auto& enemy : enemys_->GetEnemys())
	{
		if (enemy->GetState() == static_cast<int>(EnemyDragon::STATE::END)) {
			/*sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);*/
			return;
		}
	}

	// ポーズメニューの表示切り替え
	bool isSelect = InputManager::GetInstance().IsTrgDown(KEY_INPUT_ESCAPE)
		|| InputManager::GetInstance().IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START);
	if (isSelect) {
		sceMng_.PushScene(SceneManager::SCENE_ID::PAUSE);
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
	if (targetColliderCapsule_ != nullptr) {
		VECTOR enemyTransform = targetColliderCapsule_->GetCenter();
		SetUseZBuffer3D(FALSE);
		DrawBillboard3D(
			enemyTransform,
			0.5f, 0.5f,
			LOCON_UI_SIZE, 0.0f,
			lockOnImg_, true);
		SetUseZBuffer3D(TRUE);
	}

	// すべての敵が死亡した時、ゲームクリアUIを表示する
	for (const auto& enemy : enemys_->GetEnemys())
	{
		if (enemy->GetState() == static_cast<int>(EnemyDragon::STATE::DEAD))
		{
			resultAlpha_ = static_cast<int>(
				enemy->GetDesath() * RESULT_UI_ALPHA_MAGNIFICATION);
			resultImg_ = gameClearImg_;
			isResultUI_ = true;
		}
	}

	// プレイヤーが死亡した時、ゲームオーバーUIを表示する
	if (player_->GetState() == Player::STATE::DIE)
	{
		resultAlpha_ = static_cast<int>(
			player_->GetDesath() * RESULT_UI_ALPHA_MAGNIFICATION);
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
	std::shared_ptr<Camera> camera = SceneManager::GetInstance().GetCamera();
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

		std::shared_ptr<ColliderCapsule>  lastTagerEnemy = targetColliderCapsule_;

		VECTOR targetPos = targetColliderCapsule_->GetCenter();
		float diff = VSize(VSub(targetPos, playerPos));

		if (diff >= MAX_LOCKON_DIFF || isLockOn) {
			camera_->ChangeMode(Camera::MODE::FOLLOW);
			targetColliderCapsule_ = nullptr;
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

					std::shared_ptr<ColliderCapsule> colliderCapsule =
						std::dynamic_pointer_cast<ColliderCapsule>(collider);

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
					targetColliderCapsule_ = colliderCapsule;
					camera->SetTargetFollow(&targetColliderCapsule_->GetCenter());
					player_->SetTargetTransform(&targetColliderCapsule_->GetCenter());
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
					targetColliderCapsule_ = colliderCapsule;
					camera->SetTargetFollow(&targetColliderCapsule_->GetCenter());
					player_->SetTargetTransform(&targetColliderCapsule_->GetCenter());
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
					targetColliderCapsule_ = colliderCapsule;
					camera->SetTargetFollow(&targetColliderCapsule_->GetCenter());
					player_->SetTargetTransform(&targetColliderCapsule_->GetCenter());
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
					targetColliderCapsule_ = colliderCapsule;
					camera->SetTargetFollow(&targetColliderCapsule_->GetCenter());
					player_->SetTargetTransform(&targetColliderCapsule_->GetCenter());
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
				targetColliderCapsule_ = colliderCapsule;
				// コライダが保持する座標の参照先を直接渡す（有効なライフタイムが保証される）
				const VECTOR* enemyCenter = &targetColliderCapsule_->GetCenter();
				camera->SetTargetFollow(enemyCenter);
				player_->SetTargetTransform(enemyCenter);
				camera->ChangeMode(Camera::MODE::TARGET_ROCKE);
			}
		}
	}
}
