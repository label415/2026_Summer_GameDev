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

    /// <summary>
    /// カプセル同士の当たり判定を行う
    /// </summary>
    /// <param name="capsule1">1番目のカプセルコライダー</param>
    /// <param name="capsule2">2番目のカプセルコライダー</param>
    /// <returns>当たっている場合true、当たっていない場合false</returns>
    static bool IsHit(
        const ColliderCapsule& capsule1,
        const ColliderCapsule& capsule2);

    /// <summary>
    /// カプセルと球体の当たり判定を行う
    /// </summary>
    /// <param name="capsule">カプセルコライダー</param>
    /// <param name="sphere">球体コライダー</param>
    /// <returns>当たっている場合true、当たっていない場合false</returns>
    static bool IsHit(
        const ColliderCapsule& capsule,
        const ColliderSphere& sphere);

    /// <summary>
    /// カプセルとモデルの当たり判定を行う
    /// </summary>
    /// <param name="capsule">カプセルコライダー</param>
    /// <param name="model">モデルコライダー</param>
    /// <param name="isExclude">除外ポリゴンフラグ</param>
    /// <param name="isTarget">対象ポリゴンフラグ</param>
    /// <returns>当たっている場合true、当たっていない場合false</returns>
    static bool IsHit(
        const ColliderCapsule& capsule,
        const ColliderModel& model,
        bool isExclude = false,
        bool isTarget = false);

    /// <summary>
    /// 球体とモデルの当たり判定を行う
    /// </summary>
    /// <param name="sphere">球体コライダー</param>
    /// <param name="model">モデルコライダー</param>
    /// <param name="isExclude">除外ポリゴンフラグ</param>
    /// <param name="isTarget">対象ポリゴンフラグ</param>
    /// <returns>当たっている場合true、当たっていない場合false</returns>
    static bool IsHit(
        const ColliderSphere& sphere,
        const ColliderModel& model,
        bool isExclude = false,
        bool isTarget = false);

    /// <summary>
    /// ラインとモデルの当たり判定を行う
    /// </summary>
    /// <param name="line">ラインコライダー</param>
    /// <param name="model">モデルコライダー</param>
    /// <param name="isExclude">除外ポリゴンフラグ</param>
    /// <param name="isTarget">対象ポリゴンフラグ</param>
    /// <returns>当たっている場合true、当たっていない場合false</returns>
    static bool IsHit(
        const ColliderLine& line,
        const ColliderModel& model,
        bool isExclude = false,
        bool isTarget = false);

    /// <summary>
    /// モデルのフレーム内のポリゴンが有効かどうかを判定する
    /// </summary>
    /// <param name="model">モデルコライダー</param>
    /// <param name="frameIdx">フレームインデックス</param>
    /// <param name="isExclude">除外ポリゴンフラグ</param>
    /// <param name="isTarget">対象ポリゴンフラグ</param>
    /// <returns>有効なポリゴンの場合true、無効な場合false</returns>
    static bool IsValidPoly(
        const ColliderModel& model,
        int frameIdx,
        bool isExclude,
        bool isTarget);

    // 押し出し量計算処理
    /// <summary>
    /// カプセル同士の押し出しベクトルを計算する
    /// </summary>
    /// <param name="a">1番目のカプセルコライダー</param>
    /// <param name="b">2番目のカプセルコライダー</param>
    /// <param name="distance">押し出し距離（デフォルト値：0.5f）</param>
    /// <returns>計算された押し出しベクトル</returns>
    static VECTOR CalcPushCapsuleCapsule(
        const ColliderCapsule& a,
        const ColliderCapsule& b, 
        float distance = 0.5f);

    /// <summary>
    /// カプセルがモデルの複数ポリゴンから受ける総押し戻しベクトルを計算する
    /// </summary>
    /// <param name="capsule">カプセルコライダー</param>
    /// <param name="model">モデルコライダー</param>
    /// <param name="pos">基準位置</param>
    /// <param name="maxTryCnt">最大試行回数（デフォルト値：5）</param>
    /// <param name="pushDistance">押し出し距離（デフォルト値：0.5f）</param>
    /// <param name="isExclude">除外ポリゴンフラグ</param>
    /// <param name="isTarget">対象ポリゴンフラグ</param>
    /// <returns>計算された押し出しベクトル</returns>
    static VECTOR CalcPushCapsuleModel(
        const ColliderCapsule& capsule,
        const ColliderModel& model,
        const VECTOR& pos,
        int maxTryCnt = 5,
        float pushDistance = 0.5f,
        bool isExclude = false,
        bool isTarget = false);

    /// <summary>
    /// 球体がモデルから受ける総押し戻しベクトルを計算する
    /// </summary>
    /// <param name="sphere">球体コライダー</param>
    /// <param name="model">モデルコライダー</param>
    /// <param name="pos">基準位置</param>
    /// <param name="maxTryCnt">最大試行回数（デフォルト値：5）</param>
    /// <param name="pushDistance">押し出し距離（デフォルト値：0.5f）</param>
    /// <param name="isExclude">除外ポリゴンフラグ</param>
    /// <param name="isTarget">対象ポリゴンフラグ</param>
    /// <returns>計算された押し出しベクトル</returns>
    static VECTOR CalcPushSphereModel(
        const ColliderSphere& sphere,
        const ColliderModel& model,
        const VECTOR& pos,
        int maxTryCnt = 5,
        float pushDistance = 0.5f,
        bool isExclude = false,
        bool isTarget = false);

    /// <summary>
    /// ラインがモデルから受ける上方向への押し上げ量を計算する
    /// </summary>
    /// <param name="line">ラインコライダー</param>
    /// <param name="model">モデルコライダー</param>
    /// <param name="currentPosY">現在のY座標</param>
    /// <param name="pushDistance">押し出し距離（デフォルト値：0.0f）</param>
    /// <param name="isExclude">除外ポリゴンフラグ</param>
    /// <param name="isTarget">対象ポリゴンフラグ</param>
    /// <returns>計算された押し出しベクトル</returns>
    static VECTOR CalcPushUpLineModel(
        const ColliderLine& line,
        const ColliderModel& model,
        const float& currentPosY,
        float pushDistance = 0.0f,
        bool isExclude = false,
        bool isTarget = false);

    /// <summary>
    /// カメラがモデルに衝突しない後ろに押し出すベクトルを計算する
    /// </summary>
    /// <param name="sphere">球体コライダー（カメラの衝突判定用）</param>
    /// <param name="model">モデルコライダー</param>
    /// <param name="startPos">開始位置</param>
    /// <param name="cameraPos">カメラ位置</param>
    /// <param name="maxTryCnt">最大試行回数（デフォルト値：10）</param>
    /// <param name="pushDistance">押し出し距離（デフォルト値：2.0f）</param>
    /// <param name="isExclude">除外ポリゴンフラグ</param>
    /// <param name="isTarget">対象ポリゴンフラグ</param>
    /// <returns>計算された押し出しベクトル</returns>
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
    /// <summary>
    /// カプセルが三角ポリゴンから受ける法線方向への押し戻しベクトルを計算する
    /// </summary>
    /// <param name="capsule">カプセルコライダー</param>
    /// <param name="poly">三角ポリゴンの衝突結果</param>
    /// <param name="pos">基準位置</param>
    /// <param name="maxTryCnt">最大試行回数（デフォルト値：5）</param>
    /// <param name="pushDistance">押し出し距離（デフォルト値：0.5f）</param>
    /// <returns>計算された押し出しベクトル</returns>
    static VECTOR CalcPushCapsuleTriangle(
        const ColliderCapsule& capsule,
        const MV1_COLL_RESULT_POLY& poly,
        const VECTOR& pos,
        int maxTryCnt = 5,
        float pushDistance = 0.5f);

    /// <summary>
    /// 球体が三角ポリゴンから受ける法線方向への押し戻しベクトルを計算する
    /// </summary>
    /// <param name="sphere">球体コライダー</param>
    /// <param name="poly">三角ポリゴンの衝突結果</param>
    /// <param name="pos">基準位置</param>
    /// <param name="maxTryCnt">最大試行回数（デフォルト値：5）</param>
    /// <param name="pushDistance">押し出し距離（デフォルト値：0.5f）</param>
    /// <returns>計算された押し出しベクトル</returns>
    static VECTOR CalcPushSphereTriangle(
        const ColliderSphere& sphere,
        const MV1_COLL_RESULT_POLY& poly,
        const VECTOR& pos,
        int maxTryCnt = 5,
        float pushDistance = 0.5f);

    /// <summary>
    /// ラインがモデルのポリゴンと交差する最近接ポリゴンを取得する
    /// </summary>
    /// <param name="model">モデルコライダー</param>
    /// <param name="startPos">線分の開始位置</param>
    /// <param name="endPos">線分の終了位置</param>
    /// <param name="outHitPoly">交差したポリゴンの情報（出力）</param>
    /// <param name="isExclude">除外ポリゴンフラグ</param>
    /// <param name="isTarget">対象ポリゴンフラグ</param>
    /// <returns>ポリゴンが見つかった場合true、見つからない場合false</returns>
    static bool TryGetNearestHitPolyLineModel(
        const ColliderModel& model,
        const VECTOR& startPos,
        const VECTOR& endPos,
        MV1_COLL_RESULT_POLY& outHitPoly,
        bool isExclude = false,
        bool isTarget = false);

    /// <summary>
    /// 線分上の指定位置に最も近い点を取得する
    /// </summary>
    /// <param name="statePos">線分の開始位置</param>
    /// <param name="endPos">線分の終了位置</param>
    /// <param name="targetPos">比較対象位置</param>
    /// <returns>線分上の最近接点</returns>
    static VECTOR GetNearestPointOnSegment(const VECTOR& statePos,
        const VECTOR& endPos, const VECTOR& targetPos);

};