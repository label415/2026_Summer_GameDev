#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SceneManager.h"
#include "Manager/FontManager.h"
#include "Manager/CollisionManager.h"
#include "Common/FpsController.h"
#include "Libs/ImGuiWrapper.h"
#include "Application.h"

// ファイルパス
const std::wstring Application::PATH_IMAGE = L"Data/Image/";
const std::wstring Application::PATH_MODEL = L"Data/Model/";
const std::wstring Application::PATH_EFFECT = L"Data/Effect/";
const std::wstring Application::PATH_FONT = L"Data/Font/";
const std::wstring Application::PATH_CSV = L"Data/Csv/";
const std::wstring Application::PATH_SOUND = L"Data/Sound/";

bool Application::Init(void)
{
	// アプリケーションの初期設定
	SetWindowText(L"HOT SOULS");

	// ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, COLOR_BIT);
	ChangeWindowMode(true);

	// FPS制御初期化
	fpsController_ = std::make_unique<FpsController>(FRAME_RATE);

	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	if (DxLib_Init() == -1)
	{
		return false;
	}

	// Effekseerの初期化
	InitEffekseer();

	// 乱数のシード値を設定する
	DATEDATA date;

	// 現在時刻を取得する
	GetDateTime(&date);

	// 乱数の初期値を設定する
	// 設定する数値によって、ランダムの出方が変わる
	SRand(date.Year + date.Mon + date.Day + date.Hour + date.Min + date.Sec);

	// 入力制御初期化
	InputManager::CreateInstance();

	// リソース管理初期化
	ResourceManager::CreateInstance();

	// フォント管理クラス生成
	FontManager::CreateInstance();

	// サウンド管理初期化
	SoundManager::CreateInstance();

	// デバッグ描画初期化
	ImGuiWrapper::CreateInstance();

	// コリジョンマネージャーインスタンス生成
	CollisionManager::CreateInstance();

	// シーン管理初期化
	SceneManager::CreateInstance();
	SceneManager::GetInstance().Init();

	return true;
}

void Application::Run(void)
{
	//インスタンス取得
	InputManager& inputManager = InputManager::GetInstance();
	ImGuiWrapper& imGuiWrapper = ImGuiWrapper::GetInstance();
	CollisionManager& collisionManager = CollisionManager::GetInstance();
	SceneManager& sceneManager = SceneManager::GetInstance();

	// ゲームループ
	while (ProcessMessage() == 0 && !isGameEnd_)
	{
		// 入力更新処理
		inputManager.Update();

		//GUI更新処理
		imGuiWrapper.Update();

		collisionManager.Update();

		// シーン更新処理
		sceneManager.Update();

		collisionManager.DrawDebug();

		// シーン描画処理
		sceneManager.Draw();

		RenderVertex();

		// GUI描画処理
		imGuiWrapper.Draw();

		ScreenFlip();

		// 理想FPS経過待ち
		fpsController_->Wait();
	}
}

bool Application::Release(void)
{
	//インスタンス破棄
	InputManager::GetInstance().Destroy();
	CollisionManager::GetInstance().Release();
	CollisionManager::GetInstance().Destroy();
	SceneManager::GetInstance().Release();
	SceneManager::GetInstance().Destroy();
	SoundManager::GetInstance().Destroy();
	ResourceManager::GetInstance().Destroy();
	FontManager::GetInstance().Destroy();
	ImGuiWrapper::GetInstance().Destroy();

	// Effekseerを終了する。
	Effkseer_End();

	// DxLib終了
	if (DxLib_End() == -1)
	{
		return false;
	}

	return true;
}

Application::Application(void)
	:
	isGameEnd_(false)
{
}

void Application::InitEffekseer(void)
{
	// Effekseerの初期化
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}

void Application::SetIsEnd(void)
{
	//ゲーム終了
	isGameEnd_ = true;
}

