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
#include "../Utility/MatrixUtility.h"
#include "../Object/Actor/UI/UIHp.h"
#include "../Common/ShadowMap.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	SceneBase()
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Load(void)
{
	camera_ = SceneManager::GetInstance().GetCamera();
	//スカイドーム読み込み
	skydome_ = new SkyDome();
	skydome_->Load();
	//ステージ初期化
	stage_ = new Stage();
	stage_->Load();
	//プレイヤー読み込み
	player_ = new Player();
	player_->Load();
	//エネミー読み込み
	enemys_ = new EnemyManager();
	enemys_->Load();
	targetEnemy_ = nullptr;
	//シャドーマップ読み込み
	shadowMap_ = new ShadowMap(1024, 1024);
	//カメラモード変更
	camera_->SetFollow(&player_->GetTransform());
	camera_->ChangeMode(Camera::MODE::FOLLOW);
}

void GameScene::LoadEnd(void)
{
	//スカイドーム初期化
	skydome_->Init();
	//ステージ初期化
	stage_->Init();
	//プレイヤー初期化
	player_->Init();
	//エネミー初期化
	enemys_->Init();
	//シャドーマップ初期化
	shadowMap_->AddShadowMapLight(GetLightDirection());
	VECTOR playerPos = player_->GetTransform().pos;
	float shadowDiff = 1000.0f;
	shadowMap_->AddShadowMapDrawArea(
		VGet((playerPos.x - shadowDiff), -1.0f, (playerPos.z - shadowDiff)),
		VGet((playerPos.x + shadowDiff), shadowDiff * 1.5f, (playerPos.z + shadowDiff)));

	// コライダ登録
	AddCollider();
}

void GameScene::Update(void)
{
	// シーン遷移

	if (!player_->GetHP()->IsActive())
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
	}

	const auto& enemys = enemys_->GetEnemys();
	for (auto& enemy : enemys)
	{
		if (enemy == nullptr)continue;

		if (!enemy->GetHP()->IsActive())
		{
			sceMng_.ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
		}
	}

	skydome_->Update();

	//ステージ更新
	stage_->Update();

	UpdateAutoLockOn();

	enemys_->Update();
	player_->Update();

	UpdateCollider();

	for (const auto& enemy : enemys_->GetEnemys()) {
		player_->HitDamage(enemy->GetIsAttack());
	}
	enemys_->HitDamegr(player_->GetIsAttack());

	VECTOR playerPos = player_->GetTransform().pos;
	float shadowDiff = 1000.0f;
	shadowMap_->AddShadowMapDrawArea(
		VGet((playerPos.x - shadowDiff), -1.0f, (playerPos.z - shadowDiff)),
		VGet((playerPos.x + shadowDiff), shadowDiff * 1.5f, (playerPos.z + shadowDiff)));
}

void GameScene::Draw(void)
{
	skydome_->Draw();

	shadowMap_->DrawSetup();
	player_->Draw();
	enemys_->Draw();
	stage_->Draw();
	shadowMap_->DrawEnd();

	shadowMap_->SetShadow();
	player_->Draw();
	enemys_->Draw();
	stage_->Draw();
	shadowMap_->EndShadow();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();

	player_->DrawHp();
	for (const auto& enemy : enemys_->GetEnemys()) {
		enemy->DrawHp();
	}

	if(targetEnemy_ != nullptr){
		VECTOR enemyTransform = targetEnemy_->GetCenter();
		DrawSphere3D(enemyTransform,
			50.0f,10,
			0xfff000,
			0xfff000,
			true);

	}
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
	delete targetEnemy_;

	shadowMap_->Release();
	delete shadowMap_;

}

