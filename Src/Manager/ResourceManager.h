#pragma once
#include <map>
#include <string>
#include "Resource.h"

class ResourceManager
{
public:
	// リソース名
	enum class SRC
	{
		// タイトル画像
		IMG_TITLE,
		// ゲームクリア画像
		IMG_GAMECLEAR,
		// ゲームオーバー画像
		IMG_GAMEOVER,

		//プレイヤー
		MODEL_PLAYER,
		MODEL_WEAPON_BLADE,
		ANIM_PLAYER_IDLE,
		ANIM_PLAYER_RUN,
		ANIM_PLSYER_ATTACK_1,
		ANIM_PLSYER_ATTACK_2,
		ANIM_PLSYER_ATTACK_3,
		ANIM_PLAYER_EVASION,
		ANIM_PLAYER_DOWN,
		ANIM_PLAYER_UP,
		ANIM_PLAYER_RECOVERY,

		// ステージモデル
		MODEL_MAIN_STAGE,
		// スカイドーム
		MODEL_SKY_DOME,

		// エネミードラゴンモデル
		MODEL_ENEMY_DRAGON,

		// ロックオンUI
		UI_LOCKON,
		// 選択カーソルUI
		UI_SELECTION_CURSOR,
		// バーの枠UI
		UI_BAR_FRAME,
		// HPバーUI
		UI_HP_BAR,
		// STバーUI
		UI_ST_BAR,
		// アイテムボックスUI
		UI_ITEMBOX,
		// 回復瓶UI
		UI_RECOVERY_BOTTLE,

		// BGM
		BGM_TITLE,
		BGM_GAME,

		// プレイヤーSE
		SE_PLAYER_WEAPON_1,
		SE_PLAYER_WAKE,
		SE_PLAYER_RUN,
		SE_PLAYER_RECOVERY,
		SE_PLAYER_EVASION,
		SE_PLAYER_DAMAGE,

		// エネミーSE
		SE_ENEMY_ROAR,
		SE_ENEMY_WAKE,
		SE_ENEMY_ATTCEK,
		SE_ENEMY_BREASE_1,
		SE_ENEMY_BREASE_2,
		SE_ENEMY_FALL,
		SE_ENEMY_FLAP,
		SE_ENEMY_ARE_BREASE_1,
		SE_ENEMY_ARE_BREASE_2,
		SE_ENEMY_HIT_DAMAGE,

		// フォント
		FONT,
	};

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);
private:
	// 静的インスタンス
	static ResourceManager* instance_;

	// リソース管理の対象
	std::map<SRC, Resource*> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	// リソース
	Resource dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	// 内部ロード
	Resource& _Load(SRC src);

};
