#pragma once
#include <map>
#include "../Template/Singleton.h"

class SoundManager : public Singleton<SoundManager>
{
    friend class Singleton<SoundManager>;
public:

	// SEのID（名前）を定義するenum
    enum class SeId
    {
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
        Max,
    };

    void Init(void);    // 内部変数の初期化
    void Release(void); // リソースの解放

    // --- 定数定義 ---
    static constexpr int MaxVolumeValue = 100;   // ユーザーが指定する最大音量
    static constexpr int DxLibMaxVolume = 255;  // DxLib内部で扱われる最大音量
    static constexpr int DEFAULT_SLOW_RATE = 2;  // デフォルトのスロー再生倍率

    // --- BGM機能 ---
    void PlayBGM(int handle, int volume = MaxVolumeValue); // BGMのループ再生
    void PauseBGM();                                       // BGMの一時停止
    void ResumeBGM();                                      // BGMの再開（停止位置から）
    void StopBGM();                                        // BGMの完全停止とメモリ解放
    void SetBGMVolume(int volume);                         // 再生中のBGM音量変更
    void PlaySlowBGM(int handle, int slow = DEFAULT_SLOW_RATE); // BGMのスロー再生

    // --- SE機能 ---
    void PlaySE(SeId id, int handle, int volume = MaxVolumeValue);      // SEの再生
    void PlayLoopSE(SeId id, int handle, int volume = MaxVolumeValue);      // SEのループ再生
    void PlaySlowSE(SeId id, int handle, int volume = MaxVolumeValue, int slow = DEFAULT_SLOW_RATE);  // スローSE再生
    void StopSE(SeId id);                                               // 特定のSEを停止
    void AllStopSE();                                                   // 全てのSEを停止
    void SetSESpeed(SeId id, float speed);                              // SEの再生速度を設定

private:

	// コンストラクタ
    SoundManager(void);
    // コピーコンストラクタ
    SoundManager(const SoundManager& instance) = default;
    // デストラクタ
    ~SoundManager(void) = default;

    int bgm_;           // 再生中BGMの複製ハンドル
    int currentBgmVol_; // 現在のBGM音量(0-255)
    bool isBgmPaused_;  // 一時停止フラグ

    // SEをID（名前）で管理するためのマップ（二重再生防止・個別停止用）
    std::map<SeId, int> seMap_;

    // 0-100の値をDxLib用の0-255に変換する内部関数
    int CalcVolume(int volume) const {
        return (DxLibMaxVolume * volume) / MaxVolumeValue;
    }

};