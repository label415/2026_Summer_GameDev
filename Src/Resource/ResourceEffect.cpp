#include <EffekseerForDXLib.h>
#include "ResourceEffect.h"

ResourceEffect::ResourceEffect(const std::wstring& path)
	: ResourceBase(TYPE::EFFEKSEER, path)
{
}

void ResourceEffect::Load(void)
{
	// エフェクトファイルを読み込みハンドルを取得
	handleId_ = LoadEffekseerEffect(path_.c_str());
}

void ResourceEffect::Release(void)
{
	// 読み込み済みのエフェクトハンドルが存在すれば解放
	if (handleId_ != -1)
	{
		DeleteEffekseerEffect(handleId_);
		handleId_ = -1;
	}
}
