#pragma once
#include "../Template/Vector2Template.h"
#include "ResourceBase.h"

class ResourceImages : public ResourceBase
{
public:

	// コンストラクタ
	ResourceImages(const std::wstring& path, const Vector2& num, const Vector2& size);

	// デストラクタ
	~ResourceImages(void) override = default;

	// 読み込み
	void Load(void) override;

	// 解放
	void Release(void) override;

	// 複数画像ハンドルを別配列にコピー
	void CopyHandle(int* imgs) const;

private:

	// 分割画像ハンドル配列
	int* handleIds_;

	// 分割数・1枚あたりのサイズ
	Vector2 num_;
	Vector2 size_;

};
