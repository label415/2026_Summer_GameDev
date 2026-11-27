#pragma once
#include "ResourceBase.h"

class ResourceSound : public ResourceBase
{
public:

	// コンストラクタ
	ResourceSound(const std::wstring& path);

	// デストラクタ
	~ResourceSound(void) override = default;

	// 読み込み
	void Load(void) override;

	// 解放
	void Release(void) override;

};
