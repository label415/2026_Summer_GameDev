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
    // カプセル同士の当たり判定
    static bool IsHit(
        const ColliderCapsule& capsule1,
        const ColliderCapsule& capsule2);

    // カプセルと球体の当たり判定
    static bool IsHit(
        const ColliderCapsule& capsule,
        const ColliderSphere& sphere);

    // カプセルとモデルの当たり判定
    static bool IsHit(
        const ColliderCapsule& capsule,
        const ColliderModel& model,
        bool isExclude = false,
        bool isTarget = false);

    // 球体とモデルの当たり判定
    static bool IsHit(
        const ColliderSphere& sphere,
        const ColliderModel& model,
        bool isExclude = false,
        bool isTarget = false);

    // ラインとモデルの当たり判定
    static bool IsHit(
        const ColliderLine& line,
        const ColliderModel& model,
        bool isExclude = false,
        bool isTarget = false);

    // モデルフレーム判定
    static bool IsValidPoly(
        const ColliderModel& model,
        int frameIdx,
        bool isExclude,
        bool isTarget);

    // 押し出し量計算処理
    // カプセル同士の押し出し計算
    static VECTOR CalcPushCapsuleCapsule(
        const ColliderCapsule& a,
        const ColliderCapsule& b, 
        float distance = 0.5f);

    // カプセルの複数ポリゴンからの総押し戻しベクトル計算
    static VECTOR CalcPushCapsuleModel(
        const ColliderCapsule& capsule,
        const ColliderModel& model,
        const VECTOR& pos,
        int maxTryCnt = 5,
        float pushDistance = 0.5f,
        bool isExclude = false,
        bool isTarget = false);

    static VECTOR CalcPushSphereModel(
        const ColliderSphere& sphere,
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

    static VECTOR CalcCameraPushBack(
        const ColliderSphere& sphere,
        const ColliderModel& model,
        const VECTOR& startPos,
        const VECTOR& cameraPos,
        int maxTryCnt = 10,
        float pushDistance = 2.0f,
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

    // 球体の法線方向への押し戻しベクトル計算
    static VECTOR CalcPushSphereTriangle(
        const ColliderSphere& sphere,
        const MV1_COLL_RESULT_POLY& poly,
        const VECTOR& pos,
        int maxTryCnt = 5,
        float pushDistance = 0.5f);

    static bool TryGetNearestHitPolyLineModel(
        const ColliderModel& model,
        const VECTOR& startPos,
        const VECTOR& endPos,
        MV1_COLL_RESULT_POLY& outHitPoly,
        bool isExclude = false,
        bool isTarget = false);


    // コライダーの線分上の最近接点を取得
    static VECTOR GetNearestPointOnSegment(const VECTOR& statePos,
        const VECTOR& endPos, const VECTOR& targetPos);
};