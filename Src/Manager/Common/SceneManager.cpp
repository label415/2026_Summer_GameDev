#include <chrono>
#include <DxLib.h>
#include "../../Scene/Loading/Loading.h"
#include "../../Common/Fader.h"
#include "../../Scene/TitleScene.h"
#include "../../Scene/GameScene.h"
#include "../../Scene/PauseScene.h"
#include "SoundManager.h"
#include "InputManager.h"
#include "../../Application.h"
#include "../../Object/Camera.h"
#include "ResourceManager.h"
#include "EffectManager.h"
#include "SceneManager.h"

void SceneManager::Init(void)
{
	// ロード画面生成
	load_ = std::make_unique<Loading>();
	load_->Load();

	// カメラ
	camera_ = std::make_shared<Camera>();
	camera_->LoadEnd();

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 3D用の設定
	Init3D();

	// 初期シーン
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
	// 実行中のシーンを破棄しないよう、要求だけ受け付ける
	nextSceneId_ = sceneId;
	DoChangeScene();
}

void SceneManager::PushScene(SCENE_ID sceneId)
{
	// 既に積まれているシーンの更新を止める
	scenes_.push_back(CreateScene(sceneId));

	// 読み込み
	scenes_.back()->Load();
	// 読み込み後の初期化
	scenes_.back()->LoadEnd();
}

void SceneManager::PopScene(void)
{
	// 最後の1シーンは残す
	if (scenes_.size() <= 1) return;

	// 末尾のシーンを解放して削除する
	scenes_.back()->Release();
	scenes_.pop_back();
}

void SceneManager::Update(void)
{
	// ロード画面の更新
	load_->Update();

	// ロード中はシーンを更新しない
	if (load_->GetIsLoading()) return;

	if (!scenes_.empty())
	{
		scenes_.back()->Update();
	}

	camera_->Update();
}

void SceneManager::Draw(void)
{
	// 描画先グラフィック領域を裏画面に指定
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化する
	ClearDrawScreen();

	// ロード中はロード画面のみ描画する
	load_->Draw();

	// ロード中はシーンを更新しない
	if (load_->GetIsLoading()) return;

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
	// 残っているシーンの解放
	ReleaseAllScenes();

	// カメラの解放
	camera_->Release();

	// ロード画面の解放
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

void SceneManager::DoChangeScene(void)
{
	// 積まれている全シーンを解放する
	ReleaseAllScenes();

	// サウンド・リソース・当たり判定・カメラのリセット
	ResetSceneResources();

	// 新しいシーンを生成
	scenes_.push_back(CreateScene(nextSceneId_));

	// 読み込み
	load_->StartAsyncLoad();
	scenes_.back()->Load();
	load_->EndAsyncLoad();
	scenes_.back()->LoadEnd();
}

void SceneManager::ReleaseAllScenes(void)
{
	// 上に積まれているシーンから順に解放する
	for (auto& scene : scenes_)
	{
		scene->Release();
	}

	// シーンの解放が終わったら、シーンの配列をクリアする
	scenes_.clear();
}

void SceneManager::ResetSceneResources(void)
{
	// リソースの解放
	SoundManager::GetInstance().AllStopSound();
	EffectManager::GetInstance().AllStop();
	ResourceManager::GetInstance().Reset();
	
	// カメラのリセット
	camera_->Release();
}

SceneManager::SceneManager(void)
	: sceneId_(SCENE_ID::NONE)
	, nextSceneId_(SCENE_ID::NONE)
	, deltaTime_(0.0f)
{
}