#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
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

	// 推奨しませんが、どうしても使いたい方は
	using RES = Resource;
	using RES_T = RES::TYPE;
	static std::wstring PATH_IMG = Application::PATH_IMAGE;
	static std::wstring PATH_MDL = Application::PATH_MODEL;
	static std::wstring PATH_FONT = Application::PATH_FONT;
	static std::wstring PATH_EFF = Application::PATH_EFFECT;
	static std::wstring PATH_SND = Application::PATH_SOUND;

	Resource* res;

	// ロード画像
	res = new RES(RES_T::IMG, PATH_IMG + L"Loading.png");
	resourcesMap_.emplace(SRC::LOADING, res);

	//タイトルモデル
	res = new RES(RES_T::MODEL, PATH_MDL +
		L"Stage/PitfallPlanet/PitfallPlanet.mv1");
	resourcesMap_.emplace(SRC::PIT_FALL_PLANET, res);
	res = new RES(RES_T::MODEL, PATH_MDL +
		L"Stage/SpherePlanet/SpherePlanet.mv1");
	resourcesMap_.emplace(SRC::SPHERE_PLANET, res);

	//プレイヤー
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player.mv1");
	resourcesMap_.emplace(SRC::PLAYER, res);
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
	resourcesMap_.emplace(SRC::ANIM_PLAYER_AVOIDANCE, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Down.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_DOWN, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Up.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_UP, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player_Recovery.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_RECOVERY, res);

	//武器
	res = new RES(RES_T::MODEL, PATH_MDL + L"Wepon/Wepon.mv1");
	resourcesMap_.emplace(SRC::WEAPON_BLADE, res);

	//ゲームシーン
	res = new RES(RES_T::MODEL, PATH_MDL + L"Stage/Stage.mv1");
	resourcesMap_.emplace(SRC::MAIN_STAGE, res);

	//スカイドーム
	res = new RES(RES_T::MODEL, PATH_MDL + L"SkyDome/Skydome.mv1");
	resourcesMap_.emplace(SRC::SKY_DOME, res);

	// プレイヤー影
	res = new RES(RES_T::IMG, PATH_IMG + L"Shadow.png");
	resourcesMap_.emplace(SRC::PLAYER_SHADOW, res);

	//エネミーラット
	res = new RES(RES_T::MODEL, PATH_MDL + L"Enemy/Rat/Rat.mv1");
	resourcesMap_.emplace(SRC::ENEMY_RAT, res);

	//エネミーロボット
	res = new RES(RES_T::MODEL, PATH_MDL + L"Enemy/Robot/Robot.mv1");
	resourcesMap_.emplace(SRC::ENEMY_ROBOT, res);

	//エネミードラゴン
	res = new RES(RES_T::MODEL, PATH_MDL + L"Enemy/Dragon/Dragon.mv1");
	resourcesMap_.emplace(SRC::ENEMY_DRAGON, res);

	//エネミーロボット円錐モデル
	res = new RES(RES_T::MODEL, PATH_MDL + L"Enemy/Robot/Cone.mv1");
	resourcesMap_.emplace(SRC::VIEW_RANGE, res);

	//エネミーロボット円錐モデル
	res = new RES(RES_T::IMG, PATH_IMG + L"Surprise.png");
	resourcesMap_.emplace(SRC::UI_SURPRISE, res);

	res = new RES(RES_T::IMG, PATH_IMG + L"GameClear.png");
	resourcesMap_.emplace(SRC::GAME_CLEAR, res);

	res = new RES(RES_T::IMG, PATH_IMG + L"GameOver.png");
	resourcesMap_.emplace(SRC::GAME_OVER, res);


	res = new RES(RES_T::IMG, PATH_IMG + L"LockOnImg.png");
	resourcesMap_.emplace(SRC::LOCKON_IMG, res);

	//フォント
	res = new RES(RES_T::FONT, PATH_FONT + L"KazukiReiwa.ttf");
	resourcesMap_.emplace(SRC::FONT, res);

	res = new RES(RES_T::IMG, PATH_IMG + L"TitleSelect.png");
	resourcesMap_.emplace(SRC::TITLE_SELECT, res);

	res = new RES(RES_T::IMG, PATH_IMG + L"Hp1.png");
	resourcesMap_.emplace(SRC::HP_1, res);

	res = new RES(RES_T::IMG, PATH_IMG + L"Hp2.png");
	resourcesMap_.emplace(SRC::HP_2, res);

	res = new RES(RES_T::IMG, PATH_IMG + L"ST.png");
	resourcesMap_.emplace(SRC::ST, res);

	res = new RES(RES_T::IMG, PATH_IMG + L"UIBox.png");
	resourcesMap_.emplace(SRC::UI_BOX, res);
	res = new RES(RES_T::IMG, PATH_IMG + L"RecoveryBottle.png");
	resourcesMap_.emplace(SRC::UI_RECOVERY_BOTTLE, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"TitleBgm.mp3");
	resourcesMap_.emplace(SRC::TITLE_BGM, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"GameBgm.mp3");
	resourcesMap_.emplace(SRC::GAME_BGM, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"Player_Wepon_hit1.mp3");
	resourcesMap_.emplace(SRC::PLAYER_WEPON_SE1, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"1.mp3");
	resourcesMap_.emplace(SRC::PLAYER_WEPON_SE2, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"Dame.mp3");
	resourcesMap_.emplace(SRC::PLAYER_DMAGE, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"REVE.mp3");
	resourcesMap_.emplace(SRC::PLAYER_HER, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"PlayerAve.mp3");
	resourcesMap_.emplace(SRC::PLAYER_AVE, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"PlayerWake.mp3");
	resourcesMap_.emplace(SRC::PLAYER_WAKE, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"PlayerRan.mp3");
	resourcesMap_.emplace(SRC::PLAYER_RAN, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"DragomRoar.mp3");
	resourcesMap_.emplace(SRC::ENEMY_ROAR, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"DragomBreath1.mp3");
	resourcesMap_.emplace(SRC::ENEMY_BREASE1, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"DragomBreath2.mp3");
	resourcesMap_.emplace(SRC::ENEMY_BREASE2, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"DragomBreath1.mp3");
	resourcesMap_.emplace(SRC::ENEMY_BREASE1, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"DragomBreath2.mp3");
	resourcesMap_.emplace(SRC::ENEMY_BREASE2, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"DragoAtr.mp3");
	resourcesMap_.emplace(SRC::ENEMY_ARE, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"DragomAttack.mp3");
	resourcesMap_.emplace(SRC::ENEMY_ATTCEK, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"DragonWake.mp3");
	resourcesMap_.emplace(SRC::ENEMY_WAKE, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"EnemyFliyer_Att1.mp3");
	resourcesMap_.emplace(SRC::ENEMY_ARE_ENEMY_BREASE1, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"EnemyFliyer_Att2.mp3");
	resourcesMap_.emplace(SRC::ENEMY_ARE_ENEMY_BREASE2, res);

	res = new RES(RES_T::SOUND, PATH_SND + L"DragomFall.mp3");
	resourcesMap_.emplace(SRC::ENEMY_FALL , res);

	res = new RES(RES_T::IMG, PATH_IMG + L"TitleImg.png");
	resourcesMap_.emplace(SRC::TITLE_IMG, res);
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

ResourceManager::ResourceManager(void)
{
}

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
