#include <EffekseerForDXLib.h>
#include "EffectController.h"

EffectController::EffectController(void)
{
}

EffectController::~EffectController(void)
{
	effects_.clear();
}

void EffectController::Add(int type, const std::wstring path)
{

	auto effec = std::make_unique<EFFECT>();
	effec->ResId  = LoadEffekseerEffect(path.c_str());

	effects_.emplace(type, std::move(effec));
}

void EffectController::Add(int type, int effectId)
{
	auto effec = std::make_unique<EFFECT>();
	effec->ResId = effectId;

	effects_.emplace(type, std::move(effec));
}

void EffectController::Play(int type)
{
	// 指定されたエフェクトが存在しない場合は処理しない
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}

	// エフェクトを再生する
	effects_[type]->PlayId_ = PlayEffekseer3DEffect(effects_[type]->ResId);

	SetScalePlayingEffekseer3DEffect(effects_[type]->PlayId_, 1.0f, 1.0f, 1.0f);
	SetRotationPlayingEffekseer3DEffect(effects_[type]->PlayId_, 0.0f, 0.0f, 0.0f);
	SetPosPlayingEffekseer3DEffect(effects_[type]->PlayId_, 1.0f, 1.0f, 1.0f);

	
}
void EffectController::Play(int type, const VECTOR pos, VECTOR rot, VECTOR scale)
{
	// 指定されたエフェクトが存在しない場合は処理しない
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}

	// エフェクトを再生する
	effects_[type]->PlayId_ = PlayEffekseer3DEffect(effects_[type]->ResId);

	SetScalePlayingEffekseer3DEffect(effects_[type]->PlayId_, scale.x, scale.y, scale.z);
	SetRotationPlayingEffekseer3DEffect(effects_[type]->PlayId_, rot.x, rot.y, rot.z);
	SetPosPlayingEffekseer3DEffect(effects_[type]->PlayId_, pos.x, pos.y, pos.z);

}
//アニメーションが終わていたら再生
void EffectController::LoopPlay(int type)
{
	// 指定されたエフェクトが存在しない場合は処理しない
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}

	int ret = IsEffekseer3DEffectPlaying(effects_[type]->PlayId_);
	if (ret == -1)
	{
		// エフェクトを再生する
		effects_[type]->PlayId_ = PlayEffekseer3DEffect(effects_[type]->ResId);
	}
}

void EffectController::Stop(int type)
{
	int ret = IsEffekseer3DEffectPlaying(effects_[type]->PlayId_);
	if (ret == 0)
	{
		StopEffekseer3DEffect(effects_[type]->PlayId_);
	}
}

void EffectController::LoopUpdate(int type, const VECTOR pos)
{
	// 指定されたエフェクトが存在しない場合は処理しない
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}

	SetPosPlayingEffekseer3DEffect(effects_[type]->PlayId_, pos.x, pos.y, pos.z);
}

void EffectController::Update(int type, bool isLoop)
{
	// 指定されたエフェクトが存在しない場合は処理しない
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}

	// エフェクト再生チェック
	// 0:再生中、-1:再生されていない、もしくは再生終了
	int ret = IsEffekseer3DEffectPlaying(effects_[type]->PlayId_);
	if (ret == -1)
	{
		if (isLoop) {
			effects_[type]->PlayId_ = PlayEffekseer3DEffect(effects_[type]->ResId);
		}
		else {
			StopEffekseer3DEffect(effects_[type]->PlayId_);
		}
	}
}

void EffectController::Draw(int type, const VECTOR pos)
{
	// 指定されたエフェクトが存在しない場合は処理しない
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}

	DrawRotaGraph(pos.x, pos.y, 1.0f, 0.0f, effects_[type]->PlayId_, true);
}

void EffectController::SetEffectScl(int type, VECTOR scale)
{
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}
	SetScalePlayingEffekseer3DEffect(effects_[type]->PlayId_, scale.x, scale.y, scale.z);
}

void EffectController::SetEffectPos(int type, VECTOR pos)
{
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}
	SetPosPlayingEffekseer3DEffect(effects_[type]->PlayId_, pos.x, pos.y, pos.z);
}

void EffectController::SetEffectRot(int type, VECTOR rot)
{
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}
	SetRotationPlayingEffekseer3DEffect(effects_[type]->PlayId_, rot.x, rot.y, rot.z);
}

bool EffectController::IsEnd(int type)
{
	bool res = false;
	int ret = IsEffekseer3DEffectPlaying(effects_[type]->PlayId_);
	if (ret == -1)
	{
		res =  true;
	}
	return res;
}

