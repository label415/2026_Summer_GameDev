#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Common/Fader.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/GameClearScene.h"
#include "../Scene/GameOverScene.h"
#include "../Scene/Loading/Loading.h"
#include "../Manager/SoundManager.h"
#include "Camera.h"
#include "../Application.h"
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

	// フェード機能の初期化
	fader_ = new Fader();
	fader_->Init();

	// ロード画面生成
	load_ = new Loading();
	load_->Load();

	// カメラ
	camera_ = new Camera();
	camera_->Init();

	// 画面遷移中判定
	isSceneChanging_ = false;

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 3D用の設定
	Init3D();

	// 初期シーンの設定
	DoChangeScene(SCENE_ID::TITLE);

	transitionPhase_ = TransitionPhase::NONE;
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

	fader_->Update();
	load_->Update();

	if (isSceneChanging_)
	{
		Fade();

		if (transitionPhase_ == TransitionPhase::FADE_OUT_OLD ||
			transitionPhase_ == TransitionPhase::FADE_IN_NEW)
		{
			scene_->Update();
		}
	}
	else
	{
		scene_->Update();
	}

	camera_->Update();
}

void SceneManager::Draw(void)
{
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();

	// ロード画面を表示するフェーズ(明転?ロード完了待ち?暗転)
	bool showLoad =
		(transitionPhase_ == TransitionPhase::FADE_IN_LOAD) ||
		(transitionPhase_ == TransitionPhase::WAIT_LOAD) ||
		(transitionPhase_ == TransitionPhase::FADE_OUT_LOAD);

	// シーンを表示するフェーズ(通常時 / 暗転前 / 明転後)
	bool showScene =
		(transitionPhase_ == TransitionPhase::NONE) ||
		(transitionPhase_ == TransitionPhase::FADE_OUT_OLD) ||
		(transitionPhase_ == TransitionPhase::FADE_IN_NEW);

	if (showLoad)
	{
		load_->Draw();
	}

	if (showScene)
	{
		camera_->SetBeforeDraw();
		scene_->Draw();
	}

	camera_->DrawDebug();
	fader_->Draw();
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

	// フェード機能の解放
	delete fader_;

	// インスタンスのメモリ解放
	delete instance_;

}

void SceneManager::ChangeScene(SCENE_ID nextId)
{

	waitSceneId_ = nextId;

	// まず今のシーンを暗転で隠すところから開始
	transitionPhase_ = TransitionPhase::FADE_OUT_OLD;
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;

	SoundManager::GetInstance().StopBGM();
	SoundManager::GetInstance().AllStopSE();

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

	// デルタタイム
	deltaTime_ = 1.0f / 60.0f;

	camera_ = nullptr;
	load_ = nullptr;
	fader_ = nullptr;

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

	Application::GetInstance().InitEffekseer();

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
	
	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;

}

void SceneManager::Fade(void)
{
	switch (transitionPhase_)
	{
	case TransitionPhase::FADE_OUT_OLD:
		// 暗転中:今のシーンを隠している
		if (fader_->IsEnd())
		{
			// 真っ暗になったので、ここでシーンを差し替え+ロード開始
			DoChangeScene(waitSceneId_);

			// ロード画面を明転で見せる
			transitionPhase_ = TransitionPhase::FADE_IN_LOAD;
			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		break;

	case TransitionPhase::FADE_IN_LOAD:
		// 明転中:ロード画面が見えてくる
		if (fader_->IsEnd())
		{
			// 完全に見えたので、ロード完了待ちへ
			transitionPhase_ = TransitionPhase::WAIT_LOAD;
		}
		break;

	case TransitionPhase::WAIT_LOAD:
		// ロード画面表示中:ロード完了を待つ
		if (load_->IsEnd())
		{
			// ロード完了したので、ロード画面を暗転で隠す
			transitionPhase_ = TransitionPhase::FADE_OUT_LOAD;
			fader_->SetFade(Fader::STATE::FADE_OUT);
			load_->EndAsyncLoad();
		}
		break;

	case TransitionPhase::FADE_OUT_LOAD:
		// 暗転中:ロード画面を隠している
		if (fader_->IsEnd())
		{
			// 真っ暗になったので、新シーンを明転で見せる
			transitionPhase_ = TransitionPhase::FADE_IN_NEW;
			fader_->SetFade(Fader::STATE::FADE_IN);
			scene_->LoadEnd();
		}
		break;

	case TransitionPhase::FADE_IN_NEW:
		// 明転中:新シーンが見えてくる
		if (fader_->IsEnd())
		{
			// 遷移完了
			transitionPhase_ = TransitionPhase::NONE;
			isSceneChanging_ = false;
		}
		break;

	default:
		break;
	}
}
