#include "ResourceBase.h"

ResourceBase::ResourceBase(TYPE type, const std::wstring& path)
	: handleId_(-1)
	, type_(type)
	, path_(path)
{
}
