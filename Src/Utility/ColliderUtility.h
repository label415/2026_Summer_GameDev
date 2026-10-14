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
        const ColliderSphere& sphere);

    static bool IsHit(
        const ColliderCapsule& capsule,
        const ColliderModel& model,
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

    // カプセルの複数ポリゴンからの総押し戻しベクトル計算
    static VECTOR CalcPushCapsuleModel(
        const ColliderCapsule& capsule,
        const ColliderModel& model,
        const VECTOR& pos,
        int maxTryCnt = 5,
        float pushDistance = 0.5f,
        bool isExclude = false,
        bool isTarget = false);

    // 線分の上方向押し上げ量計算
    static VECTOR CalcPushUpLineModel(
        const ColliderLine& line,
        const ColliderModel& model,
        const float& currentPosY,
        float pushDistance = 0.0f,
        bool isExclude = false,
        bool isTarget = false);
private:
    // カプセルの法線方向への押し戻しベクトル計算
    static VECTOR CalcPushCapsuleTriangle(
        const ColliderCapsule& capsule,
        const MV1_COLL_RESULT_POLY& poly,
        const VECTOR& pos,
        int maxTryCnt = 5,
        float pushDistance = 0.5f);

    // コライダーの線分上の最近接点を取得
    static VECTOR GetNearestPointOnSegment(const VECTOR& statePos,
        const VECTOR& endPos, const VECTOR& targetPos);
};