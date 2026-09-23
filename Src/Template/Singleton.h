#pragma once
#include <memory>
#include <cassert>

// シングルトンテンプレートクラス
template <typename T>
class Singleton
{
public:

    /// <summary>
    /// インスタンスの生成
    /// </summary>
    static void CreateInstance()
    {
        if (!instance_)
        {
            instance_ = new T();
        }
    }

    /// <summary>
    /// インスタンスへのアクセス
    /// </summary>
    /// <returns>インスタンス</returns>
    static T& GetInstance()
    {
        assert(instance_ != nullptr && "シングルトンクラスが生成されていない");
        return *instance_;
    }


	/// <summary>
	/// 解放処理
	/// </summary>
	static void Destroy()
	{
		if (instance_)
		{
			delete instance_;
			instance_ = nullptr;
		}
	}   

    /// <summary>
    /// コピーコンストラクタを削除
    /// </summary>
    /// <param name=""></param>
    Singleton(const Singleton&) = delete;

    /// <summary>
    /// コピー代入演算子を削除
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    Singleton& operator=(const Singleton&) = delete;

protected:

    /// <summary>
    /// デフォルトコンストラクタは protected にして外部生成を防止
    /// </summary>
    Singleton() = default;

    /// <summary>
    /// デストラクタも protected
    /// </summary>
    virtual ~Singleton() = default;

private:

    //静的メンバ初期化
    inline static T* instance_ = nullptr; 

};

