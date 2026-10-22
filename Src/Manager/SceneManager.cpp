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

	// 初期シーン(ロード画面を経由して読み込む)
	ChangeScene(SCENE_ID::TITLE);
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
	// ロード中は受け付けない
	if (load_->GetIsLoading()) return;

	scenes_.push_back(CreateScene(sceneId));

	scenes_.back()->Load();
	scenes_.back()->LoadEnd();
}

void SceneManager::PopScene(void)
{
	// 最後の1シーンは残す
	if (scenes_.size() <= 1) return;

	// 末尾のシーンを解放して削除する
	scenes_.back()->Release();
	scenes_.pop_back();

	// ポーズはゲームシーンの上にしか積まないため、
	// ゲームシーンに戻す
	sceneId_ = SCENE_ID::GAME;
	camera_->SetIsMouseInput(true);
	InputManager::GetInstance().SetMouseFlage(false);
}

void SceneManager::Update(void)
{
	// ロード画面の更新(完了フラグを1フレームで下ろすため毎フレーム呼ぶ)
	load_->Update();

	// ロード画面を表示した次のフレームで、非同期読み込みを開始する
	if (isLoadStart_)
	{
		isLoadStart_ = false;
		load_->StartAsyncLoad();
		scenes_.back()->Load();
		load_->EndAsyncLoad();
		return;
	}

	// ロードが終わったフレームで一度だけ、読み込み後の初期化を行う
	if (load_->GetIsLoadEnd())
	{
		scenes_.back()->LoadEnd();
	}

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
	if (load_->GetIsLoading())
	{
		load_->Draw();
		return;
	}

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
	ResetResource();

	// 新しいシーンを生成(読み込みは次のフレームで行う)
	scenes_.push_back(CreateScene(nextSceneId_));

	// ロード画面の表示開始
	load_->StartLoading();
	isLoadStart_ = true;
}

void SceneManager::ReleaseAllScenes(void)
{
	// 上に積まれているシーンから順に解放する
	for (auto& scene : scenes_)
	{
		scene->Release();
	}
	scenes_.clear();
}

void SceneManager::ResetResource(void)
{
	SoundManager::GetInstance().Release();
	ResourceManager::GetInstance().Release();
	CollisionManager::GetInstance().Release();
	Application::GetInstance().InitEffekseer();

	camera_->Release();
}

SceneManager::SceneManager(void) :
	sceneId_(SCENE_ID::TITLE)
	, nextSceneId_(SCENE_ID::TITLE)
	, isChangeRequest_(false)
	, isLoadStart_(false)
	, deltaTime_(0.0f)
{
}