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

	// 読み込み
	void Load(void);

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);

	// エネミー
	const std::vector<EnemyBase*>& GetEnemys(void) const { return enemys_; }

	// CSVから敵情報の読取を行う
	void LoadCsvData(void);

	// エネミー生成
	EnemyBase* Create(const EnemyBase::EnemyData& data);
private:
	// エネミー
	std::vector<EnemyBase*> enemys_;
};

