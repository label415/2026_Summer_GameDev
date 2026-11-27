#pragma once
#include <map>
#include <utility>
#include "ResourceBase.h"

class ResourceFont : public ResourceBase
{
public:

	// デフォルトフォント名
	static constexpr const wchar_t* DEFAULT_FONT_NAME = L"KazukiReiwa";

	// コンストラクタ
	ResourceFont(const std::wstring& path, const std::wstring& fontName = DEFAULT_FONT_NAME);

	// デストラクタ
	~ResourceFont(void) override = default;

	// フォントリソース読み込み
	void Load(void) override;

	// フォントハンドル生成（サイズ・太さ指定）
	int Load(int size, int thickness);

	// フォントリソース解放
	void Release(void) override;

private:

	// フォントファミリー名
	std::wstring fontName_;

	// フォント登録済みフラグ
	bool isLoaded_;

	// 生成済みフォントハンドル一覧（サイズ・太さ -> ハンドル）
	std::map<std::pair<int, int>, int> fontHandles_;

};
