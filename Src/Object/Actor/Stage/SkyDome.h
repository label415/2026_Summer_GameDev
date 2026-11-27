#pragma once
#include "../../Common/Transform.h"
#include "../ActorBase.h"

class SkyDome :public ActorBase
{
public:

	// コンストラクタ
	SkyDome(void){}

	// デストラクタ
	~SkyDome(void) override = default;

	// 更新
	void Update(void)override;

	// 描画
	void Draw(void)override;

protected:

	// リソースロード
	void InitLoad(void)override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void)override;

	// 衝突判定の初期化
	void InitCollider(void)override{}

	// アニメーションの初期化
	void InitAnimation(void)override{}

	// 初期化後の個別処理
	void InitPost(void)override;

	// 衝突時の更新
	void UpdateHitCollider(void)override{}

private:

	// 回転速度(deg)
	static constexpr float ROT_SPEED_DEG = -0.01f;

	// モデルスケール
	static constexpr VECTOR DEFAULT_SCALE = { 100.0f, 100.0f, 100.0f };

};

