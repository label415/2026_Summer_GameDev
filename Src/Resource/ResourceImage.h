#pragma once
#include "ResourceBase.h"

class ResourceImage : public ResourceBase
{
public:

	// コンストラクタ
	ResourceImage(const std::wstring& path);

	// デストラクタ
	~ResourceImage(void) override = default;

	// 読み込み
	void Load(void) override;

	// 解放
	void Release(void) override;

};
