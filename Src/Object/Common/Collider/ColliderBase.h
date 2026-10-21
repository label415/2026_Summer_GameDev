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

	// コンストラクタ
	ColliderBase(
		SHAPE shape,
		TAG tag,
		const std::vector<TAG>& targetTags,
		const Transform* follow,
		int patrTag = 0);

	// デストラクタ
	virtual ~ColliderBase(void);

	// 描画
	void Draw(void);

	// 追従先の取得
	const Transform* GetFollow(void) const { return follow_; }

	// 追従先の再設定
	void SetFollow(const Transform* follow);

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

	// 行動フラグを設定
	void SetIsAction(bool isAction){ isAction_ = isAction; }

	// 行動フラグを取得
	bool GetIsAction(void)const { return isAction_; }

	// 当たり判定対象タグの判定
	bool IsTargetTag(TAG tag) const
	{
		return std::find(
			targetTags_.begin(),
			targetTags_.end(),
			tag) != targetTags_.end();
	}

	// 衝突したコライダーの解放
	void ClearHitCollider(void);

	// 衝突したコライダーを登録
	void SetHitCollider(std::weak_ptr<ColliderBase> hitCollider)
	{
		hitCollider_.push_back(hitCollider);
	}

	// 衝突したコライダーの取得
	const std::vector<std::weak_ptr<ColliderBase>>& GetHitColliders(void) { return hitCollider_; }
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
	const Transform* follow_;

	// 有効フラグ
	bool isCollier_;

	// 行動フラグ
	bool isAction_;

	// 衝突した当たり判定
	std::vector<std::weak_ptr<ColliderBase>> hitCollider_;

	// ローカル座標をワールド座標に変換
	VECTOR GetRotPos(const VECTOR& localPos) const;

	// デバッグ用描画
	virtual void DrawDebug(int color) = 0;
};
