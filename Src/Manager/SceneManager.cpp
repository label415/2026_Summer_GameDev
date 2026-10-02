#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Scene/Loading/Loading.h"
#include "../Common/Fader.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/PauseScene.h"
#include "../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
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
	// フォント管理クラス生成
	FontManager::CreateInstance();

	// ロード画面生成
	load_ = new Loading();
	load_->Load();

	// カメラ
	camera_ = new Camera();
	camera_->Init();

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 3D用の設定
	Init3D();

	//初期シーンをプッシュ
	PushScene(SCENE_ID::TITLE);
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

void SceneManager::ChangeScene(SCENE_ID sceneId)
{
	if (scenes_.empty()) {
		scenes_.push_back(CreateScene(sceneId));
		return;
	}
	scenes_.back() = CreateScene(sceneId);

	scenes_.back()->Load();
	scenes_.back()->LoadEnd();
	camera_->SetIsMouseInput(false);
	InputManager::GetInstance().SetMouseFlage(true);
}

void SceneManager::PushScene(SCENE_ID sceneId)
{
	//末尾にシーンを追加する
	scenes_.push_back(CreateScene(sceneId));

	scenes_.back()->Load();
	scenes_.back()->LoadEnd();
	camera_->SetIsMouseInput(false);
	InputManager::GetInstance().SetMouseFlage(true);
}

void SceneManager::PopScene()
{
	//末尾のシーンを削除する
	if (scenes_.size() > 1)
	{
		scenes_.pop_back();
	}
}

void SceneManager::ResetScene(SCENE_ID sceneId)
{
	scenes_.clear();
	scenes_.push_back(CreateScene(sceneId));

	scenes_.back()->Load();
	scenes_.back()->LoadEnd();
	camera_->SetIsMouseInput(false);
	InputManager::GetInstance().SetMouseFlage(true);
}

void SceneManager::Update(void)
{
	//末尾のやつだけUpdate
	scenes_.back()->Update();

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

	//乗ってるシーンをすべてDraw
	for (auto& scene : scenes_)
	{
		scene->Draw();
	}

	// カメラ描画
	camera_->DrawDebug();
}

void SceneManager::Destroy(void)
{
	camera_->Release();
	delete camera_;

	// ロード画面の削除
	load_->Release();
	delete load_;

	FontManager::GetInstance().Destroy();

	// インスタンスのメモリ解放
	delete instance_;
}

std::unique_ptr<SceneBase> SceneManager::CreateScene(SCENE_ID sceneId)
{
	// シーンIDを更新
	sceneId_ = sceneId;

	// インスタンス生成
	switch (sceneId_) {
	case SCENE_ID::TITLE:
		return std::make_unique<TitleScene>();
	case SCENE_ID::GAME:
		return std::make_unique<GameScene>();
	case SCENE_ID::PAUSE:
		return std::make_unique<PauseScene>();
	default:
		return nullptr;
	}
}
