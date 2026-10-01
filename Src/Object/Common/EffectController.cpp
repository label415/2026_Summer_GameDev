#include <EffekseerForDXLib.h>
#include "EffectController.h"

EffectController::EffectController(void){}

EffectController::~EffectController(void)
{
	// エフェクトの解放
	effects_.clear();
}

void EffectController::Add(int type, const std::wstring path)
{
	// エフェクト追加
	auto effec = std::make_unique<EFFECT>();
	effec->ResId  = LoadEffekseerEffect(path.c_str());
	effects_.emplace(type, std::move(effec));
}

void EffectController::Add(int type, int effectId)
{
	// エフェクト追加
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

	// デフォルトの大きさ、回転、位置を設定
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

	// デフォルトの大きさ、回転、位置を設定
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

	// エフェクト再生チェック
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

	// エフェクト再生チェック
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
		// エフェクトを再生する
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

	// エフェクト描画
	DrawRotaGraph(pos.x, pos.y, 1.0f, 0.0f, effects_[type]->PlayId_, true);
}

void EffectController::SetEffectScl(int type, VECTOR scale)
{
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}
	// エフェクトの大きさを設定
	SetScalePlayingEffekseer3DEffect(effects_[type]->PlayId_, scale.x, scale.y, scale.z);
}

void EffectController::SetEffectPos(int type, VECTOR pos)
{
	// 指定されたエフェクトが存在しない場合は処理しない
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}
	// エフェクトの位置を設定
	SetPosPlayingEffekseer3DEffect(effects_[type]->PlayId_, pos.x, pos.y, pos.z);
}

void EffectController::SetEffectRot(int type, VECTOR rot)
{
	// 指定されたエフェクトが存在しない場合は処理しない
	auto it = effects_.find(type);
	if (it == effects_.end()) {
		return;
	}
	// エフェクトの回転を設定
	SetRotationPlayingEffekseer3DEffect(effects_[type]->PlayId_, rot.x, rot.y, rot.z);
}

bool EffectController::IsEnd(int type)
{
	// えふぇく再生が終了しているか
	bool res = false;
	int ret = IsEffekseer3DEffectPlaying(effects_[type]->PlayId_);
	if (ret == -1)
	{
		res =  true;
	}
	return res;
}

