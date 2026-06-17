#include <DxLib.h>
#include "../Manager/Camera.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/FontManager.h"
#include "../Object/Actor/Wepon/WeponBase.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/SkyDome.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/Enemy/EnemyManager.h"
#include "../Object/Actor/Charactor/Enemy/EnemyDragon.h"
#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Utility/MatrixUtility.h"
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
	camera_ = SceneManager::GetInstance().GetCamera();

	skydome_ = new SkyDome();
	skydome_->Init();

	//ステージ初期化
	stage_ = new Stage();
	stage_->Init();

	player_ = new Player();
	camera_->SetFollow(&player_->GetTransform());
	player_->Init();

	enemys_ = new EnemyManager();
	enemys_->Init();
	targetEnemy_ = nullptr;

	camera_->ChangeMode(Camera::MODE::FOLLOW);

	// コライダ登録
	AddCollider();

	ShadowMapHandle = MakeShadowMap(1024, 1024);

	// シャドウマップが想定するライトの方向もセット
	SetShadowMapLightDirection(ShadowMapHandle, VGet(0.3f, -0.7f, 0.8f));

	// シャドウマップに描画する範囲を設定
	SetShadowMapDrawArea(ShadowMapHandle, VGet(-1000.0f, -200.0f, -1000.0f), VGet(1000.0f, 1000.0f, 1000.0f));
}

void GameScene::Update(void)
{
	// シーン遷移
	/*auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}*/

	UpdateAutoLockOn();

	skydome_->Update();

	//ステージ更新
	stage_->Update();

	player_->Update();
	for(const auto& enemy : enemys_->GetEnemys()){
		player_->HitDamage(enemy->GetIsAttack());
	}

	enemys_->Update();
	enemys_->HitDamegr(player_->GetIsAttack());

	UpdateCollider();
}

void GameScene::Draw(void)
{
	skydome_->Draw();

	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup(ShadowMapHandle);
	//ステージ描画
	stage_->Draw();
	player_->Draw();
	enemys_->Draw();
	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();

	// 描画に使用するシャドウマップを設定
	SetUseShadowMap(0, ShadowMapHandle);
	//ステージ描画
	stage_->Draw();
	player_->Draw();
	enemys_->Draw();
	// 描画に使用するシャドウマップの設定を解除
	SetUseShadowMap(0, -1);

	if(targetEnemy_ != nullptr)
	{
		VECTOR enemyTransform = targetEnemy_->GetCenter();
		DrawSphere3D(enemyTransform,
			50.0f,
			10,
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

	// シャドウマップの削除
	DeleteShadowMap(ShadowMapHandle);
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

	bool isLockOn = inp.IsTrgMouseMiddle();

	if(camera_->GetCameraMode() == Camera::MODE::TARGET_ROCKE){

		bool isNextUp = inp.IsTrgDown(KEY_INPUT_UP);
		bool isNextDown = inp.IsTrgDown(KEY_INPUT_DOWN);
		bool isNextLeft = inp.IsTrgDown(KEY_INPUT_LEFT);
		bool isNextRight = inp.IsTrgDown(KEY_INPUT_RIGHT);

		ColliderCapsule* lastTagerEnemy = targetEnemy_;

		VECTOR targetPos = targetEnemy_->GetCenter();
		float diff = VSize(VSub(targetPos, playerPos));

		if (diff >= MAX_LOCKON_DIFF || isLockOn){
			camera_->ChangeMode(Camera::MODE::FOLLOW);
			targetEnemy_ = nullptr;
			isChanger = true;
			player_->SetTargetTransform(nullptr);
		}

		if (isNextUp){
			diffMin = 0.0f;
			for (auto& enemy : enemys){
				for (const auto& collider 
					: enemy->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::CAPSULE))){

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
					diffMin = VSize(VSub(enemyPos, playerPos));
					if (lockonDiff <= diffMin)continue;

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
