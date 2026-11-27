#pragma once
#include "ResourceBase.h"

class ResourceEffect : public ResourceBase
{
public:

	// コンストラクタ
	ResourceEffect(const std::wstring& path);

	// デストラクタ
	~ResourceEffect(void) override = default;

	// 読み込み
	void Load(void) override;

	// 解放
	void Release(void) override;

};
