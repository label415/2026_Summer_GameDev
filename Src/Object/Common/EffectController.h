#pragma once
#include <map>
#include <memory>
#include <string>
#include <EffekseerForDXLib.h>


class EffectController
{
public:

	// アニメーションデータ
	struct EFFECT
	{
		int ResId = -1;
		int PlayId_ = -1;

		~EFFECT() {
			if (PlayId_ != -1 && IsEffekseer3DEffectPlaying(PlayId_) == 0) {
				StopEffekseer3DEffect(PlayId_);
			}

			if (ResId != -1) {
				DeleteEffekseerEffect(ResId);
			}
		}
	};

	EffectController(void);
	~EffectController(void);

	void Add(int type, const std::wstring path);
	void Add(int type, int effectId);

	void Play(int type);
	void Play(int type, const VECTOR pos, VECTOR rot = { 0.0f,0.0f,0.0f }, VECTOR scale = { 1.0f,1.0f,1.0f });
	void LoopPlay(int type);
	void Stop(int type);

	void LoopUpdate(int type, const VECTOR pos);
	void Update(int type, const VECTOR pos);
	void Update(int type = -1);

	void Draw(int type, const VECTOR pos);

	void SetEffectScl(int type, VECTOR scale);
	void SetEffectPos(int type, VECTOR pos);
	void SetEffectRot(int type, VECTOR rot);

private:

	// 種類別のアニメーションデータ
	std::map<int, std::unique_ptr<EFFECT>> effects_;

};

