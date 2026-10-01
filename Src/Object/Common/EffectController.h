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
		// エフェクトリソースID
		int ResId = -1;
		int PlayId_ = -1;

		// コンストラクタ
		~EFFECT() {
			// 再生中のエフェクトがあれば停止する
			if (PlayId_ != -1 && IsEffekseer3DEffectPlaying(PlayId_) == 0) {
				StopEffekseer3DEffect(PlayId_);
			}

			// エフェクトリソースがあれば解放する
			if (ResId != -1) {
				DeleteEffekseerEffect(ResId);
			}
		}
	};

	// コンストラクタ
	EffectController(void);

	// デストラクタ
	~EffectController(void);

	// エフェクトの追加
	void Add(int type, const std::wstring path);
	void Add(int type, int effectId);

	// エフェクト再生
	void Play(int type);
	void Play(int type, const VECTOR pos,
		VECTOR rot = { 0.0f,0.0f,0.0f }, VECTOR scale = { 1.0f,1.0f,1.0f });

	// エフェクトループ再生
	void LoopPlay(int type);

	// エフェクト削除
	void Stop(int type);

	// エフェクトループ再生更新
	void LoopUpdate(int type, const VECTOR pos);

	// エフェクト更新
	void Update(int type = -1, bool isLoop = false);

	// エフェクト描画
	void Draw(int type, const VECTOR pos);

	// エフェクトの大きさ、位置、回転を設定
	void SetEffectScl(int type, VECTOR scale);
	void SetEffectPos(int type, VECTOR pos);
	void SetEffectRot(int type, VECTOR rot);

	// エフェクトの再生が終了しているかを返す
	bool IsEnd(int type);
private:
	// 種類別のアニメーションデータ
	std::map<int, std::unique_ptr<EFFECT>> effects_;
};

