#pragma once
#include <map>
#include <DxLib.h>
#include "../../Template/Singleton.h"

class EffectManager : public Singleton<EffectManager>
{

	// Singleton 基底クラスからコンストラクタ・デストラクタへアクセスできるようにする
	friend class Singleton<EffectManager>;

public:

	// 管理するエフェクト種別ID
	enum class EFFECT_ID
	{
		BLOOD,
		HP_ABSOLUTE,
		FALLING_ATTACK,
		ROAR,
		CHARGE,
		BREATH,
		FIREBALL_BULLET,
		FIREBALL_EXPLOSION,
	};

	// デフォルトの座標・回転・スケール
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, 0.0f };
	static constexpr VECTOR DEFAULT_ROT = { 0.0f, 0.0f, 0.0f };
	static constexpr VECTOR DEFAULT_SCALE = { 1.0f, 1.0f, 1.0f };

	// 初期化
	void Init(void);

	// Effekseer 本体の終了処理
	void EndEffekseer(void);

	// エフェクトリソース登録
	void Add(EFFECT_ID effectId, int resHandle);

	// エフェクト再生（再生管理ハンドルを返す）
	int PlayEffect(
		EFFECT_ID effectId,
		const VECTOR& pos = DEFAULT_POS,
		const VECTOR& rot = DEFAULT_ROT,
		const VECTOR& scale = DEFAULT_SCALE,
		bool isLoop = false);

	// 再生中エフェクトの停止
	void Stop(int playHandle);

	// 再生中の全エフェクトを停止（リソース参照は保持）
	void AllStop(void);

	// 更新処理
	void Update(void);

	// 描画処理
	void Draw(void);

	// 解放処理
	void Release(void);

	// 再生中エフェクトの位置設定
	void SetPos(int playHandle, const VECTOR& pos);

	// 再生中エフェクトのスケール設定
	void SetScl(int playHandle, const VECTOR& scale);

	// 再生中エフェクトの回転設定
	void SetRot(int playHandle, const VECTOR& rot);

	// 指定した再生ハンドルが再生終了しているかを返す
	bool IsEnd(int playHandle) const;

private:

	// Effekseer の最大パーティクル数
	static constexpr int EFFEKSEER_MAX_PARTICLES = 8000;

	// 再生中エフェクト1件分の管理情報
	struct PlayInstance
	{
		// エフェクト種別ID
		EFFECT_ID id = EFFECT_ID::BLOOD;
		// 元リソースハンドル
		int resId = -1;
		// Effekseer 再生ハンドル
		int playId = -1;
		// 現在座標
		VECTOR pos = DEFAULT_POS;
		// 現在回転
		VECTOR rot = DEFAULT_ROT;
		// 現在スケール
		VECTOR scale = DEFAULT_SCALE;
		// ループ再生フラグ
		bool isLoop = false;
	};

	// コンストラクタを private にして外部生成を防止
	EffectManager(void);

	// コピーコンストラクタを禁止
	EffectManager(const EffectManager& instance) = delete;

	// デストラクタもprivateにして外部破棄を防止
	~EffectManager(void) override = default;

	// Effekseer 本体の初期化
	void InitEffekseer(void);

	// エフェクトIDとリソースハンドルの対応マップ
	std::map<EFFECT_ID, int> effectResIds_;

	// 発行した再生ハンドルと再生インスタンスの対応マップ
	std::map<int, PlayInstance> playInstances_;

	// 次に発行する再生管理ハンドル番号
	int nextHandle_;

	// Effekseer本体初期化済みフラグ
	bool isSystemInitialized_;

	// 再生中ハンドルへ座標・回転・スケールを反映する内部ヘルパ
	void ApplyTransform(const PlayInstance& inst) const;

};
