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
	// 実行中のシーンのUpdate内から呼ばれても自身を破棄しないよう、
	// ここでは要求だけ受け付けて、Updateの最後に遷移を実行する
	nextSceneId_ = sceneId;
	isChangeRequest_ = true;
}

void SceneManager::PushScene(SCENE_ID sceneId)
{
	// ロード中は受け付けない
	if (IsLoading()) return;

	scenes_.push_back(CreateScene(sceneId));
	sceneIds_.push_back(sceneId);

	scenes_.back()->Load();
	scenes_.back()->LoadEnd();

	// カメラのマウス入力を無効化
	camera_->SetIsMouseInput(false);
	// 入力管理のマウスフラグを有効化
	InputManager::GetInstance().SetMouseFlage(true);
}

void SceneManager::PopScene(void)
{
	// 実行中のシーンを破棄しないよう、要求だけ受け付ける
	isPopRequest_ = true;
}

void SceneManager::ResetScene(SCENE_ID sceneId)
{
	// 全シーンを解放して遷移するのはChangeSceneと同じ
	ChangeScene(sceneId);
}

bool SceneManager::IsLoading(void) const
{
	return isSceneLoadPending_ || load_->IsLoading();
}

void SceneManager::Update(void)
{
	// ロード中はロード処理のみ行う
	if (IsLoading())
	{
		UpdateLoading();
		return;
	}

	if (!scenes_.empty())
	{
		scenes_.back()->Update();
	}

	camera_->Update();

	// シーンの処理が終わってから遷移要求を処理する
	if (isChangeRequest_)
	{
		isChangeRequest_ = false;
		isPopRequest_ = false;
		DoChangeScene(nextSceneId_);
	}
	else if (isPopRequest_)
	{
		isPopRequest_ = false;
		DoPopScene();
	}
}

void SceneManager::Draw(void)
{
	// 描画先グラフィック領域を裏画面に指定
	//（カメラ設定などもリセットされる）
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化する
	ClearDrawScreen();

	// ロード中はロード画面のみ描画する
	if (IsLoading())
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

	// ロード画面の削除
	load_->Release();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	// 積まれている全シーンを解放する
	// (ポーズ中にタイトルへ戻った時などに、下のゲームシーンが残らないように)
	ReleaseAllScenes();

	// SE・BGM解放
	SoundManager::GetInstance().Release();

	// リソース解放
	ResourceManager::GetInstance().Release();

	// 当たり判定情報解放
	CollisionManager::GetInstance().Release();

	// 前のシーンのオブジェクトを指しているカメラの情報をリセット
	camera_->ResetForScene();

	// 入力管理のマウスフラグを有効化
	InputManager::GetInstance().SetMouseFlage(true);

	// ロード画面の表示開始
	nextSceneId_ = sceneId;
	sceneId_ = sceneId;
	load_->StartLoading();
	isSceneLoadPending_ = true;
}

void SceneManager::DoPopScene(void)
{
	// 最後の1シーンは残す
	if (scenes_.size() <= 1) return;

	// 末尾のシーンを解放して削除する
	scenes_.back()->Release();
	scenes_.pop_back();
	sceneIds_.pop_back();

	// 現在のシーンIDを戻す
	sceneId_ = sceneIds_.back();

	// ゲームシーンに戻った時はカメラ操作を再開する
	if (sceneId_ == SCENE_ID::GAME)
	{
		camera_->SetIsMouseInput(true);
		InputManager::GetInstance().SetMouseFlage(false);
	}
}

void SceneManager::UpdateLoading(void)
{
	// ロード画面を表示した次のフレームでシーンを生成・読み込みする
	if (isSceneLoadPending_)
	{
		isSceneLoadPending_ = false;

		scenes_.push_back(CreateScene(nextSceneId_));
		sceneIds_.push_back(nextSceneId_);

		// 非同期読み込みでシーンのリソースを読み込む
		load_->StartAsyncLoad();
		scenes_.back()->Load();
		load_->EndAsyncLoad();
		return;
	}

	// ロード画面の更新
	load_->Update();

	// ロードが完了したら、読み込み後の初期化を行う
	if (!load_->IsLoading())
	{
		scenes_.back()->LoadEnd();
	}
}

void SceneManager::ReleaseAllScenes(void)
{
	// 上に積まれているシーンから順に解放する
	for (auto it = scenes_.rbegin(); it != scenes_.rend(); ++it)
	{
		if (*it)
		{
			(*it)->Release();
		}
	}
	scenes_.clear();
	sceneIds_.clear();
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
	, isChangeRequest_(false)
	, isPopRequest_(false)
	, isSceneLoadPending_(false)
	, deltaTime_(0.0f)
{
}
