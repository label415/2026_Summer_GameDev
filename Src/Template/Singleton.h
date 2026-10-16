#pragma once
#include <memory>
#include <cassert>

template <typename T>
class Singleton
{
public:
    // インスタンスの生成
    static void CreateInstance()
    {
        if (!instance_)
        {
            instance_ = new T();
        }
    }

    // インスタンスへのアクセス
    static T& GetInstance()
    {
        assert(instance_ != nullptr 
            && "シングルトンクラスが生成されていない");
        return *instance_;
    }

	// 解放処理
	static void Destroy()
	{
		if (instance_)
		{
            delete instance_;
            instance_ = nullptr;
		}
	}   

    // コピーコンストラクタを削除
    Singleton(const Singleton&) = delete;

    // コピー代入演算子を削除
    Singleton& operator=(const Singleton&) = delete;
protected:
    // デフォルトコンストラクタは protected にして外部生成を防止
    Singleton() = default;

    // デストラクタも protected
    virtual ~Singleton() = default;
private:
    //静的メンバ初期化
    inline static T* instance_ = nullptr;
};