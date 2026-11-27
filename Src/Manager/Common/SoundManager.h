#pragma once
#include <map>
#include "../../Template/Singleton.h"

class SoundManager : public Singleton<SoundManager>
{
    friend class Singleton<SoundManager>;
public:

    // サウンドの種類を定義するenum
    enum class SOUND_ID
    {
        BGM_TITLE,
        BGM_GAME,

        PLAYER_WEAPON_SE1,
        PLAYER_WEAPON_SE2,
        PLAYER_WALK,
        PLAYER_RUN,
        PLAYER_HEAL,
        PLAYER_EVASION,
        PLAYER_DAMAGE,

        ENEMY_ROAR,
        ENEMY_WALK,
        ENEMY_BREATH1,
        ENEMY_BREATH2,
        ENEMY_ATTACK,
        ENEMY_AIR,
        ENEMY_AIR_BREATH1,
        ENEMY_AIR_BREATH2,
        ENEMY_FALL,
        MAX,
    };

    // ユーザーが指定する最大音量
    static constexpr int MAX_VOLUME_VALUE = 100;
    // DxLib内部で扱われる最大音量
    static constexpr int DX_LIB_MAX_VOLUME = 255;
    // 共通BGM音量
    static constexpr int BGM_VOLUME = 40;
    // 共通SE音量
    static constexpr int SE_VOLUME = 50;

    // 初期化
    void Init(void);

	// 解放
    void Release(void);

    // サウンド登録
    void Add(SOUND_ID soundId, int handle, int volume, bool isBgm = false);

	// サウンド再生
    void PlaySoundId(SOUND_ID soundId, bool isLoop = false);

	// サウンド停止
    void StopSound(SOUND_ID soundId);

	// 全てのサウンド停止
    void AllStopSound(void);

	// 全てのBGM停止
    void AllStopSE(void);

	// サウンドの音量を設定する
    void SetVolume(SOUND_ID id, int volume);

	// サウンドの再生速度を設定する
    void SetSpeed(SOUND_ID id, float speed);

private:

    // サウンド管理データ
    struct SoundInfo
    {
        int handle = -1;
        int volume = 0;
        int baseFreq = 0;
        bool isBgm = false;
    };

    // コンストラクタ
    SoundManager(void);

    // コピーコンストラクタ
    SoundManager(const SoundManager& instance) = default;

    // デストラクタ
    ~SoundManager(void) override = default;

    // サウンドIDごとの管理マップ
    std::map<SOUND_ID, SoundInfo> sounds_;

    // 0-100の値をDxLib用の0-255に変換する内部関数
    int CalcVolume(int volume) const {
        return (DX_LIB_MAX_VOLUME * volume) / MAX_VOLUME_VALUE;
    }

};