#include <DxLib.h>
#include "SoundManager.h"

SoundManager* SoundManager::instance_ = nullptr;

void SoundManager::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new SoundManager();
        instance_->Init();
    }
}

SoundManager& SoundManager::GetInstance(void)
{
    // インスタンスがなければ生成する
    if (instance_ == nullptr)
    {
        CreateInstance();
    }
    return *instance_;
}

void SoundManager::Init(void)
{
    bgm_ = -1;
    currentBgmVol_ = DxLibMaxVolume;
    isBgmPaused_ = false;
    seMap_.clear();
}

// --- BGM機能の実装 ---

void SoundManager::PlayBGM(int handle, int volume)
{
   
    StopBGM();
    //ハンドルの複製
    bgm_ = DuplicateSoundMem(handle);
    if (bgm_ != -1)
    {
        
        SetBGMVolume(volume);
        PlaySoundMem(bgm_, DX_PLAYTYPE_LOOP, TRUE); 
        isBgmPaused_ = false;
    }
}

void SoundManager::PauseBGM()
{
    // 再生中の場合のみ停止処理を行う
    if (bgm_ != -1 && CheckSoundMem(bgm_) == TRUE)
    {
        StopSoundMem(bgm_);
        isBgmPaused_ = true;
    }
}

void SoundManager::ResumeBGM()
{
    // 一時停止フラグが立っている場合のみ再開
    if (bgm_ != -1 && isBgmPaused_)
    {
        
        PlaySoundMem(bgm_, DX_PLAYTYPE_LOOP, FALSE);
        isBgmPaused_ = false;
    }
}

void SoundManager::StopBGM()
{
    if (bgm_ != -1)
    {
        StopSoundMem(bgm_);     
        DeleteSoundMem(bgm_);   
        bgm_ = -1;
        isBgmPaused_ = false;
    }
}

void SoundManager::SetBGMVolume(int volume)
{
    if (bgm_ != -1) {
        
        ChangeVolumeSoundMem(CalcVolume(volume), bgm_);
    }
}

void SoundManager::PlaySlowBGM(int handle,int slow)
{
    StopBGM();
    bgm_ = DuplicateSoundMem(handle);
    if (bgm_ != -1)
    {
       
        int freq = GetFrequencySoundMem(bgm_);
        SetFrequencySoundMem(freq / slow, bgm_);
        PlaySoundMem(bgm_, DX_PLAYTYPE_LOOP, TRUE);
    }
}

// --- SE機能の実装 ---

void SoundManager::PlaySE(SeId id, int handle, int volume)
{
    // 同じIDのSEが既に鳴っていれば停止（連打時のノイズ防止や音の重なり制御）
    StopSE(id);

    int newSe = DuplicateSoundMem(handle);
    if (newSe != -1)
    {
        ChangeVolumeSoundMem(CalcVolume(volume), newSe);
        PlaySoundMem(newSe, DX_PLAYTYPE_BACK, TRUE); // 非同期で再生
        seMap_[id] = newSe; // IDをキーにしてハンドルを保存
    }
}

void SoundManager::StopSE(SeId id)
{
    // マップ内に指定されたIDが存在するか確認
    if (seMap_.count(id))
    {
        StopSoundMem(seMap_[id]);   // 再生停止
        DeleteSoundMem(seMap_[id]); // メモリ解放
        seMap_.erase(id);           // リストから削除
    }
}

void SoundManager::AllStopSE()
{
    // 管理されているすべてのSEをループで停止・解放
    for (auto& pair : seMap_) {
        StopSoundMem(pair.second);
        DeleteSoundMem(pair.second);
    }
    seMap_.clear();
}

void SoundManager::PlaySlowSE(SeId id, int handle, int volume,int slow)
{
    StopSE(id);
    int newSe = DuplicateSoundMem(handle);
    if (newSe != -1)
    {
        ChangeVolumeSoundMem(CalcVolume(volume), newSe);
        // 周波数を下げて「スロー演出（鈍い音）」にする
        int freq = GetFrequencySoundMem(newSe);
        SetFrequencySoundMem(freq / slow, newSe);
        PlaySoundMem(newSe, DX_PLAYTYPE_BACK, TRUE);
        seMap_[id] = newSe;
    }
}