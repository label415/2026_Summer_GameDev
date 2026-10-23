#pragma once
#include "../../Common/AnimationController.h"
#include "../../Common/EffectController.h"
#include "../Wepon/WeponBase.h"
#include "../UI/UIHp.h"
#include "../ActorBase.h"

class CharactorBase :
    public ActorBase
{
public:
    // コンストラクタ
    CharactorBase(void);

    // デストラクタ
    virtual ~CharactorBase(void) override;

    // 更新
    virtual void Update(void) override;

    // 描画
    virtual void Draw(void) override;

    // 解放
    virtual void Release(void) override;

	//HP描画
    virtual void DrawHp(void){}

    // 追尾対象の取得
    void SetTargetPos(const VECTOR* targetTrans) { targetPos_ = targetTrans; }
protected:
    // 最大落下速度
    static constexpr float MAX_FALL_SPEED = -30.0f;

    // 衝突時の押し戻し試行回数
    static constexpr int CNT_TRY_COLLISION = 20;
    // 衝突時の押し戻し量
    static constexpr float COLLISION_BACK_DIS = 1.0f;

	//HP UI
	std::unique_ptr<UIHp> uiHp_;

    //アニメーションコントローラ
    std::unique_ptr<AnimationController> animationController_;

    // 武器
    std::vector<std::unique_ptr<WeponBase>> weapons_;

    // 追尾対象のTransform
    const VECTOR* targetPos_;

    // 移動方向
    VECTOR moveDir_;

    // 移動量
    VECTOR movePow_;

    // 移動前の座標
    VECTOR prevPos_;

    // ジャンプ量
    VECTOR jumpPow_;

    // 移動スピード
    float moveSpeed_;

    // ジャンプの入力受付時間
    float stepJump_;

    // ジャンプ判定
    bool isJump_;

    // リソースロード
    virtual void InitLoad(void) override{}

    // 更新系
    virtual void UpdateProcess(void) = 0;
    virtual void UpdateProcessPost(void) = 0;

    // 移動方向に応じた遅延回転
    void DelayRotate(void);

    // 重力計算
    void CalcGravityPow(void);

    // ターゲット方向の取得
    VECTOR GetTargetDir(void);

    // 衝突判定
    void Collision(void)override;
    void CollisionGravity(void);
    virtual void CollisionCapsule(void);

    //キャラクター移動量の無効
    void SetFrameUserLocalPos(VECTOR locakPos, int frameNo);
};

