#include <cassert>
#include <DxLib.h>
#include "ResourceFont.h"

ResourceFont::ResourceFont(const std::wstring& path, const std::wstring& fontName)
	: ResourceBase(TYPE::FONT, path)
	, fontName_(fontName)
	, isLoaded_(false)
{
}

void ResourceFont::Load(void)
{
	// 既にアプリ内フォントとして登録済みなら何もしない
	if (isLoaded_)
	{
		return;
	}

	// プロセス限定のプライベートフォントとして登録
	if (AddFontResourceEx(path_.c_str(), FR_PRIVATE, NULL) > 0)
	{
		isLoaded_ = true;
	}
}

int ResourceFont::Load(int size, int thickness)
{
	// 先にフォントリソース自体の登録を完了させておく
	Load();

	// 同一サイズ・太さの生成済みハンドルがあれば再利用
	const auto key = std::make_pair(size, thickness);
	const auto it = fontHandles_.find(key);
	if (it != fontHandles_.end())
	{
		return it->second;
	}

	// 新規にフォントハンドルを生成してキャッシュへ登録
	int handle = CreateFontToHandle(
		fontName_.c_str(),
		size,
		thickness);

	assert(handle != -1 && L"フォント生成に失敗しました");

	fontHandles_.emplace(key, handle);
	return handle;
}

void ResourceFont::Release(void)
{
	// 生成済みの全フォントハンドルを削除
	for (const auto& pair : fontHandles_)
	{
		if (pair.second != -1)
		{
			DeleteFontToHandle(pair.second);
		}
	}
	fontHandles_.clear();

	// 未登録ならシステムフォント解除は行わない
	if (!isLoaded_)
	{
		return;
	}

	// 登録したプライベートフォントを解除して状態をリセット
	RemoveFontResourceEx(path_.c_str(), FR_PRIVATE, NULL);
	isLoaded_ = false;
	handleId_ = -1;
}
