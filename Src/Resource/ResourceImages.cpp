#include <DxLib.h>
#include "ResourceImages.h"

ResourceImages::ResourceImages(const std::wstring& path, const Vector2& num, const Vector2& size)
	: ResourceBase(TYPE::IMGS, path)
	, handleIds_(nullptr)
	, num_(num)
	, size_(size)
{
}

void ResourceImages::Load(void)
{
	// 分割数分のハンドル配列を確保して分割読み込みを実行
	handleIds_ = new int[num_.x * num_.y];
	LoadDivGraph(
		path_.c_str(),
		num_.x * num_.y,
		num_.x, num_.y,
		size_.x, size_.y,
		&handleIds_[0]);
}

void ResourceImages::Release(void)
{
	// 未確保の場合は処理しない
	if (handleIds_ == nullptr)
	{
		return;
	}

	// 全分割画像ハンドルを解放して配列を破棄
	int num = num_.x * num_.y;
	for (int i = 0; i < num; i++)
	{
		DeleteGraph(handleIds_[i]);
	}
	delete[] handleIds_;
	handleIds_ = nullptr;
	handleId_ = -1;
}

void ResourceImages::CopyHandle(int* imgs) const
{
	// 未読み込みの場合はコピーしない
	if (handleIds_ == nullptr)
	{
		return;
	}

	// 保持している全画像ハンドルを呼び出し先配列へコピー
	int num = num_.x * num_.y;
	for (int i = 0; i < num; i++)
	{
		imgs[i] = handleIds_[i];
	}
}
