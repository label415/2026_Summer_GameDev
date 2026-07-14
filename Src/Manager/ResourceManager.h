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
		LOADING,
		PIT_FALL_PLANET,
		SPHERE_PLANET,

		//プレイヤー
		PLAYER,
		ANIM_PLAYER_IDLE,
		ANIM_PLAYER_RUN,
		ANIM_PLSYER_ATTACK,
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

		TITLE_SELECT,
		HP_1,
		HP_2,

		UI_BOX,
		UI_RECOVERY_BOTTLE,
		SE_1,
		SE_2,
		SE_3,
		SE_4,
		SE_5,
		SE_6,
		SE_7,

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
