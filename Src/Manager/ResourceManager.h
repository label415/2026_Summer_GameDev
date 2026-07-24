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
		TITLE_IMG,

		LOADING,
		PIT_FALL_PLANET,
		SPHERE_PLANET,

		//プレイヤー
		PLAYER,
		ANIM_PLAYER_IDLE,
		ANIM_PLAYER_RUN,
		ANIM_PLSYER_ATTACK_1,
		ANIM_PLSYER_ATTACK_2,
		ANIM_PLSYER_ATTACK_3,
		ANIM_PLAYER_AVOIDANCE,
		ANIM_PLAYER_DOWN,
		ANIM_PLAYER_UP,
		ANIM_PLAYER_RECOVERY,

		//ステージ
		MAIN_STAGE,
		STAGE_LOCK,

		SKY_DOME,
		PLAYER_SHADOW,
		ENEMY_RAT,
		ENEMY_ROBOT,
		ENEMY_DRAGON,
		VIEW_RANGE,
		UI_SURPRISE,
		WEAPON_BLADE,
		FONT,
		L_FONT,

		LOCKON_IMG,

		TITLE_SELECT,
		HP_1,
		HP_2,
		ST,

		UI_BOX,
		UI_RECOVERY_BOTTLE,
		TITLE_BGM,
		GAME_BGM,

		PLAYER_WEPON_SE1,
		PLAYER_WEPON_SE2,
		PLAYER_WAKE,
		PLAYER_RAN,
		PLAYER_HER,
		PLAYER_AVE,
		PLAYER_DMAGE,

		ENEMY_ROAR,
		ENEMY_WAKE,
		ENEMY_ATTCEK,
		ENEMY_BREASE1,
		ENEMY_BREASE2,
		ENEMY_FALL,
		ENEMY_ARE,
		ENEMY_ARE_ENEMY_BREASE1,
		ENEMY_ARE_ENEMY_BREASE2,

		GAME_CLEAR,
		GAME_OVER,

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

	Resource dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	// 内部ロード
	Resource& _Load(SRC src);

};
