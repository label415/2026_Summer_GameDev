#pragma once
#include <map>




class SoundManager
{
public:

    enum class SeId
    {
        PLAYER_WEPON_SE1,
        PLAYER_WEPON_SE2,
        PLAYER_WAKE,
        PLAYER_RAN,
        PLAYER_HER,
        PLAYER_AVE,
        PLAYER_DMAGE,

        ENEMY_ROAR,
        ENEMY_WAKE,
        ENEMY_BREASE1,
        ENEMY_BREASE2,
        ENEMY_ATTCEK,
        ENEMY_ARE,
        ENEMY_ARE_ENEMY_BREASE1,
        ENEMY_ARE_ENEMY_BREASE2,
        ENEMY_FALL,
        Max,
    };

    
    static void CreateInstance(void);  
    static SoundManager& GetInstance(void); 

    void Init(void); // 内部変数の初期化

    // --- 定数定義 ---
    static constexpr int MaxVolumeValue = 100;   // ユーザーが指定する最大音量
    static constexpr int DxLibMaxVolume = 255;  // DxLib内部で扱われる最大音量

    // --- BGM機能 ---
    void PlayBGM(int handle, int volume = MaxVolumeValue); // BGMのループ再生
    void PauseBGM();                                       // BGMの一時停止
    void ResumeBGM();                                      // BGMの再開（停止位置から）
    void StopBGM();                                        // BGMの完全停止とメモリ解放
    void SetBGMVolume(int volume);                         // 再生中のBGM音量変更
    void PlaySlowBGM(int handle,int slow=2);                          // BGMのスロー再生

    // --- SE機能 ---
    void PlaySE(SeId id, int handle, int volume = MaxVolumeValue);      // SEの再生
    void PlayLoopSE(SeId id, int handle, int volume = MaxVolumeValue);      // SEの再生
    void PlaySlowSE(SeId id, int handle, int volume = MaxVolumeValue,int slow=2);  // スローSE再生
    void StopSE(SeId id);                                               // 特定のSEを停止
    void AllStopSE();                                                   // 全てのSEを停止

    void SetSESpeed(SeId id, float speed);

private:
    SoundManager() = default;
    ~SoundManager() = default;
    static SoundManager* instance_;

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