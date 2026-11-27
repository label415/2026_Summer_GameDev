#include <DxLib.h>
#include "../../Application.h"
#include "../../Resource/ResourceImage.h"
#include "../../Resource/ResourceImages.h"
#include "../../Resource/ResourceModel.h"
#include "../../Resource/ResourceFont.h"
#include "../../Resource/ResourceEffect.h"
#include "../../Resource/ResourceSound.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager(void)
	: dummy_(nullptr)
{
}

void ResourceManager::Init(void)
{
	// 型のエイリアス
	using TYPE = ResourceBase::TYPE;

	// パスのエイリアス
	static const std::wstring PATH_IMG = Application::PATH_IMAGE;
	static const std::wstring PATH_MDL = Application::PATH_MODEL;
	static const std::wstring PATH_EFF = Application::PATH_EFFECT;
	static const std::wstring PATH_FONT = Application::PATH_FONT;
	static const std::wstring PATH_SND = Application::PATH_SOUND;

	// プレイヤーモデル
	Add(SRC::MODEL_PLAYER, TYPE::MODEL, PATH_MDL + L"Player/Player.mv1");
	// プレイヤー武器
	Add(SRC::MODEL_WEAPON_BLADE, TYPE::MODEL, PATH_MDL + L"Wepon/Wepon.mv1");
	// プレイヤーアニメーション
	Add(SRC::ANIM_PLAYER_IDLE, TYPE::MODEL, PATH_MDL + L"Player/Player_Idle.mv1");
	Add(SRC::ANIM_PLAYER_RUN, TYPE::MODEL, PATH_MDL + L"Player/Player_Run.mv1");
	Add(SRC::ANIM_PLAYER_ATTACK_1, TYPE::MODEL, PATH_MDL + L"Player/Player_Attack_1.mv1");
	Add(SRC::ANIM_PLAYER_ATTACK_2, TYPE::MODEL, PATH_MDL + L"Player/Player_Attack_2.mv1");
	Add(SRC::ANIM_PLAYER_ATTACK_3, TYPE::MODEL, PATH_MDL + L"Player/Player_Attack_3.mv1");
	Add(SRC::ANIM_PLAYER_EVASION, TYPE::MODEL, PATH_MDL + L"Player/Player_Running.mv1");
	Add(SRC::ANIM_PLAYER_DOWN, TYPE::MODEL, PATH_MDL + L"Player/Player_Down.mv1");
	Add(SRC::ANIM_PLAYER_UP, TYPE::MODEL, PATH_MDL + L"Player/Player_Up.mv1");
	Add(SRC::ANIM_PLAYER_RECOVERY, TYPE::MODEL, PATH_MDL + L"Player/Player_Drinking.mv1");

	// ステージモデル
	Add(SRC::MODEL_MAIN_STAGE, TYPE::MODEL, PATH_MDL + L"Stage/Stage.mv1");
	// スカイドーム
	Add(SRC::MODEL_SKY_DOME, TYPE::MODEL, PATH_MDL + L"SkyDome/Skydome.mv1");

	// エネミードラゴンモデル
	Add(SRC::MODEL_ENEMY_DRAGON, TYPE::MODEL, PATH_MDL + L"Enemy/Dragon/Dragon.mv1");

	// タイトル画像
	Add(SRC::IMG_TITLE, TYPE::IMG, PATH_IMG + L"TitleImg.png");
	// ゲームクリア画像
	Add(SRC::IMG_GAMECLEAR, TYPE::IMG, PATH_IMG + L"GameClearImg.png");
	// ゲームオーバー画像
	Add(SRC::IMG_GAMEOVER, TYPE::IMG, PATH_IMG + L"GameOverImg.png");

	// ロックオンUI
	Add(SRC::UI_LOCKON, TYPE::IMG, PATH_IMG + L"LockOnImg.png");
	// 選択カーソルUI
	Add(SRC::UI_SELECTION_CURSOR, TYPE::IMG, PATH_IMG + L"SelectionCursor.png");
	// バーの枠UI
	Add(SRC::UI_BAR_FRAME, TYPE::IMG, PATH_IMG + L"BarFrame.png");
	// HPバーUI
	Add(SRC::UI_HP_BAR, TYPE::IMG, PATH_IMG + L"HPBar.png");
	// STバーUI
	Add(SRC::UI_ST_BAR, TYPE::IMG, PATH_IMG + L"STBar.png");
	// アイテムボックスUI
	Add(SRC::UI_ITEMBOX, TYPE::IMG, PATH_IMG + L"ItemBox.png");
	// 回復瓶UI
	Add(SRC::UI_RECOVERY_BOTTLE, TYPE::IMG, PATH_IMG + L"RecoveryBottle.png");

	// フォント
	Add(SRC::FONT, TYPE::FONT, PATH_FONT + L"KazukiReiwa.ttf");

	// エフェクト
	Add(SRC::EFF_BLOOD, TYPE::EFFEKSEER, PATH_EFF + L"Blood.efkefc");
	Add(SRC::EFF_HP_ABSOLUTE, TYPE::EFFEKSEER, PATH_EFF + L"Absolute.efkefc");
	Add(SRC::EFF_FALLING_ATTACK, TYPE::EFFEKSEER, PATH_EFF + L"Fall.efkefc");
	Add(SRC::EFF_ROAR, TYPE::EFFEKSEER, PATH_EFF + L"RoarEff.efkefc");
	Add(SRC::EFF_CHARGE, TYPE::EFFEKSEER, PATH_EFF + L"Charge.efkefc");
	Add(SRC::EFF_BREATH, TYPE::EFFEKSEER, PATH_EFF + L"Breath.efkefc");
	Add(SRC::EFF_FIREBALL_BULLET, TYPE::EFFEKSEER, PATH_EFF + L"FireBall_Bullet.efkefc");
	Add(SRC::EFF_FIREBALL_EXPLOSION, TYPE::EFFEKSEER, PATH_EFF + L"FireBall_Explosion.efkefc");

	// BGM
	Add(SRC::BGM_TITLE, TYPE::SOUND, PATH_SND + L"BGM/TitleBgm.mp3");
	Add(SRC::BGM_GAME, TYPE::SOUND, PATH_SND + L"BGM/GameBgm.mp3");

	// プレイヤーSE
	Add(SRC::SE_PLAYER_WEAPON_1, TYPE::SOUND, PATH_SND + L"PlayerSE/PlayerWeaponHitSE1.mp3");
	Add(SRC::SE_PLAYER_DAMAGE, TYPE::SOUND, PATH_SND + L"PlayerSE/PlayerDamageSE.mp3");
	Add(SRC::SE_PLAYER_RECOVERY, TYPE::SOUND, PATH_SND + L"PlayerSE/PlayerRecoverySE.mp3");
	Add(SRC::SE_PLAYER_EVASION, TYPE::SOUND, PATH_SND + L"PlayerSE/PlayerEvasionSE.mp3");
	Add(SRC::SE_PLAYER_WALK, TYPE::SOUND, PATH_SND + L"PlayerSE/PlayerWakeSE.mp3");
	Add(SRC::SE_PLAYER_RUN, TYPE::SOUND, PATH_SND + L"PlayerSE/PlayerRunSE.mp3");

	// エネミーSE
	Add(SRC::SE_ENEMY_ROAR, TYPE::SOUND, PATH_SND + L"EnemySE/DragonRoarSE.mp3");
	Add(SRC::SE_ENEMY_BREATH_1, TYPE::SOUND, PATH_SND + L"EnemySE/DragonBreathSE1.mp3");
	Add(SRC::SE_ENEMY_BREATH_2, TYPE::SOUND, PATH_SND + L"EnemySE/DragonBreathSE2.mp3");
	Add(SRC::SE_ENEMY_FLAP, TYPE::SOUND, PATH_SND + L"EnemySE/DragonFlapSE.mp3");
	Add(SRC::SE_ENEMY_ATTACK, TYPE::SOUND, PATH_SND + L"EnemySE/DragonAttackSE.mp3");
	Add(SRC::SE_ENEMY_WALK, TYPE::SOUND, PATH_SND + L"EnemySE/DragonWakeSE.mp3");
	Add(SRC::SE_ENEMY_AIR_BREATH_1, TYPE::SOUND, PATH_SND + L"EnemySE/DragonFliyerAttSE1.mp3");
	Add(SRC::SE_ENEMY_AIR_BREATH_2, TYPE::SOUND, PATH_SND + L"EnemySE/DragonFliyerAttSE2.mp3");
	Add(SRC::SE_ENEMY_FALL, TYPE::SOUND, PATH_SND + L"EnemySE/DragonFallSE.mp3");
	Add(SRC::SE_ENEMY_HIT_DAMAGE, TYPE::SOUND, PATH_SND + L"EnemySE/EnemyHItDamageSE.mp3");
}

