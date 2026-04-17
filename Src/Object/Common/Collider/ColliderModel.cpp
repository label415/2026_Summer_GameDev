#include "../../Common/Transform.h"
#include "ColliderModel.h"
ColliderModel::ColliderModel(TAG tag, const Transform* follow)
	:
	ColliderBase(SHAPE::MODEL, tag, follow)
{
}

ColliderModel::~ColliderModel(void)
{
}

void ColliderModel::AddExcludeFrameIds(const std::wstring& name)
{
	// フレーム数を取得
	int num = MV1GetFrameNum(follow_->modelId);
	for (int i = 0; i < num; i++)
	{
		// フレーム名称を取得
		std::wstring frameName = MV1GetFrameName(follow_->modelId, i);
		if (frameName.find(name) != std::wstring::npos)
		{
			// 除外フレームに追加
			excludeFrameIds_.push_back(i);
		}
	}
}

void ColliderModel::ClearExcludeFrame(void)
{
	excludeFrameIds_.clear();
}

bool ColliderModel::IsExcludeFrame(int frameIdx) const
{
	// 除外判定
	if (std::find(
		excludeFrameIds_.begin(),
		excludeFrameIds_.end(),
		frameIdx) != excludeFrameIds_.end())
	{
		// 除外に該当する
		return true;
	}
	return false;
}

void ColliderModel::AddTargetFrameIds(const std::wstring& name)
{
	// フレーム数を取得
	int num = MV1GetFrameNum(follow_->modelId);
	for (int i = 0; i < num; i++)
	{
		// フレーム名称を取得
		std::wstring frameName = MV1GetFrameName(follow_->modelId, i);
		if (frameName.find(name) != std::wstring::npos)
		{
			// 除外フレームに追加
			targetFrameIds_.push_back(i);
		}
	}
}

void ColliderModel::ClearTargetFrame(void)
{
	targetFrameIds_.clear();
}

bool ColliderModel::IsTargetFrame(int frameIdx) const
{
	// 除外判定
	if (std::find(
		targetFrameIds_.begin(),
		targetFrameIds_.end(),
		frameIdx) != targetFrameIds_.end())
	{
		// 除外に該当する
		return true;
	}
	return false;
}
