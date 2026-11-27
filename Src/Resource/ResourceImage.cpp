#include <DxLib.h>
#include "ResourceImage.h"

ResourceImage::ResourceImage(const std::wstring& path)
	: ResourceBase(TYPE::IMG, path)
{
	// 単一画像リソースとして基底クラスを初期化
}

void ResourceImage::Load(void)
{
	// 画像ファイルを読み込みハンドルを取得
	handleId_ = LoadGraph(path_.c_str());
}

void ResourceImage::Release(void)
{
	// 読み込み済みの画像ハンドルが存在すれば解放
	if (handleId_ != -1)
	{
		DeleteGraph(handleId_);
		handleId_ = -1;
	}
}
