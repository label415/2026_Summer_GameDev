#include <DxLib.h>
#include "ResourceManager.h"
#include "SoundManager.h"

void SoundManager::Add(SOUND_ID soundId, int handle, int volume, bool isBgm)
{
	// サウンド情報を格納
	SoundInfo info;
	info.handle = handle;
	info.volume = volume;
	info.baseFreq = (handle != -1) ? GetFrequencySoundMem(handle) : 0;
	info.isBgm = isBgm;
	sounds_[soundId] = info;
}

void SoundManager::Init(void)
{
	// リソース管理から各BGM・SEをロードして登録
	auto& res = ResourceManager::GetInstance();

	// BGM
	Add(SOUND_ID::BGM_TITLE, res.Load(ResourceManager::SRC::BGM_TITLE).handleId_, BGM_VOLUME, true);
	Add(SOUND_ID::BGM_GAME, res.Load(ResourceManager::SRC::BGM_GAME).handleId_, BGM_VOLUME, true);

	// プレイヤーSE
	Add(SOUND_ID::PLAYER_WEAPON_SE1, res.Load(ResourceManager::SRC::SE_PLAYER_WEAPON_1).handleId_, SE_VOLUME);
	Add(SOUND_ID::PLAYER_WEAPON_SE2, res.Load(ResourceManager::SRC::SE_ENEMY_HIT_DAMAGE).handleId_, SE_VOLUME);
	Add(SOUND_ID::PLAYER_WALK, res.Load(ResourceManager::SRC::SE_PLAYER_WALK).handleId_, SE_VOLUME);
	Add(SOUND_ID::PLAYER_RUN, res.Load(ResourceManager::SRC::SE_PLAYER_RUN).handleId_, SE_VOLUME);
	Add(SOUND_ID::PLAYER_HEAL, res.Load(ResourceManager::SRC::SE_PLAYER_RECOVERY).handleId_, SE_VOLUME);
	Add(SOUND_ID::PLAYER_EVASION, res.Load(ResourceManager::SRC::SE_PLAYER_EVASION).handleId_, SE_VOLUME);
	Add(SOUND_ID::PLAYER_DAMAGE, res.Load(ResourceManager::SRC::SE_PLAYER_DAMAGE).handleId_, SE_VOLUME);

	// エネミーSE
	Add(SOUND_ID::ENEMY_ROAR, res.Load(ResourceManager::SRC::SE_ENEMY_ROAR).handleId_, SE_VOLUME);
	Add(SOUND_ID::ENEMY_WALK, res.Load(ResourceManager::SRC::SE_ENEMY_WALK).handleId_, SE_VOLUME);
	Add(SOUND_ID::ENEMY_BREATH1, res.Load(ResourceManager::SRC::SE_ENEMY_BREATH_1).handleId_, SE_VOLUME);
	Add(SOUND_ID::ENEMY_BREATH2, res.Load(ResourceManager::SRC::SE_ENEMY_BREATH_2).handleId_, SE_VOLUME);
	Add(SOUND_ID::ENEMY_ATTACK, res.Load(ResourceManager::SRC::SE_ENEMY_ATTACK).handleId_, SE_VOLUME);
	Add(SOUND_ID::ENEMY_AIR, res.Load(ResourceManager::SRC::SE_ENEMY_FLAP).handleId_, SE_VOLUME);
	Add(SOUND_ID::ENEMY_AIR_BREATH1, res.Load(ResourceManager::SRC::SE_ENEMY_AIR_BREATH_1).handleId_, SE_VOLUME);
	Add(SOUND_ID::ENEMY_AIR_BREATH2, res.Load(ResourceManager::SRC::SE_ENEMY_AIR_BREATH_2).handleId_, SE_VOLUME);
	Add(SOUND_ID::ENEMY_FALL, res.Load(ResourceManager::SRC::SE_ENEMY_FALL).handleId_, SE_VOLUME);
}

void SoundManager::Release(void)
{
	// 全サウンド停止後に管理マップをクリア
	AllStopSound();
	sounds_.clear();
}

void SoundManager::PlaySoundId(SOUND_ID soundId, bool isLoop)
{
	// 対象サウンドが未登録・無効、または再生中なら再生しない
	auto it = sounds_.find(soundId);
	if (it == sounds_.end() || it->second.handle == -1
		|| CheckSoundMem(it->second.handle) == 1)
	{
		return;
	}

	// サウンドを再生させる
	SetVolume(soundId, it->second.volume);
	PlaySoundMem(it->second.handle,
		isLoop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK);
}

void SoundManager::StopSound(SOUND_ID soundId)
{
	// 対象サウンドが未登録・無効、または未再生なら停止しない
	auto it = sounds_.find(soundId);
	if (it == sounds_.end() || it->second.handle == -1
		|| CheckSoundMem(it->second.handle) != 1)
	{
		return;
	}

	// サウンドを停止
	StopSoundMem(it->second.handle);
}

void SoundManager::AllStopSound(void)
{
	// すべてのサウンドを停止
	for (const auto& sound : sounds_)
	{
		int stopHandle = sound.second.handle;
		if (stopHandle != -1)
		{
			StopSoundMem(stopHandle);
		}
	}
}

void SoundManager::AllStopSE(void)
{
	// BGM以外のすべてのSEを停止
	for (const auto& sound : sounds_)
	{
		if (!sound.second.isBgm && sound.second.handle != -1
			&& CheckSoundMem(sound.second.handle) == 1)
		{
			StopSoundMem(sound.second.handle);
		}
	}
}

void SoundManager::SetVolume(SOUND_ID id, int volume)
{
	// 有効なサウンドハンドルが見つかれば音量を変更
	auto it = sounds_.find(id);
	if (it == sounds_.end() || it->second.handle == -1)
	{
		return;
	}

	// サウンド音量設定
	ChangeVolumeSoundMem(CalcVolume(volume), it->second.handle);
}

void SoundManager::SetSpeed(SOUND_ID id, float speed)
{
	// 有効なサウンドハンドルと基準周波数があれば再生速度を変更
	auto it = sounds_.find(id);

	if (it == sounds_.end() 
		|| it->second.handle == -1 
		|| it->second.baseFreq <= 0)
	{
		return;
	}

	// サウンド再生速度設定
	SetFrequencySoundMem(static_cast<int>(it->second.baseFreq * speed), it->second.handle);
}

SoundManager::SoundManager(void)
{
	// 管理マップをクリア
	sounds_.clear();
}
