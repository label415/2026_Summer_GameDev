#include <DxLib.h>
#include "ResourceModel.h"

ResourceModel::ResourceModel(const std::wstring& path)
	: ResourceBase(TYPE::MODEL, path)
{
}

void ResourceModel::Load(void)
{
	// 3Dモデルファイルを読み込みハンドルを取得
	handleId_ = MV1LoadModel(path_.c_str());
}

void ResourceModel::Release(void)
{
	// 元モデルハンドルが存在すれば解放
	if (handleId_ != -1)
	{
		MV1DeleteModel(handleId_);
		handleId_ = -1;
	}

	// 複製したモデルハンドルをすべて解放して一覧をクリア
	for (int id : duplicateModelIds_)
	{
		MV1DeleteModel(id);
	}
	duplicateModelIds_.clear();
}

int ResourceModel::Duplicate(void)
{
	// 元モデルが未読み込みなら無効値を返す
	if (handleId_ == -1)
	{
		return -1;
	}

	// 非同期読み込み中のモデルは複製できないため、読み込み完了を待つ
	if (CheckHandleASyncLoad(handleId_) == TRUE)
	{
		WaitHandleASyncLoad(handleId_);
	}

	// モデルを複製して解放管理用リストへ登録
	int duId = MV1DuplicateModel(handleId_);
	duplicateModelIds_.push_back(duId);
	return duId;
}