void ResourceManager::Add(SRC src, ResourceBase::TYPE type, const std::wstring& path)
{
	// すでに登録済みなら何もしない
	if (resourcesMap_.count(src) > 0)return;

	std::unique_ptr<ResourceBase> res;
	switch (type)
	{
	case ResourceBase::TYPE::IMG:
		res = std::make_unique<ResourceImage>(path);
		break;
	case ResourceBase::TYPE::MODEL:
		res = std::make_unique<ResourceModel>(path);
		break;
	case ResourceBase::TYPE::FONT:
		res = std::make_unique<ResourceFont>(path);
		break;
	case ResourceBase::TYPE::EFFEKSEER:
		res = std::make_unique<ResourceEffect>(path);
		break;
	case ResourceBase::TYPE::SOUND:
		res = std::make_unique<ResourceSound>(path);
		break;
	default:
		break;
	}

	// リソースを登録
	resourcesMap_.emplace(src, std::move(res));
}

void ResourceManager::Add(SRC src, const std::wstring& path, const Vector2& num, const Vector2& size)
{
	// すでに登録済みなら何もしない
	if (resourcesMap_.count(src) > 0)return;

	// 複数画像リソースを登録
	resourcesMap_.emplace(src, std::make_unique<ResourceImages>(path, num, size));
}

