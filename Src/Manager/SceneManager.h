#pragma once
#include <chrono>
#include <vector>
#include <memory>
#include <DxLib.h>
#include "../Template/Singleton.h"
#include "../Template/Vector2Template.h"

class SceneBase;
class Fader;
class Camera;
class Loading;

class SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;
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
	static constexpr VECTOR LIGHT_DIRECTION = { -0.5f, -1.0f, -0.5f };
	// ライトのディフューズカラー値
	static constexpr COLOR_F LIGHT_DIFF_COLOR = { 1.0f, 1.0f, 0.95f, 1.0f };
	// ライトのアンビエントカラー値
	static constexpr COLOR_F LIGHT_AMB_COLOR = { 0.4f, 0.4f, 0.45f, 1.0f };

	// フォグカラー値
	static constexpr int FOG_COLOR_R = 5;
	static constexpr int FOG_COLOR_G = 5;
	static constexpr int FOG_COLOR_B = 5;
	// フォグ表示範囲
	static constexpr float FOG_START = 10000.0f;
	static constexpr float FOG_END = 20000.0f;

	// 初期化
	void Init(void);

	// 3Dの初期化
	void Init3D(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// リソースの破棄
	void Release(void);

	// シーン遷移
	void ChangeScene(SCENE_ID sceneId);

	// シーン追加
	void PushScene(SCENE_ID sceneId);

	// シーン解放
	void PopScene(void);

	// デルタタイムの取得
	float GetDeltaTime(void) const { return 1.0f / 60.0f; }

	// カメラの取得
	std::shared_ptr<Camera> GetCamera(void) const { return camera_; }

	//現在のシーンID
	const SCENE_ID GetSceneID(void)const { return sceneId_; }
private:
	// シーン遷移とロードの進行状態
	enum class LOAD_STATE
	{
		NONE,
		WAIT_DRAW_LOADING,
		EXECUTE_LOAD,
		WAIT_ASYNC_LOAD,
	};

	// 現在のシーンID
	SCENE_ID sceneId_;

	// 次に遷移する予約シーンID
	SCENE_ID nextSceneId_;

	// 遷移種別
	bool isClearScenes_ = true;

	// 現在のロード進行状態
	LOAD_STATE loadState_ = LOAD_STATE::NONE;

	// 各種シーン
	std::vector<std::unique_ptr<SceneBase>> scenes_;

	// カメラ
	std::shared_ptr<Camera> camera_;

	// ロード画面
	std::unique_ptr<Loading> load_;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	// フレーム間の経過時間（デルタタイム）
	float deltaTime_;

	// デフォルトコンストラクタをprivateにして、外部から生成できない様にする
	SceneManager(void);

	// コピーコンストラクタ
	SceneManager(const SceneManager& instance) = default;

	// デストラクタ
	~SceneManager(void) = default;

	// 現在ロード処理中かどうかを判定する
	bool IsLoadingState(void) const;

	// 次のシーン遷移を予約し、ロード画面を開始する
	void ReserveSceneChange(SCENE_ID sceneId, bool isClearScenes);

	// ロード画面を挟まず即座にシーンを追加する（ポーズ画面用）
	void PushSceneImmediate(SCENE_ID sceneId);

	// カメラとマウス入力の状態をリセットする
	void ResetCameraAndInput(void);

	// ロード中の更新処理
	void UpdateLoading(void);

	// 非同期ロードを開始する
	void ExecuteSceneLoad(void);

	// 非同期ロード完了後の初期化
	void FinishSceneLoad(void);

	// 通常時更新処理
	void UpdateNormal(void);

	// 通常時描画処理
	void DrawScenes(void) const;

	// サウンド・リソース・当たり判定解放
	void ReleaseSceneResources(void);

	// 指定されたシーンIDのインスタンスを生成する
	std::unique_ptr<SceneBase> CreateScene(SCENE_ID sceneId);
};