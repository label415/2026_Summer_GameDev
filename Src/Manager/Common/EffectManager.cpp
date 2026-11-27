#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "ResourceManager.h"
#include "EffectManager.h"

EffectManager::EffectManager(void)
	: nextHandle_(1)
	, isSystemInitialized_(false)
{
	// コンテナを初期状態で空にしておく
	effectResIds_.clear();
	playInstances_.clear();
}

void EffectManager::InitEffekseer(void)
{
	// すでに初期化済みなら何もしない
	if (isSystemInitialized_)return;

	// Effekseer の初期化
	if (Effekseer_Init(EFFEKSEER_MAX_PARTICLES) == -1)
	{
		DxLib_End();
		return;
	}

	// 画面設定変更時のグラフィックスシステムリセット設定とデバイスロストコールバック登録
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
	isSystemInitialized_ = true;
}

void EffectManager::EndEffekseer(void)
{
	// 初期化されていなければ何もしない
	if (!isSystemInitialized_)	return;

	// Effekseer を終了
	Effkseer_End();
	isSystemInitialized_ = false;
}

void EffectManager::Init(void)
{
	// 初回呼び出し時に Effekseer 本体を初期化
	InitEffekseer();

	// ResourceManager から各エフェクトリソースをロードして登録
	auto& res = ResourceManager::GetInstance();

	// エフェクトリソースの登録
	Add(EFFECT_ID::BLOOD, res.Load(ResourceManager::SRC::EFF_BLOOD).handleId_);
	Add(EFFECT_ID::HP_ABSOLUTE, res.Load(ResourceManager::SRC::EFF_HP_ABSOLUTE).handleId_);
	Add(EFFECT_ID::FALLING_ATTACK, res.Load(ResourceManager::SRC::EFF_FALLING_ATTACK).handleId_);
	Add(EFFECT_ID::ROAR, res.Load(ResourceManager::SRC::EFF_ROAR).handleId_);
	Add(EFFECT_ID::CHARGE, res.Load(ResourceManager::SRC::EFF_CHARGE).handleId_);
	Add(EFFECT_ID::BREATH, res.Load(ResourceManager::SRC::EFF_BREATH).handleId_);
	Add(EFFECT_ID::FIREBALL_BULLET, res.Load(ResourceManager::SRC::EFF_FIREBALL_BULLET).handleId_);
	Add(EFFECT_ID::FIREBALL_EXPLOSION, res.Load(ResourceManager::SRC::EFF_FIREBALL_EXPLOSION).handleId_);
}

void EffectManager::Add(EFFECT_ID effectId, int resHandle)
{
	// 無効ハンドルは登録しない
	if (resHandle == -1)return;

	// エフェクトIDに対応するリソースハンドルを登録・更新
	effectResIds_[effectId] = resHandle;
}

int EffectManager::PlayEffect(
	EFFECT_ID effectId,
	const VECTOR& pos,
	const VECTOR& rot,
	const VECTOR& scale,
	bool isLoop)
{
	// 登録済みのエフェクトリソースを検索
	auto it = effectResIds_.find(effectId);
	if (it == effectResIds_.end() || it->second == -1)return -1;

	// Effekseer3D 再生を開始
	int playId = PlayEffekseer3DEffect(it->second);
	if (playId == -1)return -1;

	// 再生インスタンス情報を作成して座標・回転・スケールを反映
	PlayInstance inst;
	inst.id = effectId;
	inst.resId = it->second;
	inst.playId = playId;
	inst.pos = pos;
	inst.rot = rot;
	inst.scale = scale;
	inst.isLoop = isLoop;
	ApplyTransform(inst);

	// 一意の管理ハンドルを発行して管理マップに登録
	int managedHandle = nextHandle_++;
	playInstances_.emplace(managedHandle, inst);

	return managedHandle;
}

void EffectManager::Stop(int playHandle)
{
	// 管理中の再生インスタンスを検索
	auto it = playInstances_.find(playHandle);
	if (it == playInstances_.end())	return;

	// 再生中であれば Effekseer 側を停止（0: 再生中）
	if (it->second.playId != -1 && IsEffekseer3DEffectPlaying(it->second.playId) == 0)
	{
		StopEffekseer3DEffect(it->second.playId);
	}

	// 管理マップから削除
	playInstances_.erase(it);
}

