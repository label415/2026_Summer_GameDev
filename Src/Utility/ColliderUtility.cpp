#include "../Object/Common/Collider/ColliderCapsule.h"
#include "../Object/Common/Collider/ColliderSphere.h"
#include "../Object/Common/Collider/ColliderLine.h"
#include "../Object/Common/Collider/ColliderModel.h"
#include "../Object/Common/Transform.h"
#include "AsoUtility.h"
#include "ColliderUtility.h"

bool CollisionUtility::IsHit(
	const ColliderCapsule& capsule1,
	const ColliderCapsule& capsule2)
{
	return HitCheck_Capsule_Capsule(
		capsule2.GetPosTop(), capsule2.GetPosDown(),
		capsule2.GetRadius(),
		capsule1.GetPosTop(), capsule1.GetPosDown(),
		capsule1.GetRadius());
}

bool CollisionUtility::IsHit(
	const ColliderCapsule& capsule,
	const ColliderModel& model,
	MV1_COLL_RESULT_POLY_DIM& outHits,
	bool isExclude, bool isTarget)
{
	auto modelFollow = model.GetFollow().lock();
	if (!modelFollow) return false;

	outHits = MV1CollCheck_Capsule(
		modelFollow->modelId, -1,
		capsule.GetPosTop(),
		capsule.GetPosDown(),
		capsule.GetRadius());

	for (int i = 0; i < outHits.HitNum; i++)
	{
		if (IsValidPoly(
			model,
			outHits.Dim[i].FrameIndex,
			isExclude, isTarget))
		{
			return true;
		}
	}
	return false;
}

bool CollisionUtility::IsHit(
	const ColliderSphere& sphere,
	const ColliderModel& model,
	bool isExclude, bool isTarget)
{
	auto modelFollow = model.GetFollow().lock();
	if (!modelFollow) return false;

	auto hits = MV1CollCheck_Sphere(
		modelFollow->modelId, -1,
		sphere.GetPos(), sphere.GetRadius());

	bool isHit = false;
	for (int i = 0; i < hits.HitNum; i++)
	{
		if (IsValidPoly(
			model,
			hits.Dim[i].FrameIndex,
			isExclude, isTarget))
		{
			isHit = true;
			break;
		}
	}
	MV1CollResultPolyDimTerminate(hits);
	return isHit;
}

bool CollisionUtility::IsHit(
	const ColliderLine& line,
	const ColliderModel& model,
	MV1_COLL_RESULT_POLY_DIM& outHits,
	bool isExclude, bool isTarget)
{
	auto modelFollow = model.GetFollow().lock();
	if (!modelFollow) return false;

	outHits = MV1CollCheck_LineDim(
		modelFollow->modelId, -1,
		line.GetPosStart(), line.GetPosEnd());

	for (int i = 0; i < outHits.HitNum; i++)
	{
		if (IsValidPoly(
			model,
			outHits.Dim[i].FrameIndex,
			isExclude, isTarget))
		{
			return true;
		}
	}
	return false;
}

bool CollisionUtility::IsValidPoly(
	const ColliderModel& model,
	int frameIdx, bool isExclude, bool isTarget)
{
	if (isExclude && model.IsExcludeFrame(frameIdx)) return false;
	if (isTarget && !model.IsTargetFrame(frameIdx)) return false;
	return true;
}

VECTOR CollisionUtility::CalcPushCapsuleCapsule(
	const ColliderCapsule& a,
	const ColliderCapsule& b)
{
	VECTOR targetPos = b.GetFollow().lock() ? b.GetFollow().lock()->pos : b.GetPosDown();
	VECTOR p1 = AsoUtility::GetNearestPointOnSegment(
		a.GetPosTop(), a.GetPosDown(), targetPos);
	VECTOR p2 = AsoUtility::GetNearestPointOnSegment(
		b.GetPosTop(), b.GetPosDown(), p1);

	VECTOR vBA = VSub(p1, p2);
	float distance = VSize(vBA);
	float totalRadius = a.GetRadius() + b.GetRadius();

	if (distance >= totalRadius)
	{
		return AsoUtility::VECTOR_ZERO;
	}

	if (distance < 1e-6f)
	{
		vBA = AsoUtility::DIR_R;
		distance = 1e-6f;
	}

	float overlap = totalRadius - distance;
	VECTOR pushDir = VNorm(vBA);
	pushDir.y = 0.0f;

	return VScale(pushDir, overlap);
}

VECTOR CollisionUtility::CalcPushCapsuleTriangle(
	const VECTOR& top,
	const VECTOR& down,
	float radius, const MV1_COLL_RESULT_POLY& poly,
	int maxTryCnt, float pushDistance)
{
	VECTOR push = AsoUtility::VECTOR_ZERO;
	VECTOR curTop = top;
	VECTOR curDown = down;

	for (int tryCnt = 0; tryCnt < maxTryCnt; ++tryCnt)
	{
		if (!HitCheck_Capsule_Triangle(
			curTop, curDown, radius,
			poly.Position[0], poly.Position[1], poly.Position[2]))
		{
			break;
		}

		VECTOR step = VScale(poly.Normal, pushDistance);
		push = VAdd(push, step);
		curTop = VAdd(curTop, step);
		curDown = VAdd(curDown, step);
	}
	return push;
}

VECTOR CollisionUtility::CalcPushCapsuleModel(
	const ColliderCapsule& capsule,
	const ColliderModel& model,
	const MV1_COLL_RESULT_POLY_DIM& hits,
	int maxTryCnt, float pushDistance, bool isExclude, bool isTarget)
{
	VECTOR totalPush = AsoUtility::VECTOR_ZERO;

	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hitPoly = hits.Dim[i];
		if (!IsValidPoly(
			model, hitPoly.FrameIndex,
			isExclude, isTarget)) continue;

		// ’~Ï‚³‚ê‚½‰Ÿ‚µ–ß‚µ—Ê‚ð‰Á–¡‚µ‚½Œ»ÝˆÊ’u‚ÅŒvŽZ
		VECTOR curTop = VAdd(capsule.GetPosTop(), totalPush);
		VECTOR curDown = VAdd(capsule.GetPosDown(), totalPush);

		VECTOR push = CalcPushCapsuleTriangle(
			curTop, curDown,
			capsule.GetRadius(),
			hitPoly, maxTryCnt, pushDistance);

		totalPush = VAdd(totalPush, push);
	}
	return totalPush;
}

VECTOR CollisionUtility::CalcPushUpLineModel(
	float currentPosY,
	const ColliderModel& model,
	const MV1_COLL_RESULT_POLY_DIM& hits,
	float pushDistance, bool isExclude, bool isTarget)
{
	float maxDiffY = 0.0f;

	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hit = hits.Dim[i];
		if (!IsValidPoly(
			model, hit.FrameIndex,
			isExclude, isTarget)) continue;

		float targetY = hit.HitPosition.y + pushDistance;
		if (currentPosY < targetY)
		{
			float diffY = targetY - currentPosY;
			if (diffY > maxDiffY)
			{
				maxDiffY = diffY;
			}
		}
	}
	return VScale(AsoUtility::DIR_U, maxDiffY);
}
