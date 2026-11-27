#pragma once
#include <string>
#include <memory>
#include "Template/Singleton.h"
#include "Template/Vector2Template.h"

class FpsController;

class Application : public Singleton<Application>
{

	friend class Singleton<Application>;

public:

	// スクリーンサイズ
	static constexpr Vector2 SCREEN_SIZE = {1280, 720};

	// ハーフスクリーンサイズ
	static constexpr Vector2 HALF_SCREEN_SIZE = { SCREEN_SIZE.x / 2, SCREEN_SIZE.y / 2 };

	// カラービット数
	static constexpr int COLOR_BIT = 32;

	// FPSレート
	static constexpr int FRAME_RATE = 60;

	// データパス関連
	//-------------------------------------------
	static const std::wstring PATH_DATA;
	static const std::wstring PATH_IMAGE;
	static const std::wstring PATH_MODEL;
	static const std::wstring PATH_EFFECT;
	static const std::wstring PATH_FONT;
	static const std::wstring PATH_CSV;
	static const std::wstring PATH_SOUND;

	static const std::wstring PATH_KEY_CONFIG;
	static const std::wstring PATH_KEY_CONFIG_GAMEPAD;
	static const std::wstring PATH_KEY_CONFIG_KEYBOARD;
	//-------------------------------------------

	// 重力
	static constexpr float GRAVITY = 9.81f * 100.0f;
	static constexpr float GRAVITY_SCALE = 0.7f;

	// 初期化
	bool Init(void);

	// ゲームループの開始
	void Run(void);

	// リソースの破棄
	bool Release(void);

	// 重力の取得
	float GetGravityPow(void) const { return GRAVITY * GRAVITY_SCALE; }

	// ゲーム終了フラグ設定
	void SetIsEnd(void);

private:

	// FPSコントローラー
	std::unique_ptr<FpsController> fpsController_;

	// ゲーム終了フラグ
	bool isGameEnd_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	Application(void);

	// コピーコンストラクタも同様
	Application(const Application& instance) = default;

	// デストラクタも同様
	~Application(void) override = default;

};