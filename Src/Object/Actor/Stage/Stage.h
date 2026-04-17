#pragma once
#include<vector>
#include <string>
#include "../ActorBase.h"
#include "../../Common/Transform.h"
class Stage :
    public ActorBase
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

private:

	// 除外フレーム名称
	const std::vector<std::wstring> EXCLUDE_FRAME_NAMES = {	L"Mush", L"Grass", };

	// 対象フレーム
	const std::vector<std::wstring> TARGET_FRAME_NAMES = { L"Ground", };

	std::vector<int> frameOpacityRate_;

	// 衝突判定
	void Collision(void);

	void RateFrameIds(const std::wstring& name);

	bool IsRateFrame(int frameIdx) const;
};

