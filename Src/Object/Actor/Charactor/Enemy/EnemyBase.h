#pragma once
#include <DxLib.h>
#include <functional>
#include "../CharactorBase.h"

class WeponBase;

class EnemyBase :
    public CharactorBase
{
public:
    // 種別
    enum class TYPE
    {
        RAT,
        ROBOT,
        DRAGON,
    };

    // エネミーデータ
    struct EnemyData
    {
		// ハンドル
        int id;
		// 種別
        EnemyBase::TYPE type;
		// HP
        int hp;
		// 初期位置
        VECTOR defaultPos;
		// 移動可能範囲
        float moveRadius;
    };

    // コンストラクタ
    EnemyBase(const EnemyBase::EnemyData& data);

    // デストラクタ
    virtual ~EnemyBase(void) override;

    // 描画
    virtual void Draw(void) override;

    //武器情報取得
    const WeponBase* GetWepon(void)const { return wepon_; }

	// 種別取得
    const int GetState(void)const { return stateBase_; }

	// 死亡演出タイム取得
    const float GetDesath(void)const { return deathAnimationTime_; }
protected:
	// 武器
    WeponBase* wepon_;

    // 状態管理
    int stateBase_;
    // 状態管理(状態遷移時初期処理)
    std::map<int, std::function<void(void)>> stateChanges_;
    // 状態管理(更新ステップ)
    std::function<void(void)> stateUpdate_;

    // リソースロード
    void InitLoad(void) override {}

    // 大きさ、回転、座標の初期化
    void InitTransform(void) override {}

    // 衝突判定の初期化
    void InitCollider(void) override {}

    // アニメーションの初期化
    void InitAnimation(void) override {}

    // 初期化後の個別処理
    void InitPost(void) override {}

    // 巡回ルート座標
    std::vector<VECTOR> wayPoints_;

    // 種別
    TYPE type_;

    // 初期位置
    const VECTOR defaultPos_;

    //移動可能範囲
    float moveRadius_;

    // HP
    int hp_;

	// 死亡アニメーション時間
    float deathAnimationTime_;

    // 状態遷移
    void ChangeState(int state);

    // 更新系
    virtual void UpdateProcessPost(void) override {}

    // 移動可能範囲判定
    bool InMovableRange(void) const;
};
