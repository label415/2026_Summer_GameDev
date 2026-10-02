#pragma once
#include <chrono>
#include <vector>
#include <memory>
#include <DxLib.h>

class SceneBase;
class Fader;
class Camera;
class Loading;

class SceneManager
{
public:
	// シーン名
	enum class SCENE_ID
	{
		TITLE,
		GAME,
		PAUSE,
	};

	// 背景色
	static constexpr int BACKGROUND_COLOR_R = 0;
	static constexpr int BACKGROUND_COLOR_G = 0;
	static constexpr int BACKGROUND_COLOR_B = 0;

	// ディレクショナルライトの方向
	static constexpr VECTOR LIGHT_DIRECTION = { 1.0f, -1.0f, 1.0f };
	
	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static SceneManager& GetInstance(void);

	// 初期化
	void Init(void);
	
	// 3Dの初期化
	void Init3D(void);

	void ChangeScene(SCENE_ID sceneId);
	void PushScene(SCENE_ID sceneId);
	void PopScene();
	void ResetScene(SCENE_ID sceneIde);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// リソースの破棄
	void Destroy(void);

	// デルタタイムの取得
	float GetDeltaTime(void) const { return 1.0f / 60.0f; }

	// カメラの取得
	Camera* GetCamera(void) const { return camera_; }

	//現在のシーンID
	const SCENE_ID GetSceneID(void)const { return sceneId_; }
private:
	// 静的インスタンス
	static SceneManager* instance_;

	// 現在のシーン
	SCENE_ID sceneId_;

	// 各種シーン
	std::vector<std::unique_ptr<SceneBase>> scenes_;

	// カメラ
	Camera* camera_;

	// ロード画面
	Loading* load_;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;
	
	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void) = default;

	// コピーコンストラクタも同様
	SceneManager(const SceneManager& instance) = default;

	// デストラクタも同様
	~SceneManager(void) = default;

	// 特定のシーンインスタンスを生成する
	std::unique_ptr<SceneBase> CreateScene(SCENE_ID sceneId);
};