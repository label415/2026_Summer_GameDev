#pragma once
#include <map>
#include <memory>
#include <string>
#include "../../Template/Singleton.h"
#include "../../Template/Vector2Template.h"
#include "../../Resource/ResourceBase.h"

class ResourceManager : public Singleton<ResourceManager>
{
	friend class Singleton<ResourceManager>;
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
		ANIM_PLAYER_ATTACK_1,
		ANIM_PLAYER_ATTACK_2,
		ANIM_PLAYER_ATTACK_3,
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
		SE_PLAYER_WALK,
		SE_PLAYER_RUN,
		SE_PLAYER_RECOVERY,
		SE_PLAYER_EVASION,
		SE_PLAYER_DAMAGE,

		// エネミーSE
		SE_ENEMY_ROAR,
		SE_ENEMY_WALK,
		SE_ENEMY_ATTACK,
		SE_ENEMY_BREATH_1,
		SE_ENEMY_BREATH_2,
		SE_ENEMY_FALL,
		SE_ENEMY_FLAP,
		SE_ENEMY_AIR_BREATH_1,
		SE_ENEMY_AIR_BREATH_2,
		SE_ENEMY_HIT_DAMAGE,

		// フォント
		FONT,

		// エフェクト
		EFF_BLOOD,
		EFF_HP_ABSOLUTE,
		EFF_FALLING_ATTACK,
		EFF_ROAR,
		EFF_CHARGE,
		EFF_BREATH,
		EFF_FIREBALL_BULLET,
		EFF_FIREBALL_EXPLOSION,
	};

	// 初期化
	void Init(void);

	// リソース登録
	void Add(SRC src, ResourceBase::TYPE type, const std::wstring& path);
	void Add(SRC src, const std::wstring& path, const Vector2& num, const Vector2& size);

	// ハンドル削除(シーン切替時)
	void Reset(void);

	// 配列解放(完全解放時)
	void Release(void);

	// リソースのロード
	const ResourceBase& Load(SRC src);

	// リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

	// フォントのロード（サイズ・太さ指定）
	int LoadFont(SRC src, int size, int thickness);

private:

	// リソース管理の対象
	std::map<SRC, std::unique_ptr<ResourceBase>> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, ResourceBase*> loadedMap_;

	// 未登録時に返すダミーリソース
	std::unique_ptr<ResourceBase> dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);
	// コピーコンストラクタ
	ResourceManager(const ResourceManager& manager) = delete;
	// デストラクタ
	~ResourceManager(void) override = default;

	// 内部ロード
	ResourceBase& _Load(SRC src);

};
