#pragma once
#include <string>

class ResourceBase
{
public:

	// リソースタイプ
	enum class TYPE
	{
		NONE,
		IMG,
		IMGS,
		MODEL,
		FONT,
		EFFEKSEER,
		SOUND,
	};

	// コンストラクタ
	ResourceBase(TYPE type, const std::wstring& path);

	// デストラクタ
	virtual ~ResourceBase(void) = default;

	// 読み込み
	virtual void Load(void) = 0;

	// 解放
	virtual void Release(void) = 0;

	// リソースタイプの取得
	TYPE GetType(void) const { return type_; }

	// リソースの読み込み先パス
	const std::wstring& GetPath(void) const { return path_; }

	// 各種ハンドルの取得用
	int handleId_;

protected:

	// リソースタイプ
	TYPE type_;

	// リソースの読み込み先
	std::wstring path_;

};
