#pragma once
#include<vector>
#include <string>
#include "../../Common/Transform.h"
#include "../ActorBase.h"

class ColliderModel;

class Stage :public ActorBase
{
public:
	// コンストラクタ
	Stage(void);

	// デストラクタ
	~Stage(void);

	// 更新
	void Update(void)override;

	//描画処理
	void Draw(void)override;
protected:
	// リソースロード
	void InitLoad(void)override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void)override;

	// 衝突判定の初期化
	void InitCollider(void)override;

	// アニメーションの初期化
	void InitAnimation(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

	// 当たり判定衝突時の更新処理
	void UpdateHitCollider(void)override;
private:
	// ステージモデル座標
	static constexpr VECTOR STAGE_POS = { 0.0f, -100.0f, 0.0f };
	// ステージモデル大きさ
	static constexpr VECTOR STAGE_SCALE = { 1.0f, 1.0f, 1.0f };

	// 除外フレーム名称
	const std::vector<std::wstring> EXCLUDE_FRAME_NAMES = { L"Ground", };

	// 対象フレーム
	const std::vector<std::wstring> TARGET_FRAME_NAMES = { L"Ground", L"Rocka" };

	// 対象フレームの不透明度率
	std::vector<int> frameOpacityRate_;
	std::shared_ptr<ColliderModel> colModel_;

	// 対象フレームの不透明度率を設定
	void RateFrameIds(const std::wstring& name);

	// 対象フレームかどうかを判定
	bool IsRateFrame(int frameIdx) const;
};

