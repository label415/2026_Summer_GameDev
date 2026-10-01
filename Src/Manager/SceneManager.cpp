#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Common/Fader.h"
#include "../Manager/SoundManager.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/Loading/Loading.h"
#include "../Application.h"
#include "Camera.h"
#include "ResourceManager.h"
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
	// シーンIDの初期化
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
	scene_->LoadEnd();

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
	// シーンが存在しない場合は更新しない
	if (scene_ == nullptr){return;}

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	// フェードとロード画面の更新
	fader_->Update();
	load_->Update();

	// シーンの更新
	if (isSceneChanging_)
	{
		// シーン遷移中はフェード処理を行う
		Fade();

		// シーンの更新は暗転中と明転中のみ行う
		if (transitionPhase_ == TransitionPhase::FADE_OUT_OLD ||
			transitionPhase_ == TransitionPhase::FADE_IN_NEW)
		{
			scene_->Update();
		}
	}
	else
	{
		// 通常時はシーンを更新する
		scene_->Update();
	}

	// カメラの更新
	camera_->Update();
}

void SceneManager::Draw(void)
{
	// 描画先を裏画面に設定して、裏画面をクリアする
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();

	// ロード画面を表示するフェーズ
	bool showLoad =
		(transitionPhase_ == TransitionPhase::FADE_IN_LOAD) ||
		(transitionPhase_ == TransitionPhase::WAIT_LOAD) ||
		(transitionPhase_ == TransitionPhase::FADE_OUT_LOAD);

	// シーンを表示するフェーズ
	bool showScene =
		(transitionPhase_ == TransitionPhase::NONE) ||
		(transitionPhase_ == TransitionPhase::FADE_OUT_OLD) ||
		(transitionPhase_ == TransitionPhase::FADE_IN_NEW);


	if (showLoad)
	{
		// ロード画面の描画
		load_->Draw();
	}

	if (showScene)
	{
		// シーンの描画
		camera_->SetBeforeDraw();
		scene_->Draw();
	}

	// デバッグ用描画
	camera_->DrawDebug();

	// フェード描画
	fader_->Draw();
}

void SceneManager::Destroy(void)
{
	// シーンの解放
	if (scene_ != nullptr)
	{
		delete scene_;
	}

	// カメラの解放
	camera_->Release();
	delete camera_;

	// ロード画面の削除
	load_->Release();
	delete load_;

	// フォント管理クラスの解放
	FontManager::GetInstance().Destroy();

	// フェード機能の解放
	delete fader_;

	// インスタンスのメモリ解放
	delete instance_;
}

void SceneManager::ChangeScene(SCENE_ID nextId)
{
	// シーン遷移中は変更しない
	waitSceneId_ = nextId;

	// まず今のシーンを暗転で隠すところから開始
	transitionPhase_ = TransitionPhase::FADE_OUT_OLD;
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;

	// BGMとSEを停止
	SoundManager::GetInstance().StopBGM();
	SoundManager::GetInstance().AllStopSE();
}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	return DELTA_TIME;
}

Camera* SceneManager::GetCamera(void) const
{
	return camera_;
}

SceneManager::SceneManager(void)
{
	// シーンIDの初期化
	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	// デルタタイム
	deltaTime_ = DELTA_TIME;

	// 各ポインタ変数初期化
	scene_ = nullptr;
	camera_ = nullptr;
	load_ = nullptr;
	fader_ = nullptr;
}

void SceneManager::ResetDeltaTime(void)
{
	// デルタタイムを初期化する
	deltaTime_ = DELTA_TIME_RESET;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	// リソースの解放
	ResourceManager::GetInstance().Release();

	// エフェクシアの初期化
	Application::GetInstance().InitEffekseer();

	// シーンを変更する
	sceneId_ = sceneId;

	// 現在のシーンを解放
	if (scene_ != nullptr)
	{
		delete scene_;
	}

	// 新しいシーンを生成
	switch (sceneId_)
	{
	case SCENE_ID::TITLE:
		scene_ = new TitleScene();
		break;
	case SCENE_ID::GAME:
		scene_ = new GameScene();
		break;
	}

	// 各シーンの初期化
	load_->StartAsyncLoad();
	scene_->Load();
	
	// デルタタイムをリセット
	ResetDeltaTime();

	// 待機シーンIDを初期化
	waitSceneId_ = SCENE_ID::NONE;
}

void SceneManager::Fade(void)
{
	// フェード処理の状態に応じて処理を分岐
	switch (transitionPhase_)
	{
	case TransitionPhase::FADE_OUT_OLD:
		// 暗転中:今のシーンを隠している
		if (fader_->IsEnd())
		{
			// 真っ暗になったので、ここでシーンを差し替え+ロード開始
			DoChangeScene(waitSceneId_);

			// ロード画面を明転で見せる
			fader_->SetFade(Fader::STATE::FADE_IN);
			transitionPhase_ = TransitionPhase::FADE_IN_LOAD;
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
