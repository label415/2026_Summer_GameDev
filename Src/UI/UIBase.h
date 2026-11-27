#pragma once
#include "../Template/Vector2Template.h"

class UIBase
{
public:

	// コンストラクタ
	UIBase(int img, Vector2F pos, Vector2F size);

	// デストラクタ
	virtual ~UIBase(void) = default;

	// 更新
	virtual void Update(void) = 0;

	// 描画
	virtual void Draw(void) = 0;

	// 解放
	virtual void Release(void) {}

	// 有効・無効判定
	bool IsActive(void) const { return active_; }

	// 有効・無効設定
	void SetActive(bool active) { active_ = active; }

	// 画像ハンドル設定
	void SetImg(int img) { img_ = img; }

	// 画像ハンドル取得
	int GetImg(void) const { return img_; }

	// 座標設定
	void SetPos(Vector2F pos) { pos_ = pos; }

	// 座標取得
	Vector2F GetPos(void) const { return pos_; }

	// 大きさ設定
	void SetSize(Vector2F size) { size_ = size; }

	// 大きさ取得
	Vector2F GetSize(void) const { return size_; }

protected:

	// 画像のハンドルID
	int img_;

	// 座標
	Vector2F pos_;

	// 大きさ
	Vector2F size_;

	// 有効・無効
	bool active_;

};