void GameScene::AddCollider(void)
{

	// 各クラスにステージコライダーを初期化時に登録
	const std::vector<ColliderBase*> stageCollider =
		stage_->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::MODEL));
	player_->AddHitCollider(static_cast<int>(ColliderBase::SHAPE::MODEL), stageCollider);
	enemys_->AddHitCollider(static_cast<int>(ColliderBase::SHAPE::MODEL), stageCollider);
	camera_->AddHitCollider(static_cast<int>(ColliderBase::SHAPE::MODEL), stageCollider);

	// 各クラスにプレイヤーコライダーを初期化時に登録
	const std::vector<ColliderBase*> pColliders =
		player_->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	enemys_->AddHitCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE), pColliders);

	// 各クラスにカメラコライダーを初期化時に登録
	const std::vector<ColliderBase*> cameraCollider =
		camera_->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::SPHERE));
	stage_->AddHitCollider(static_cast<int>(ColliderBase::SHAPE::SPHERE), cameraCollider);

	//各クラスにエネミーコライダーを初期化時に登録
	const auto& enemys = enemys_->GetEnemys();
	for (auto& enemy : enemys)
	{
		if (enemy == nullptr)continue;

		const std::vector<ColliderBase*> enemyColliders =
			enemy->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE));

		player_->AddHitCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE), enemyColliders);
	}
}

void GameScene::UpdateCollider(void)
{
	// 各クラスに武器コライダーを更新に登録
	const WeponBase* wepon = player_->GetWepon();
	const std::vector<ColliderBase*> weponColliders =
		wepon->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
	enemys_->AddHitCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE), weponColliders);

	const auto& enemys = enemys_->GetEnemys();
	for (auto& enemy : enemys)
	{
		if (enemy == nullptr) continue;

		const WeponBase* enemyWepon = enemy->GetWepon();
		if (enemyWepon != nullptr) {

			const std::vector<ColliderBase*> weponCollider1 =
				enemyWepon->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE));
			player_->AddHitCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE), weponCollider1);

			const std::vector<ColliderBase*> weponCollider2 =
				enemyWepon->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::SPHERE));
			player_->AddHitCollider(static_cast<int>(ColliderBase::SHAPE::SPHERE), weponCollider2);

		}
		else{
			player_->RemoveHitColliderByShapeAndTag(ColliderBase::SHAPE::CAPSULE, ColliderBase::TAG::ENEMY_WEPON);
			player_->RemoveHitColliderByShapeAndTag(ColliderBase::SHAPE::SPHERE, ColliderBase::TAG::ENEMY_WEPON);
		}
	}

	// 生存していない時は削除
	if (!wepon->GetIsAlive())
	{
		enemys_->RemoveCollider(ColliderBase::SHAPE::CAPSULE, ColliderBase::TAG::PLAYER_WEPON);
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
		|| inp.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER);

	if(camera_->GetCameraMode() == Camera::MODE::TARGET_ROCKE){

		VECTOR dir = AsoUtility::VECTOR_ZERO;
			InputManager::JOYPAD_IN_STATE padState =
				inp.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

			// アナログスティック方向
			dir = inp.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);

			static float prevStickX = 0.0f;
			static float prevStickZ = 0.0f;
			const float stickThreshold = InputManager::THRESHOLD;

			bool isNextUp = (dir.z > stickThreshold) && (prevStickZ <= stickThreshold);
			bool isNextDown = (dir.z < -stickThreshold) && (prevStickZ >= -stickThreshold);
			bool isNextRight = (dir.x > stickThreshold) && (prevStickX <= stickThreshold) || inp.GetMouseWheelRot() > 0.0f;
			bool isNextLeft = (dir.x < -stickThreshold) && (prevStickX >= -stickThreshold) || inp.GetMouseWheelRot() < 0.0f;

			prevStickX = dir.x;
			prevStickZ = dir.z;


		ColliderCapsule* lastTagerEnemy = targetEnemy_;

		VECTOR targetPos = targetEnemy_->GetCenter();
		float diff = VSize(VSub(targetPos, playerPos));

		if (diff >= MAX_LOCKON_DIFF || isLockOn){
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
					float a = AsoUtility::Deg2RadF(VIEW_ANGLE);
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
					float a = AsoUtility::Deg2RadF(VIEW_ANGLE);
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
					float a = AsoUtility::Deg2RadF(VIEW_ANGLE);
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
					float a = AsoUtility::Deg2RadF(VIEW_ANGLE);
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
				float a = AsoUtility::Deg2RadF(VIEW_ANGLE);
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