void ResourceManager::Reset(void)
{
	// 常駐リソース以外の読み込み済みハンドルを削除
	for (auto it = loadedMap_.begin(); it != loadedMap_.end();)
	{
		if (it->second
			&& it->second->GetType() != ResourceBase::TYPE::SOUND
			&& it->second->GetType() != ResourceBase::TYPE::EFFEKSEER)
		{
			it->second->Release();
			it = loadedMap_.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ResourceManager::Release(void)
{
	// 全読み込み済みハンドルを削除した上で登録配列を解放
	for (auto& p : loadedMap_)
	{
		if (p.second)
		{
			p.second->Release();
		}
	}

	// 読み込み済みマップと登録マップを解放
	loadedMap_.clear();
	resourcesMap_.clear();
}

const ResourceBase& ResourceManager::Load(SRC src)
{
	// リソースをロード	
	const ResourceBase& res = _Load(src);
	if (res.GetType() == ResourceBase::TYPE::NONE)
	{
		return *dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	// リソースをロード
	ResourceBase& res = _Load(src);
	auto* model = dynamic_cast<ResourceModel*>(&res);
	if (!model)
	{
		return -1;
	}

	// モデルの複製を生成
	return model->Duplicate();
}

int ResourceManager::LoadFont(SRC src, int size, int thickness)
{
	// リソースをロード
	ResourceBase& res = _Load(src);
	auto* font = dynamic_cast<ResourceFont*>(&res);
	if (!font)
	{
		return -1;
	}

	// フォントハンドルを生成
	return font->Load(size, thickness);
}

ResourceBase& ResourceManager::_Load(SRC src)
{
	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *lPair->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return *dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 読み込み済みマップに登録
	loadedMap_.emplace(src, rPair->second.get());

	return *rPair->second;
}
