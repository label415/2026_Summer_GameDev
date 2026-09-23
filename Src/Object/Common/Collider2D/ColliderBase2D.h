#pragma once
#include <DxLib.h>
#include "../../../Utility/AsoUtility.h"

class ColliderBase2D
{
public:
	// 形状
	enum class SHAPE
	{
		NONE,
		LINE,
		BOX
	};
	// 衝突種別
	enum class TAG
	{
		UI
	};

	// コンストラクタ
	ColliderBase2D(SHAPE shape, TAG tag);
	// デストラクタ
	virtual ~ColliderBase2D(void);
	// 描画
	void Draw(void);
	// 形状
	SHAPE GetShape(void) const { return shape_; }
	// 衝突種別
	TAG GetTag(void) const { return tag_; }
	//色変更
	void SetValid(bool isValid);

protected:
	// デバッグ表示の色
	static constexpr int COLOR_VALID = 0xff0000;
	static constexpr int COLOR_INVALID = 0xaaaaaa;
	// 形状
	SHAPE shape_;
	// 衝突種別
	TAG tag_;
	// 有効フラグ
	bool isValid_;
	// デバッグ用描画
	virtual void DrawDebug(int color) = 0;
};