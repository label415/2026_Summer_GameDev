#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/AnimationController.h"

class SkyDome;

class TitleScene : public SceneBase
{

public:

	enum class ANIM_TYPE
	{
		RUN,
	};

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	//スカイドーム
	SkyDome* skydome_;

	//アニメーションコントローラ
	AnimationController* anim_;

	// 惑星
	Transform bigPlanet_;
	Transform spherPlanet_;

	//プレイヤー
	Transform player_;

	// 画像読み込み
	int imgTitle_;
	int imgPushSpace_;

};
