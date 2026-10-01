#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	// 静的インスタンスが生成されていなければ生成する
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{
	// リソースの初期化
	using RES = Resource;
	using RES_T = RES::TYPE;
	static std::wstring PATH_IMG = Application::PATH_IMAGE;
	static std::wstring PATH_MDL = Application::PATH_MODEL;
	static std::wstring PATH_FONT = Application::PATH_FONT;
	static std::wstring PATH_EFF = Application::PATH_EFFECT;
	static std::wstring PATH_SND = Application::PATH_SOUND;

	// リソースの登録
	Resource* res;

	// プレイヤーモデル
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player.mv1");
	resourcesMap_.emplace(SRC::MODEL_PLAYER, res);
	// プレイヤー武器
	res = new RES(RES_T::MODEL, PATH_MDL + L"Wepon/Wepon.mv1");
	resourcesMap_.emplace(SRC::MODEL_WEAPON_BLADE, res);
	// プレイヤーアニメーション
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Idle.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_IDLE, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Run.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_RUN, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Attack_1.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLSYER_ATTACK_1, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Attack_2.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLSYER_ATTACK_2, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Attack_3.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLSYER_ATTACK_3, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Running.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_EVASION, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Down.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_DOWN, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Up.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_UP, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Drinking.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_RECOVERY, res);

	// ステージモデル
	res = new RES(RES_T::MODEL, PATH_MDL + L"Stage/Stage.mv1");
	resourcesMap_.emplace(SRC::MODEL_MAIN_STAGE, res);
	// スカイドーム
	res = new RES(RES_T::MODEL, PATH_MDL + L"SkyDome/Skydome.mv1");
	resourcesMap_.emplace(SRC::MODEL_SKY_DOME, res);

	// エネミードラゴンモデル
	res = new RES(RES_T::MODEL, PATH_MDL + L"Enemy/Dragon/Dragon.mv1");
	resourcesMap_.emplace(SRC::MODEL_ENEMY_DRAGON, res);

	// タイトル画像
	res = new RES(RES_T::IMG, PATH_IMG + L"TitleImg.png");
	resourcesMap_.emplace(SRC::IMG_TITLE, res);
	// ゲームクリア画像
	res = new RES(RES_T::IMG, PATH_IMG + L"GameClearImg.png");
	resourcesMap_.emplace(SRC::IMG_GAMECLEAR, res);
	// ゲームオーバー画像
	res = new RES(RES_T::IMG, PATH_IMG + L"GameOverImg.png");
	resourcesMap_.emplace(SRC::IMG_GAMEOVER, res);

	// ロックオンUI
	res = new RES(RES_T::IMG, PATH_IMG + L"LockOnImg.png");
	resourcesMap_.emplace(SRC::UI_LOCKON, res);
	// 選択カーソルUI
	res = new RES(RES_T::IMG, PATH_IMG + L"SelectionCursor.png");
	resourcesMap_.emplace(SRC::UI_SELECTION_CURSOR, res);
	// バーの枠UI
	res = new RES(RES_T::IMG, PATH_IMG + L"BarFrame.png");
	resourcesMap_.emplace(SRC::UI_BAR_FRAME, res);
	// HPバーUI
	res = new RES(RES_T::IMG, PATH_IMG + L"HPBar.png");
	resourcesMap_.emplace(SRC::UI_HP_BAR, res);
	// STバーUI
	res = new RES(RES_T::IMG, PATH_IMG + L"STBar.png");
	resourcesMap_.emplace(SRC::UI_ST_BAR, res);
	// アイテムボックスUI
	res = new RES(RES_T::IMG, PATH_IMG + L"ItemBox.png");
	resourcesMap_.emplace(SRC::UI_ITEMBOX, res);
	// 回復瓶UI
	res = new RES(RES_T::IMG, PATH_IMG + L"RecoveryBottle.png");
	resourcesMap_.emplace(SRC::UI_RECOVERY_BOTTLE, res);

	// フォント
	res = new RES(RES_T::FONT, PATH_FONT + L"KazukiReiwa.ttf");
	resourcesMap_.emplace(SRC::FONT, res);

	// BGM
	res = new RES(RES_T::SOUND, PATH_SND + L"BGM/TitleBgm.mp3");
	resourcesMap_.emplace(SRC::BGM_TITLE, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"BGM/GameBgm.mp3");
	resourcesMap_.emplace(SRC::BGM_GAME, res);

	// プレイヤーSE
	res = new RES(RES_T::SOUND, PATH_SND + L"PlayerSE/PlayerWeaponHitSE1.mp3");
	resourcesMap_.emplace(SRC::SE_PLAYER_WEAPON_1, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"PlayerSE/PlayerDamageSE.mp3");
	resourcesMap_.emplace(SRC::SE_PLAYER_DAMAGE, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"PlayerSE/PlayerRecoverySE.mp3");
	resourcesMap_.emplace(SRC::SE_PLAYER_RECOVERY, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"PlayerSE/PlayerEvasionSE.mp3");
	resourcesMap_.emplace(SRC::SE_PLAYER_EVASION, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"PlayerSE/PlayerWakeSE.mp3");
	resourcesMap_.emplace(SRC::SE_PLAYER_WAKE, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"PlayerSE/PlayerRunSE.mp3");
	resourcesMap_.emplace(SRC::SE_PLAYER_RUN, res);

	// エネミーSE
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/DragonRoarSE.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_ROAR, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/DragonBreathSE1.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_BREASE_1, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/DragonBreathSE2.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_BREASE_2, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/DragonFlapSE.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_FLAP, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/DragonAttackSE.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_ATTCEK, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/DragonWakeSE.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_WAKE, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/DragonFliyerAttSE1.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_ARE_BREASE_1, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/DragonFliyerAttSE2.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_ARE_BREASE_2, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/DragonFallSE.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_FALL, res);
	res = new RES(RES_T::SOUND, PATH_SND + L"EnemySE/EnemyHItDamageSE.mp3");
	resourcesMap_.emplace(SRC::SE_ENEMY_HIT_DAMAGE, res);
}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	for (auto& res : resourcesMap_)
	{
		res.second->Release();
		delete res.second;
	}
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	const Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void){}

Resource& ResourceManager::_Load(SRC src)
{
	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *resourcesMap_.find(src)->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;
}
