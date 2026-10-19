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
	// 次のシーンへ遷移する
	ReserveSceneChange(sceneId, true);
}

void SceneManager::PushScene(SCENE_ID sceneId)
{
	// ポーズ画面はロード画面を挟まず即座に重ねる
	if (sceneId == SCENE_ID::PAUSE)
	{
		PushSceneImmediate(sceneId);
		return;
	}

	// 既存シーンを残す設定で次のシーンを予約
	ReserveSceneChange(sceneId, false);
}

void SceneManager::PopScene(void)
{
	//末尾のシーンを削除する
	if (scenes_.size() > 1)
	{
		scenes_.pop_back();
	}
}

void SceneManager::Update(void)
{
	if (IsLoadingState())
	{
		UpdateLoading();
		return;
	}

	// 通常時のシーンとカメラの更新を行う
	UpdateNormal();
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

	// ロード処理中はシーンを描画せずロード画面のみを描画する
	if (IsLoadingState())
	{
		// ロード画面描画
		load_->Draw();
		return;
	}

	// 通常時はシーン群を描画する
	DrawScenes();
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
	, nextSceneId_(SCENE_ID::TITLE)
	, isClearScenes_(true)
	, loadState_(LOAD_STATE::NONE)
	, deltaTime_(0.0f)
{
}

bool SceneManager::IsLoadingState(void) const
{
	// ロード中か判定する
	return (loadState_ != LOAD_STATE::NONE) || load_->IsLoading();
}

void SceneManager::ReserveSceneChange(SCENE_ID sceneId, bool isClearScenes)
{
	if (IsLoadingState()) return;

	// 次に生成するシーンIDを保存
	nextSceneId_ = sceneId;

	// 古いシーンを消去するかどうかのフラグを保存
	isClearScenes_ = isClearScenes;

	// ロード画面の表示を開始
	load_->StartLoading();

	// まずはロード画面を1フレーム描画させる状態にする
	loadState_ = LOAD_STATE::WAIT_DRAW_LOADING;

	// カメラとマウス入力の状態をリセット
	ResetCameraAndInput();
}

void SceneManager::PushSceneImmediate(SCENE_ID sceneId)
{
	SetUseASyncLoadFlag(false);

	// シーンを生成して末尾に追加
	scenes_.push_back(CreateScene(sceneId));

	// リソースの読み込みと初期化を即座に実行
	scenes_.back()->Load();
	scenes_.back()->LoadEnd();

	// カメラとマウス入力の状態をリセット
	ResetCameraAndInput();
}

void SceneManager::ResetCameraAndInput(void)
{
	// カメラのマウス入力を無効化
	camera_->SetIsMouseInput(false);

	// 入力管理のマウスフラグを有効化
	InputManager::GetInstance().SetMouseFlage(true);
}

void SceneManager::UpdateLoading(void)
{
	// ロード画面のタイマーと状態を更新
	load_->Update();

	switch (loadState_)
	{
	case LOAD_STATE::WAIT_DRAW_LOADING:
		loadState_ = LOAD_STATE::EXECUTE_LOAD;
		break;
	case LOAD_STATE::EXECUTE_LOAD:
		ExecuteSceneLoad();
		break;
	case LOAD_STATE::WAIT_ASYNC_LOAD:
		if (load_->IsLoading())return;
		FinishSceneLoad();
		break;
	default:
		break;
	}
}

void SceneManager::ExecuteSceneLoad(void)
{
	// 各シーンの解放
	if (isClearScenes_)
	{
		scenes_.clear();
	}

	// リソース解放とシーンのインスタンス生成は同期状態で安全に行う
	SetUseASyncLoadFlag(false);

	// 前シーンのリソースを一括解放
	ReleaseSceneResources();

	// 新しいシーンを生成して末尾に追加
	scenes_.push_back(CreateScene(nextSceneId_));

	// ファイル読み込みの間だけ非同期読み込みを有効にする
	load_->StartAsyncLoad();
	scenes_.back()->Load();
	load_->EndAsyncLoad();

	// 非同期読み込みの完了待ち状態へ移行
	loadState_ = LOAD_STATE::WAIT_ASYNC_LOAD;
}

void SceneManager::FinishSceneLoad(void)
{
	// 同期状態に戻してからLoadEndを実行する
	SetUseASyncLoadFlag(false);
	scenes_.back()->LoadEnd();

	// ロード状態を通常に戻す
	loadState_ = LOAD_STATE::NONE;
}

void SceneManager::UpdateNormal(void)
{
	// シーンが存在する場合は末尾のシーンのみ更新する
	if (!scenes_.empty())
	{
		scenes_.back()->Update();
	}

	// カメラを更新する
	camera_->Update();
}

void SceneManager::DrawScenes(void) const
{
	// 積まれているすべてのシーンを順番に描画する
	for (const auto& scene : scenes_)
	{
		scene->Draw();
	}

	// カメラのデバッグ描画
	camera_->DrawDebug();
}

void SceneManager::ReleaseSceneResources(void)
{
	// SE・BGM解放
	SoundManager::GetInstance().Release();

	// リソース解放
	ResourceManager::GetInstance().Release();

	// 当たり判定情報解放
	CollisionManager::GetInstance().Release();

	if (camera_ != nullptr)
	{
		camera_->Init();
	}

	// 解放で消えたロード画面用のフォントを再読み込みする
	if (load_ != nullptr)
	{
		load_->Load();
	}
}
