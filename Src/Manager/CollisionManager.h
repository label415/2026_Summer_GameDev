#pragma once
#include "../Object/Common/Collider/ColliderBase.h"
#include <vector>
#include <map>

class CollisionManager
{
public:
	// インスタンスを明示的に生成
	static void CreateInstance(void);

	// インスタンスの取得
	static CollisionManager& GetInstance(void);

private:

	// 静的インスタンス
	static CollisionManager* instance_;

	// コライダーリスト
	std::map<int, std::vector<ColliderBase*>> colliders_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	CollisionManager(void);
	CollisionManager(const CollisionManager& manager) = default;
	~CollisionManager(void) = default;
};

