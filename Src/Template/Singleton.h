#pragma once
#include <memory>
#include <cassert>

template <typename T>
class Singleton
{
public:

    // インスタンスの生成
    static void CreateInstance(void)
    {
		// すでにインスタンスが存在する場合は何もしない
        if (!instance_)
        {
            instance_ = new T();
        }
    }

    // インスタンスへのアクセス
    static T& GetInstance(void)
    {
		// インスタンスが生成されていない場合はエラー
        assert(instance_ != nullptr 
            && "シングルトンクラスが生成されていない");
        return *instance_;
    }

	// 解放処理
	static void Destroy(void)
	{
		// インスタンスが存在する場合のみ解放
		if (instance_)
		{
            delete instance_;
            instance_ = nullptr;
		}
	}   

protected:

    // デフォルトコンストラクタは protected にして外部生成を防止
    Singleton(void) = default;

    // コピーコンストラクタを削除
    Singleton(const Singleton&) = delete;

    // デストラクタも protected
    virtual ~Singleton(void) = default;

private:

    //静的メンバ初期化
    inline static T* instance_ = nullptr;

};