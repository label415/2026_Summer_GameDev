#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Common/Fader.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/GameClearScene.h"
#include "../Scene/GameOverScene.h"
#include "../Scene/Loading/Loading.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "../Scene/DebugScene.h"
#include "FontManager.h"
#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Init(void)
{

	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	// フォント管理クラス生成
	FontManager::CreateInstance();

	// ロード画面生成
	load_ = new Loading();
	load_->Init();
	load_->Load();

	// カメラ
	camera_ = new Camera();
	camera_->Init();

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 3D用の設定
	Init3D();

	isSceneChanging_ = false;

	// 遷移待ちタイマー初期化
	sceneChangeDelayTimer_ = 0.0f;

	// 初期シーンの設定
	DoChangeScene(SCENE_ID::TITLE);
}

void SceneManager::Init3D(void)
{

	// 背景色設定
	SetBackgroundColor(
		BACKGROUND_COLOR_R, 
		BACKGROUND_COLOR_G, 
		BACKGROUND_COLOR_B);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);
	ChangeLightTypeDir(VGet(-0.5f, -1.0f, -0.5f));
	SetLightDifColor(GetColorF(1.0f, 1.0f, 0.95f, 1.0f));
	SetLightAmbColor(GetColorF(0.4f, 0.4f, 0.45f, 1.0f)); 

	// フォグ設定
	SetFogEnable(true);
	SetFogColor(5, 5, 5);
	SetFogStartEnd(10000.0f, 20000.0f);

}

void SceneManager::Update(void)
{
	if (scene_ == nullptr){return;}

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	// ロード中
	if (isSceneChanging_){
		// タイトルへ戻る際に、直前が GAMEOVER/GAMECLEAR の場合は遅延を入れる
		if (sceneChangeDelayTimer_ > 0.0f)
		{
			sceneChangeDelayTimer_ -= deltaTime_;
			// 遅延中もロード画面は更新して表示を保つ
			load_->Update();
			// タイマーがまだ残っていれば遷移はまだ行わない
			if (sceneChangeDelayTimer_ > 0.0f)
			{
				return;
			}
		}

		// 遅延が終わった（または遅延不要）ので実際にシーン変更処理を行う
		load_->Update();
		DoChangeScene(waitSceneId_);
		if (load_->IsLoading())
		{
			isSceneChanging_ = false;
		}
	}
	// 通常の更新処理
	else{
		// 現在のシーンの更新
		scene_->Update();
	}

	// カメラ更新
	camera_->Update();
}

void SceneManager::Draw(void)
{
	
	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化
	ClearDrawScreen();

	// カメラ設定
	camera_->SetBeforeDraw();

	// ロード中ならロード画面を描画
	if (isSceneChanging_)
	{
		// ロードの描画
		load_->Draw();
	}
	// 通常の更新
	else
	{
		// Effekseerにより再生中のエフェクトを更新する。
		UpdateEffekseer3D();

		// 各シーンの描画処理
		scene_->Draw();

		// カメラ描画
		camera_->DrawDebug();

		// Effekseerにより再生中のエフェクトを描画する。
		DrawEffekseer3D();
	}
}

void SceneManager::Destroy(void)
{

	// シーンの解放
	if (scene_ != nullptr)
	{
		delete scene_;
	}

	camera_->Release();
	delete camera_;

	// ロード画面の削除
	load_->Release();
	delete load_;

	FontManager::GetInstance().Destroy();

	// インスタンスのメモリ解放
	delete instance_;

}

void SceneManager::ChangeScene(SCENE_ID nextId)
{

	// 遷移先シーンを保持
	waitSceneId_ = nextId;

	// タイトルへ戻る場合で、現在が GAMEOVER または GAMECLEAR のときは
	// 読み込みを長く見せるための遅延を設定する
	if (
		(nextId == SCENE_ID::TITLE &&(sceneId_ == SCENE_ID::GAMEOVER || sceneId_ == SCENE_ID::GAMECLEAR))
		|| ((nextId == SCENE_ID::GAMEOVER || nextId == SCENE_ID::GAMECLEAR) && sceneId_ == SCENE_ID::GAME)
		)
	{
		sceneChangeDelayTimer_ = TITLE_RETURN_DELAY;
	}
	else
	{
		sceneChangeDelayTimer_ = 0.0f;
	}

	isSceneChanging_ = true;

}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	return 1.0f / 60.0f;
}

Camera* SceneManager::GetCamera(void) const
{
	return camera_;
}

SceneManager::SceneManager(void)
{

	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	scene_ = nullptr;

	isSceneChanging_ = false;

	// デルタタイム
	deltaTime_ = 1.0f / 60.0f;

	camera_ = nullptr;
	load_ = nullptr;
	sceneChangeDelayTimer_ = 0.0f;

}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{

	// リソースの解放
	ResourceManager::GetInstance().Release();

	// シーンを変更する
	sceneId_ = sceneId;

	// 現在のシーンを解放
	if (scene_ != nullptr)
	{
		delete scene_;
	}

	switch (sceneId_)
	{
	case SCENE_ID::TITLE:
		scene_ = new TitleScene();
		break;
	case SCENE_ID::GAME:
		scene_ = new GameScene();
		break;
	case SCENE_ID::GAMEOVER:
		scene_ = new GameOverScene();
		break;
	case SCENE_ID::GAMECLEAR:
		scene_ = new GameClearScene();
		break;
	case SCENE_ID::DEBUG:
		scene_ = new DebugScene();
		break;
	}

	// 各シーンの初期化
	load_->StartAsyncLoad();
	scene_->Load();
	load_->EndAsyncLoad();
	scene_->LoadEnd();
	
	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;

}