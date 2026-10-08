#pragma once
#include <DxLib.h>
#include "WeponBase.h"

class Transform;

class WeponBlade : public WeponBase
{
public:
	// コンストラクタ
	WeponBlade(const Transform& followTransform, int followFrameId);

	// デストラクタ
	~WeponBlade(void) override;
	// 更新
	void Update(void) override;

	// コライダーを設定
	void SetCollider(void) override;

	// コライダーをクリア
	void ClearCollider(void) override;
protected:
	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;
private:
	// モデルの大きさ
	static constexpr float SCALE = 1.3f;

	// 衝突判定用カプセルの初期値
		static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 150.0f, 0.0f };
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 20.0f, 0.0f };
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;

	// 保持しているコライダーへの参照
	std::shared_ptr<ColliderCapsule> colliderCapsule_;
};

