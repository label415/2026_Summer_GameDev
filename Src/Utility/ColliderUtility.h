#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>

class ColliderSphere;
class ColliderCapsule;
class ColliderLine;
class ColliderModel;

class CollisionUtility
{
public:
    // 判定処理
    static bool IsHit(
        const ColliderCapsule& capsule1,
        const ColliderCapsule& capsule2);

    static bool IsHit(
        const ColliderCapsule& capsule,
        const ColliderModel& model,
        MV1_COLL_RESULT_POLY_DIM& outHits,
        bool isExclude = false,
        bool isTarget = false);

    static bool IsHit(
        const ColliderSphere& sphere,
        const ColliderModel& model,
        bool isExclude = false,
        bool isTarget = false);

    static bool IsHit(
        const ColliderLine& line,
        const ColliderModel& model,
        MV1_COLL_RESULT_POLY_DIM& outHits,
        bool isExclude = false,
        bool isTarget = false);

    static bool IsValidPoly(
        const ColliderModel& model,
        int frameIdx,
        bool isExclude,
        bool isTarget);

    // 押し出し量計算処理

    // カプセル同士の押し出し計算
    static VECTOR CalcPushCapsuleCapsule(
        const ColliderCapsule& a,
        const ColliderCapsule& b);

    // カプセルの法線方向への押し戻しベクトル計算
    static VECTOR CalcPushCapsuleTriangle(
        const VECTOR& top,
        const VECTOR& down,
        float radius,
        const MV1_COLL_RESULT_POLY& poly,
        int maxTryCnt = 5,
        float pushDistance = 0.5f);

    // カプセルの複数ポリゴンからの総押し戻しベクトル計算
    static VECTOR CalcPushCapsuleModel(
        const ColliderCapsule& capsule,
        const ColliderModel& model,
        const MV1_COLL_RESULT_POLY_DIM& hits,
        int maxTryCnt = 5,
        float pushDistance = 0.5f,
        bool isExclude = false,
        bool isTarget = false);

    // 線分の上方向押し上げ量計算
    static VECTOR CalcPushUpLineModel(
        float currentPosY,
        const ColliderModel& model,
        const MV1_COLL_RESULT_POLY_DIM& hits,
        float pushDistance = 0.0f,
        bool isExclude = false,
        bool isTarget = false);
};