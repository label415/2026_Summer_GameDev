#pragma once
#include <vector>
#include "ResourceBase.h"

class ResourceModel : public ResourceBase
{
public:

	// コンストラクタ
	ResourceModel(const std::wstring& path);

	// デストラクタ
	~ResourceModel(void) override = default;

	// 読み込み
	void Load(void) override;

	// 解放
	void Release(void) override;

	// モデルの複製
	int Duplicate(void);

private:

	// 複製済みモデルハンドル一覧
	std::vector<int> duplicateModelIds_;

};
