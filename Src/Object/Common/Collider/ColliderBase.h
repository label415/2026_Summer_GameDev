#pragma once
#include <memory>
#include <vector>
#include <DxLib.h>
#include "../../../Utility/AsoUtility.h"

class Transform;

class ColliderBase
{
public:
	// 形状
	enum class SHAPE
	{
		NONE,
		LINE,
		SPHERE,
		CAPSULE,
		MODEL,
		BOX
	};

	// 衝突種別
	enum class TAG
	{
		NONE,
		STAGE,
		PLAYER,
		CAMERA,
		ENEMY,
		VIEW_RANGE,
		PLAYER_WEPON,
		ENEMY_WEPON,
		GROUND
	};

	// 処理結果
	struct HitInfo
	{
		// 衝突相手
		TAG targetTag_ = TAG::NONE;
		// 押し出し方向
		VECTOR pushVectorDir_;
		// 押し出し量
		VECTOR  pushVector_;
		// 衝突フラグ
		bool isHit_ = false;
		// 遮断フラグ
		bool isBlocked = false;
	};

	// コンストラクタ
	ColliderBase(
		SHAPE shape,
		TAG tag,
		const std::vector<TAG>& targetTags,
		std::weak_ptr<const Transform> follow,
		int patrTag);

	// デストラクタ
	virtual ~ColliderBase(void);

	// 描画
	void Draw(void);

	// 追従先の取得
	std::weak_ptr<const Transform> GetFollow(void) const { return follow_; }

	// 追従先の再設定
	void SetFollow(std::weak_ptr<const Transform> follow);

	// 形状
	SHAPE GetShape(void) const { return shape_; }

	// 衝突種別
	TAG GetTag(void) const { return tag_; }

	//各部位衝突種別
	int GetPatrTag(void)const { return patrTag_; }

	// 当たり判定フラグを設定する
	void SetIsCollier(bool isIsCollier);

	// 当たり判定フラグを取得
	bool GetIsCollier(void)const { return isCollier_; }

	// 当たり判定対象タグの判定
	bool IsTargetTag(TAG tag) const
	{
		return std::find(
			targetTags_.begin(),
			targetTags_.end(),
			tag) != targetTags_.end();
	}

	// 衝突結果の格納・取得
	void ClearCollisionResults() { hitInfos_.clear(); }
	void AddCollisionResult(const HitInfo& res) { hitInfos_.push_back(res); }
	const std::vector<HitInfo>& GetCollisionResults() const { return hitInfos_; }
protected:
	// デバッグ表示の色
	static constexpr int COLOR_VALID = 0xff0000;
	static constexpr int COLOR_INVALID = 0xaaaaaa;

	// 衝突判定対象とするタグ一覧
	std::vector<TAG> targetTags_;

	// 形状
	SHAPE shape_;

	// 衝突種別
	TAG tag_;

	//各部位のタグ番号
	int patrTag_;

	// 追従先
	std::weak_ptr<const Transform> follow_;

	// 有効フラグ
	bool isCollier_;

	// 処理結果情報格納関数
	std::vector<HitInfo> hitInfos_;

	// ローカル座標をワールド座標に変換
	VECTOR GetRotPos(const VECTOR& localPos) const;

	// デバッグ用描画
	virtual void DrawDebug(int color) = 0;
};
