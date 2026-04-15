#pragma once

#include <vector>

#include "../SceneBase.h"

class Camera;
class Stage;
class ActorBase;

class GameScene : public SceneBase
{
public:
	GameScene(void);				// コンストラクタ
	~GameScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:
	// カメラ
	Camera* camera_;

	// ステージ
	Stage* stage_;

	// 全てのアクター
	std::vector<ActorBase*> allActor_;

	// 衝突判定(床)
	void FieldCollision(ActorBase* actor);

	// 衝突判定(壁)
	void WallCollision(ActorBase* actor);
};
