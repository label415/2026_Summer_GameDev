#include <DxLib.h>
#include <cassert>
#include "../Application.h"
#include "FontManager.h"

void FontManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new FontManager();
	}
}

FontManager& FontManager::GetInstance(void)
{
	return *instance_;
}

int FontManager::CreateMyFont(const std::wstring& fontName, const int size, const int thickness)
{
	FontKey key{ fontName, size, thickness };  // キー

	auto it = fontHandleMap_.find(key);        // 既存検索
	if (it != fontHandleMap_.end())
	{
		return it->second;  // 既存を返す
	}

	// 生成
	int handle = CreateFontToHandle(
		fontName.c_str(), // フォント名
		size,
		thickness);

	assert(handle != -1 && L"フォント生成に失敗しました");

	fontHandleMap_[key] = handle;                    // 登録
	return handle;                                   // 生成結果
}

void FontManager::Destroy(void)
{
	for (const auto& pair : fontHandleMap_)
	{
		DeleteFontToHandle(pair.second);  // フォントハンドルの削除
	}
	fontHandleMap_.clear();  // マップのクリア
	delete instance_;        // インスタンスの削除
	instance_ = nullptr;     // ポインタをnullptrに設定
}

FontManager::FontManager(void)
{
	fontHandleMap_.clear();
}
