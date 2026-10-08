#pragma once
#include <map>
#include <memory>
#include <vector>
#include "../Common/Transform.h"
#include "../Common/EffectController.h"
#include "../Common/Collider/ColliderBase.h"

class ResourceManager;
class SceneManager;
class EffectController;

class ActorBase
{
public:
	// コンストラクタ
	ActorBase(void);

	// デストラクタ
	virtual ~ActorBase(void);

	//読み込み
	void Load(void);

	// 初期化
	void Init(void);

	// 更新
	virtual void Update(void) = 0;

	// 描画
	virtual void Draw(void);

	// 解放
	virtual void Release(void);

	// 大きさ、回転、座標等の取得
	const Transform& GetTransform(void) const;

	//生存フラグ取得
	const bool GetIsAlive(void)const { return isAlive_; }
protected:
	// 生存フラグ
	bool isAlive_;

	// シングルトン参照
	ResourceManager& resMng_;

	SceneManager& scnMng_;

	// モデル制御の基本情報
	Transform transform_;

	//エフェクトコントローラ
	std::unique_ptr<EffectController> effect_;

	// 所有しているコライダー
	std::map<int, std::vector<std::shared_ptr<ColliderBase>>> ownColliders_;

	// リソースロード
	virtual void InitLoad(void) = 0;

	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void) = 0;

	// 衝突判定の初期化
	virtual void InitCollider(void) = 0;

	// アニメーションの初期化
	virtual void InitAnimation(void) = 0;

	// 初期化後の個別処理
	virtual void InitPost(void) = 0;

	// 当たり判定衝突時の更新処理
	virtual void Collision(void);
	virtual void UpdateHitCollider(void);
};