void EffectManager::AllStop(void)
{
	// 再生中の全エフェクトを停止（0: 再生中）
	for (const auto& pair : playInstances_)
	{
		if (pair.second.playId != -1 && IsEffekseer3DEffectPlaying(pair.second.playId) == 0)
		{
			StopEffekseer3DEffect(pair.second.playId);
		}
	}

	// 再生インスタンスマップをクリア（リソースIDマップは保持）
	playInstances_.clear();
	nextHandle_ = 1;
}

void EffectManager::Update(void)
{
	// 再生終了したインスタンスのループ再再生または削除を処理
	for (auto it = playInstances_.begin(); it != playInstances_.end();)
	{
		auto& inst = it->second;
		if (inst.playId == -1 || IsEffekseer3DEffectPlaying(inst.playId) != 0)
		{
			if (inst.isLoop && inst.resId != -1)
			{
				// ループ指定がある場合は再再生して現在の姿勢を再設定
				inst.playId = PlayEffekseer3DEffect(inst.resId);
				if (inst.playId != -1)
				{
					ApplyTransform(inst);
					++it;
					continue;
				}
			}

			// ループしない、または再再生不可の場合は管理対象から除外
			it = playInstances_.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Effekseer の3Dエフェクト全体を更新
	UpdateEffekseer3D();
}

void EffectManager::Draw(void)
{
	// Effekseer の3Dエフェクトを描画
	DrawEffekseer3D();
}

void EffectManager::Release(void)
{
	// 再生中の全エフェクトを停止し、リソース参照マップも解放
	AllStop();
	effectResIds_.clear();
}

void EffectManager::SetPos(int playHandle, const VECTOR& pos)
{
	// 管理中の再生インスタンスを検索
	auto it = playInstances_.find(playHandle);
	if (it == playInstances_.end())	return;
	
	// 座標を保存して Effekseer 側へ反映
	it->second.pos = pos;
	SetPosPlayingEffekseer3DEffect(it->second.playId, pos.x, pos.y, pos.z);
}

void EffectManager::SetScl(int playHandle, const VECTOR& scale)
{
	// 管理中の再生インスタンスを検索
	auto it = playInstances_.find(playHandle);
	if (it == playInstances_.end())return;

	// スケールを保存して Effekseer 側へ反映
	it->second.scale = scale;
	SetScalePlayingEffekseer3DEffect(it->second.playId, scale.x, scale.y, scale.z);
}

void EffectManager::SetRot(int playHandle, const VECTOR& rot)
{
	// 管理中の再生インスタンスを検索
	auto it = playInstances_.find(playHandle);
	if (it == playInstances_.end())return;
	
	// 回転を保存して Effekseer 側へ反映
	it->second.rot = rot;
	SetRotationPlayingEffekseer3DEffect(it->second.playId, rot.x, rot.y, rot.z);
}

bool EffectManager::IsEnd(int playHandle) const
{
	// 管理対象外、または Effekseer 側が再生中でなければ終了とみなす
	auto it = playInstances_.find(playHandle);
	if (it == playInstances_.end())return true;

	// Effekseer 側の再生状態を確認（0: 再生中 / -1: 終了または未再生）
	return IsEffekseer3DEffectPlaying(it->second.playId) != 0;
}

void EffectManager::ApplyTransform(const PlayInstance& inst) const
{
	// 無効ハンドルには適用しない
	if (inst.playId == -1)return;
	
	// 座標・回転・スケールを Effekseer 再生ハンドルへ反映
	SetPosPlayingEffekseer3DEffect(inst.playId, inst.pos.x, inst.pos.y, inst.pos.z);
	SetRotationPlayingEffekseer3DEffect(inst.playId, inst.rot.x, inst.rot.y, inst.rot.z);
	SetScalePlayingEffekseer3DEffect(inst.playId, inst.scale.x, inst.scale.y, inst.scale.z);
}
