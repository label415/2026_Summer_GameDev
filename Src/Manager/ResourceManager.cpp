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

	Resource* res;

	// タイトル画像
	res = new RES(RES_T::IMG, PATH_IMG + L"Title.png");
	resourcesMap_.emplace(SRC::TITLE, res);
	res = new RES(RES_T::IMG, PATH_IMG + L"PushSpace.png");
	resourcesMap_.emplace(SRC::PUSHSPACE, res);
	//タイトルモデル
	res = new RES(RES_T::MODEL, PATH_MDL +
		L"Stage/PitfallPlanet/PitfallPlanet.mv1");
	resourcesMap_.emplace(SRC::PIT_FALL_PLANET, res);
	res = new RES(RES_T::MODEL, PATH_MDL +
		L"Stage/SpherePlanet/SpherePlanet.mv1");
	resourcesMap_.emplace(SRC::SPHERE_PLANET, res);
	res = new RES(RES_T::MODEL, PATH_MDL + L"Player/Player.mv1");
	resourcesMap_.emplace(SRC::PLAYER, res);

	//ゲームシーン
	res = new RES(RES_T::MODEL, PATH_MDL + L"Stage/MainStage/MainStage.mv1");
	resourcesMap_.emplace(SRC::MAIN_STAGE, res);

	//スカイドーム
	res = new RES(RES_T::MODEL, PATH_MDL + L"SkyDome/SkyDome.mv1");
	resourcesMap_.emplace(SRC::SKY_DOME, res);

	// プレイヤー影
	res = new RES(RES_T::IMG, PATH_IMG + L"Shadow.png");
	resourcesMap_.emplace(SRC::PLAYER_SHADOW, res);

	// プレイヤー影
	res = new RES(RES_T::IMG, PATH_IMG + L"Shadow.png");
	resourcesMap_.emplace(SRC::PLAYER_SHADOW, res);

	//エネミーラット
	res = new RES(RES_T::MODEL, PATH_MDL + L"Enemy/Rat/Rat.mv1");
	resourcesMap_.emplace(SRC::ENEMY_RAT, res);

	//エネミーロボット
	res = new RES(RES_T::MODEL, PATH_MDL + L"Enemy/Robot/Robot.mv1");
	resourcesMap_.emplace(SRC::ENEMY_ROBOT, res);

	//エネミーロボット円錐モデル
	res = new RES(RES_T::MODEL, PATH_MDL + L"Enemy/Robot/Cone.mv1");
	resourcesMap_.emplace(SRC::VIEW_RANGE, res);

	//エネミーロボット円錐モデル
	res = new RES(RES_T::IMG, PATH_IMG + L"Surprise.png");
	resourcesMap_.emplace(SRC::UI_SURPRISE, res);

	//エネミーロボット
	res = new RES(RES_T::MODEL, PATH_MDL + L"Wepon/KeyBlade1.mv1");
	resourcesMap_.emplace(SRC::WEAPON_BLADE, res);


	//フォント
	res = new RES(RES_T::FONT, PATH_FONT + L"Hanazome.ttf");
	resourcesMap_.emplace(SRC::FONT, res);

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
