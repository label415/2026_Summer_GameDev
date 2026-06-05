#pragma once
#include <vector>
#include "EnemyBase.h"

class EnemyBase;
class ColliderBase;

class EnemyManager
{
public:

	// コンストラクタ
	EnemyManager(void);

	// デストラクタ
	~EnemyManager(void);

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);

	// エネミー
	const std::vector<EnemyBase*>& GetEemies(void) const { return enemys_; }

	// 衝突対象となるコライダを登録
	void AddHitCollider(int shape, const std::vector<ColliderBase*> hitCollider);

	// CSVから敵情報の読取を行う
	void LoadCsvData(void);

	// エネミー生成
	EnemyBase* Create(const EnemyBase::EnemyData& data);

	const std::vector<EnemyBase*>& GetEnemys(void) const;

	void RemoveCollider(ColliderBase::SHAPE shape, ColliderBase::TAG tag);

private:
	// エネミー
	std::vector<EnemyBase*> enemys_;
};

