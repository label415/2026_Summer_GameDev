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

	// シーン遷移(要求のみ受け付け、Updateの最後に実行する)
	void ChangeScene(SCENE_ID sceneId);

	// シーン追加
	void PushScene(SCENE_ID sceneId);

	// シーン解放(要求のみ受け付け、Updateの最後に実行する)
	void PopScene(void);

	// デルタタイムの取得
	float GetDeltaTime(void) const { return 1.0f / 60.0f; }

	// カメラの取得
	std::shared_ptr<Camera> GetCamera(void) const { return camera_; }

	//現在のシーンID
	const SCENE_ID GetSceneID(void)const { return sceneId_; }
private:
	// ロード状態
	enum class LOAD_STATE
	{
		NONE,		// ロードしていない(通常更新)
		START,		// ロード画面を表示した直後(次のフレームで読み込み開始)
		LOADING,	// 非同期読み込み中
	};

	// 現在のロード状態
	LOAD_STATE loadState_;

	// 現在のシーンID
	SCENE_ID sceneId_;

	// 次に遷移するシーンID
	SCENE_ID nextSceneId_;

	// ロード画面を表示した次のフレームで読み込みを開始するフラグ
	bool isLoadStart_;

	// シーン遷移要求フラグ
	bool isChangeRequest_;

	// 各種シーン
	std::vector<std::unique_ptr<SceneBase>> scenes_;

	// 各種シーンのID(scenes_と同じ並び)
	std::vector<SCENE_ID> sceneIds_;

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

	// 指定されたシーンIDのインスタンスを生成する
	std::unique_ptr<SceneBase> CreateScene(SCENE_ID sceneId);

	// シーン遷移の実行
	void DoChangeScene(void);

	// 積まれている全シーンの解放
	void ReleaseAllScenes(void);

	// リソース解放
	void ResetResource(void);

	// ロード中の更新
	void UpdateLoading(void);

	// ロード中か
	bool IsLoading(void) const { return loadState_ != LOAD_STATE::NONE; }
};