#pragma once
#include <string>
#include <map>

class FontManager 
{
public:

	// インスタンスを明示的に生成
	static void CreateInstance(void);

	// インスタンスの取得
	static FontManager& GetInstance(void);

	/// <summary>
	/// オリジナルフォントの生成
	/// </summary>
	/// <param name="fontName">フォント名前</param>
	/// <param name="size">サイズ</param>
	/// <param name="thickness">太さ</param>
	/// <returns>生成できた場合true,失敗した場合false</returns>
	int CreateMyFont(const std::wstring& fontName, const int size, const int thickness);

	// リソースの破棄
	void Destroy(void);

private:

	// 静的インスタンス
	static FontManager* instance_;

	// フォント設定情報をまとめた構造体
	struct FontKey
	{
		std::wstring fontName;
		int size;
		int thickness;

		// map用の比較演算子
		bool operator<(const FontKey& other) const noexcept
		{
			if (fontName != other.fontName) return fontName < other.fontName;
			if (size != other.size) return size < other.size;
			return thickness < other.thickness;
		}
	};

	//生成したフォントを管理するマップ
	std::map<FontKey, int> fontHandleMap_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	FontManager(void);
	FontManager(const FontManager& manager) = default;
	~FontManager(void) = default;
};