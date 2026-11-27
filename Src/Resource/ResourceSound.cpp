#include <DxLib.h>
#include "ResourceSound.h"

ResourceSound::ResourceSound(const std::wstring& path)
	: ResourceBase(TYPE::SOUND, path)
{
}

void ResourceSound::Load(void)
{
	// サウンドファイルをメモリへ読み込みハンドルを取得
	handleId_ = LoadSoundMem(path_.c_str());
}

void ResourceSound::Release(void)
{
	// 読み込み済みのサウンドハンドルが存在すれば解放
	if (handleId_ != -1)
	{
		DeleteSoundMem(handleId_);
		handleId_ = -1;
	}
}
