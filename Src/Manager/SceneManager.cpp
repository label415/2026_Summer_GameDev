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
#include "../Manager/CollisionManager.h"
#include "../Application.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "FontManager.h"
#include "SceneManager.h"

void SceneManager::Init(void)
{
	// ロード画面生成
	load_ = std::make_unique<Loading>();
	load_->Load();

	// カメラ
	camera_ = std::make_shared<Camera>();
	camera_->Init();

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 3D用の設定
	Init3D();

	//初期シーン
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
	// ライトの方向
	ChangeLightTypeDir(LIGHT_DIRECTION);
	// ライトのディフューズカラー
	SetLightDifColor(LIGHT_DIFF_COLOR);
	// ライトのアンビエントカラー
	SetLightAmbColor(LIGHT_AMB_COLOR);

	// フォグ設定
	SetFogEnable(true);
	// フォグの色
	SetFogColor(
		FOG_COLOR_R,
		FOG_COLOR_G,
		FOG_COLOR_B);
	// フォグ表示範囲
	SetFogStartEnd(FOG_START, FOG_END);
}

void SceneManager::ChangeScene(SCENE_ID sceneId)
{
	// SE・BGM解放
	SoundManager::GetInstance().Release();

	// リソース解放
	ResourceManager::GetInstance().Release();

	// 当たり判定情報解放
	CollisionManager::GetInstance().Release();

	if (scenes_.empty()) {
		scenes_.push_back(CreateScene(sceneId));
		return;
	}
	scenes_.back() = CreateScene(sceneId);
	scenes_.back()->Load();
	scenes_.back()->LoadEnd();

	// カメラのマウス入力を無効化
	camera_->SetIsMouseInput(false);
	// 入力管理のマウスフラグを有効化
	InputManager::GetInstance().SetMouseFlage(true);
}

void SceneManager::PushScene(SCENE_ID sceneId)
{
	scenes_.push_back(CreateScene(sceneId));

	scenes_.back()->Load();
	scenes_.back()->LoadEnd();

	// カメラのマウス入力を無効化
	camera_->SetIsMouseInput(false);
	// 入力管理のマウスフラグを有効化
	InputManager::GetInstance().SetMouseFlage(true);
}

void SceneManager::PopScene(void)
{
	//末尾のシーンを削除する
	if (scenes_.size() > 1)
	{
		scenes_.pop_back();
	}
}

void SceneManager::ResetScene(SCENE_ID sceneId)
{
	// SE・BGM解放
	SoundManager::GetInstance().Release();

	// リソース解放
	ResourceManager::GetInstance().Release();

	// 当たり判定情報解放
	CollisionManager::GetInstance().Release();

	scenes_.clear();
	scenes_.push_back(CreateScene(sceneId));

	scenes_.back()->Load();
	scenes_.back()->LoadEnd();

	// カメラのマウス入力を無効化
	camera_->SetIsMouseInput(false);
	// 入力管理のマウスフラグを有効化
	InputManager::GetInstance().SetMouseFlage(true);
}

void SceneManager::Update(void)
{
	scenes_.back()->Update();

	camera_->Update();
}

void SceneManager::Draw(void)
{
	// 描画先グラフィック領域を裏画面に指定
	//（カメラ設定などもリセットされる）
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化する
	ClearDrawScreen();

	// カメラの描画前設定を行う
	camera_->SetBeforeDraw();

	for (auto& scene : scenes_)
	{
		scene->Draw();
	}

	camera_->DrawDebug();
}

void SceneManager::Release(void)
{
	// カメラの解放
	camera_->Release();

	// ロード画面の削除
	load_->Release();
}

std::unique_ptr<SceneBase> SceneManager::CreateScene(SCENE_ID sceneId)
{
	// 現在のシーンIDを更新
	sceneId_ = sceneId;

	// シーンIDに応じたインスタンスを生成して返す
	switch (sceneId_)
	{
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

SceneManager::SceneManager(void) :
	sceneId_(SCENE_ID::TITLE)
	, deltaTime_(0.0f)
{
}
