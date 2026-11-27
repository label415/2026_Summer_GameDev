#include <math.h>
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "Quaternion.h"

Quaternion::Quaternion(void)
	: w(1)
	, x(0)
	, y(0)
	, z(0)
{
	// 単位クォータニオンで初期化
}

Quaternion::Quaternion(const VECTOR& rad)
{
	// オイラー角(ラジアン)からクォータニオンへ変換して初期化
	Quaternion q = Euler(rad.x, rad.y, rad.z);
	w = q.w;
	x = q.x;
	y = q.y;
	z = q.z;
}

Quaternion::Quaternion(double ww, double wx, double wy, double wz)
{
	// 各成分を直接設定
	w = ww;
	x = wx;
	y = wy;
	z = wz;
}

Quaternion Quaternion::Euler(double radX, double radY, double radZ)
{
	Quaternion ret = Quaternion();

	// 各軸の角度を 0?2π の範囲へ正規化
	radX = AsoUtility::RadIn2PI(radX);
	radY = AsoUtility::RadIn2PI(radY);
	radZ = AsoUtility::RadIn2PI(radZ);

	// 半角に対する三角関数値を計算
	double cosZ = cos(radZ / AsoUtility::DOUBLE);
	double sinZ = sin(radZ / AsoUtility::DOUBLE);
	double cosX = cos(radX / AsoUtility::DOUBLE);
	double sinX = sin(radX / AsoUtility::DOUBLE);
	double cosY = cos(radY / AsoUtility::DOUBLE);
	double sinY = sin(radY / AsoUtility::DOUBLE);

	// 回転順に基づくクォータニオン各成分を算出
	ret.w = cosX * cosY * cosZ + sinX * sinY * sinZ;
	ret.x = sinX * cosY * cosZ + cosX * sinY * sinZ;
	ret.y = cosX * sinY * cosZ - sinX * cosY * sinZ;
	ret.z = cosX * cosY * sinZ - sinX * sinY * cosZ;

	return ret;
}

Quaternion Quaternion::Mult(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion ret = Quaternion();
	double d1, d2, d3, d4;

	// wの計算 
	d1 = q1.w * q2.w;
	d2 = -q1.x * q2.x;
	d3 = -q1.y * q2.y;
	d4 = -q1.z * q2.z;
	ret.w = d1 + d2 + d3 + d4;

	// xの計算 
	d1 = q1.w * q2.x;
	d2 = q2.w * q1.x;
	d3 = q1.y * q2.z;
	d4 = -q1.z * q2.y;
	ret.x = d1 + d2 + d3 + d4;

	// yの計算
	d1 = q1.w * q2.y;
	d2 = q2.w * q1.y;
	d3 = q1.z * q2.x;
	d4 = -q1.x * q2.z;
	ret.y = d1 + d2 + d3 + d4;

	// zの計算
	d1 = q1.w * q2.z;
	d2 = q2.w * q1.z;
	d3 = q1.x * q2.y;
	d4 = -q1.y * q2.x;
	ret.z = d1 + d2 + d3 + d4;

	return ret;
}

Quaternion Quaternion::AngleAxis(double rad, VECTOR axis)
{
	Quaternion ret = Quaternion();

	double norm;
	double c, s;

	// Unityに合わせる
	//ret.w = ret.x = ret.y = ret.z = 0.0;
	ret.w = 1.0;
	ret.x = ret.y = ret.z = 0.0;

	// 軸ベクトルの長さの2乗を算出し、ゼロベクトルなら単位クォータニオンを返す
	norm = (double)axis.x * (double)axis.x + (double)axis.y * (double)axis.y + (double)axis.z * (double)axis.z;
	if (norm <= 0.0f)
	{
		return ret;
	}

	// 軸ベクトルを正規化
	norm = 1.0 / sqrt(norm);
	axis.x = (float)(axis.x * norm);
	axis.y = (float)(axis.y * norm);
	axis.z = (float)(axis.z * norm);

	// 回転角の半角からクォータニオン成分を算出
	c = cos(AsoUtility::HALF * rad);
	s = sin(AsoUtility::HALF * rad);

	ret.w = c;
	ret.x = s * axis.x;
	ret.y = s * axis.y;
	ret.z = s * axis.z;

	return ret;
}

VECTOR Quaternion::PosAxis(const Quaternion& q, VECTOR pos)
{
	// 位置情報に回転情報を反映させる
	Quaternion tmp = Quaternion();
	tmp = tmp.Mult(q);
	tmp = tmp.Mult(Quaternion(0.0f, pos.x, pos.y, pos.z));
	tmp = tmp.Mult(q.Inverse());
	return { (float)tmp.x, (float)tmp.y, (float)tmp.z };
}

VECTOR Quaternion::ToEuler(const Quaternion& q)
{
	VECTOR ret;

	// クォータニオン成分から回転行列要素を算出してオイラー角へ変換
	double r11 = AsoUtility::DOUBLE_D * (q.x * q.z + q.w * q.y);
	double r12 = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;
	double r21 = -AsoUtility::DOUBLE_D * (q.y * q.z - q.w * q.x);
	double r31 = AsoUtility::DOUBLE_D * (q.x * q.y + q.w * q.z);
	double r32 = q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z;

	ret.x = static_cast<float>(asin(r21));
	ret.y = static_cast<float>(atan2(r11, r12));
	ret.z = static_cast<float>(atan2(r31, r32));

	return ret;
}

MATRIX Quaternion::ToMatrix(const Quaternion& q)
{
	MATRIX mat;

	FLOAT4 fq = { (float)q.x, (float)q.y, (float)q.z, (float)q.w };

	// クォータニオンの積項を事前計算して4x4回転行列へ展開
	float sx = fq.x * fq.x * AsoUtility::DOUBLE;
	float sy = fq.y * fq.y * AsoUtility::DOUBLE;
	float sz = fq.z * fq.z * AsoUtility::DOUBLE;
	float cx = fq.y * fq.z * AsoUtility::DOUBLE;
	float cy = fq.x * fq.z * AsoUtility::DOUBLE;
	float cz = fq.x * fq.y * AsoUtility::DOUBLE;
	float wx = fq.w * fq.x * AsoUtility::DOUBLE;
	float wy = fq.w * fq.y * AsoUtility::DOUBLE;
	float wz = fq.w * fq.z * AsoUtility::DOUBLE;

	mat.m[0][0] = 1.0f - (sy + sz);	mat.m[0][1] = cz + wz;			mat.m[0][2] = cy - wy;			mat.m[0][3] = 0.0f;
	mat.m[1][0] = cz - wz;			mat.m[1][1] = 1.0f - (sx + sz);	mat.m[1][2] = cx + wx;			mat.m[1][3] = 0.0f;
	mat.m[2][0] = cy + wy;			mat.m[2][1] = cx - wx;			mat.m[2][2] = 1.0f - (sx + sy);	mat.m[2][3] = 0.0f;
	mat.m[3][0] = 0.0f;				mat.m[3][1] = 0.0f;				mat.m[3][2] = 0.0f;				mat.m[3][3] = 1.0f;

	return mat;
}

Quaternion Quaternion::LookRotation(const VECTOR& dir)
{
	// ワールド上方向を基準として回転を生成
	VECTOR up = { 0.0f, 1.0f, 0.0f };
	return LookRotation(dir, up);
}

Quaternion Quaternion::LookRotation(const VECTOR& dir, const VECTOR& up)
{
	// 前方・右・上方向の直交基底ベクトルを構築
	VECTOR norDir = AsoUtility::VNormalize(dir);
	VECTOR right = AsoUtility::VNormalize(VCross(up, norDir));
	VECTOR crossUp = VCross(norDir, right);
	auto m00 = right.x;
	auto m01 = right.y;
	auto m02 = right.z;
	auto m10 = crossUp.x;
	auto m11 = crossUp.y;
	auto m12 = crossUp.z;
	auto m20 = norDir.x;
	auto m21 = norDir.y;
	auto m22 = norDir.z;

	// 回転行列のトレース値に応じたケース分けでクォータニオンへ変換
	float num8 = (m00 + m11) + m22;
	auto quaternion = Quaternion();
	if (num8 > 0.0f)
	{
		double num = sqrt(num8 + 1.0);
		quaternion.w = num * AsoUtility::HALF_D;
		num = AsoUtility::HALF_D / num;
		quaternion.x = ((double)m12 - m21) * num;
		quaternion.y = ((double)m20 - m02) * num;
		quaternion.z = ((double)m01 - m10) * num;
		return quaternion.Normalized();
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		// xとwが逆？
		auto num7 = sqrt(((1.0f + m00) - m11) - m22);
		auto num4 = AsoUtility::HALF / num7;
		quaternion.x = ((double)m12 - m21) * num4;
		quaternion.y = ((double)m01 + m10) * num4;
		quaternion.z = ((double)m02 + m20) * num4;
		quaternion.w = AsoUtility::HALF_D * num7;
		return quaternion.Normalized();
	}
	if (m11 > m22)
	{
		auto num6 = sqrt(((1.0f + m11) - m00) - m22);
		auto num3 = AsoUtility::HALF / num6;
		quaternion.x = ((double)m10 + m01) * num3;
		quaternion.y = AsoUtility::HALF_D * num6;
		quaternion.z = ((double)m21 + m12) * num3;
		quaternion.w = ((double)m20 - m02) * num3;
		return quaternion.Normalized();
	}
	auto num5 = sqrt(((1.0f + m22) - m00) - m11);
	auto num2 = AsoUtility::HALF / num5;
	quaternion.x = ((double)m20 + m02) * num2;
	quaternion.y = ((double)m21 + m12) * num2;
	quaternion.z = AsoUtility::HALF_D * num5;
	quaternion.w = ((double)m01 - m10) * num2;
	return quaternion.Normalized();

}

Quaternion Quaternion::GetRotation(const MATRIX& mat)
{
	Quaternion ret;

	// 行列の対角和(トレース)から最大軸を判定してクォータニオンを復元
	float s;
	float tr = mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + 1.0f;
	if (tr >= 1.0f)
	{
		s = AsoUtility::HALF / sqrtf(tr);
		ret.w = AsoUtility::QUARTER / s;
		ret.x = (mat.m[1][2] - mat.m[2][1]) * s;
		ret.y = (mat.m[2][0] - mat.m[0][2]) * s;
		ret.z = (mat.m[0][1] - mat.m[1][0]) * s;
	}
	else
	{
		float max;
		max = mat.m[1][1] > mat.m[2][2] ? mat.m[1][1] : mat.m[2][2];

		if (max < mat.m[0][0])
		{
			s = sqrtf(mat.m[0][0] - (mat.m[1][1] + mat.m[2][2]) + 1.0f);

			float x = s * AsoUtility::HALF;
			s = AsoUtility::HALF / s;
			ret.x = x;
			ret.y = (mat.m[0][1] + mat.m[1][0]) * s;
			ret.z = (mat.m[2][0] + mat.m[0][2]) * s;
			ret.w = (mat.m[1][2] - mat.m[2][1]) * s;
		}
		else
			if (max == mat.m[1][1])
			{
				s = sqrtf(mat.m[1][1] - (mat.m[2][2] + mat.m[0][0]) + 1.0f);

				float y = s * AsoUtility::HALF;
				s = AsoUtility::HALF / s;
				ret.x = (mat.m[0][1] + mat.m[1][0]) * s;
				ret.y = y;
				ret.z = (mat.m[1][2] + mat.m[2][1]) * s;
				ret.w = (mat.m[2][0] - mat.m[0][2]) * s;
			}
			else
			{
				s = sqrtf(mat.m[2][2] - (mat.m[0][0] + mat.m[1][1]) + 1.0f);

				float z = s * AsoUtility::HALF;
				s = AsoUtility::HALF / s;
				ret.x = (mat.m[2][0] + mat.m[0][2]) * s;
				ret.y = (mat.m[1][2] + mat.m[2][1]) * s;
				ret.z = z;
				ret.w = (mat.m[0][1] - mat.m[1][0]) * s;
			}
	}

	return ret;
}

VECTOR Quaternion::GetForward(void) const
{
	// 前方向ベクトルを取得
	return GetDir(AsoUtility::DIR_F);
}

VECTOR Quaternion::GetBack(void) const
{
	// 後方向ベクトルを取得
	return GetDir(AsoUtility::DIR_B);
}

VECTOR Quaternion::GetRight(void) const
{
	// 右方向ベクトルを取得
	return GetDir(AsoUtility::DIR_R);
}

VECTOR Quaternion::GetLeft(void) const
{
	// 左方向ベクトルを取得
	return GetDir(AsoUtility::DIR_L);
}

VECTOR Quaternion::GetUp(void) const
{
	// 上方向ベクトルを取得
	return GetDir(AsoUtility::DIR_U);
}

VECTOR Quaternion::GetDown(void) const
{
	// 下方向ベクトルを取得
	return GetDir(AsoUtility::DIR_D);
}

Quaternion Quaternion::Normalize(const Quaternion& q)
{
	// 長さの逆数を掛けて正規化したクォータニオンを返す
	float scale = 1.0f / static_cast<float>(q.Length());
	VECTOR v = VScale(q.xyz(), scale);
	Quaternion ret = Quaternion(q.w * scale, v.x, v.y, v.z);
	return ret;
}

Quaternion Quaternion::Normalized(void) const
{
	// 各成分を大きさで割った正規化済みクォータニオンを返す
	double mag = sqrt(w * w + x * x + y * y + z * z);
	return Quaternion(w / mag, x / mag, y / mag, z / mag);
}

void Quaternion::Normalize(void)
{
	// 自身の大きさを求めて各成分を正規化
	double mag = sqrt(w * w + x * x + y * y + z * z);

	w /= mag;
	x /= mag;
	y /= mag;
	z /= mag;
}

Quaternion Quaternion::Inverse(void) const
{
	// 共役クォータニオンをノルム2乗で割って逆クォータニオンを算出
	double n = 1.0f / (w * w + x * x + y * y + z * z);
	Quaternion tmp = Quaternion(w, -x, -y, -z);
	return Quaternion(tmp.w * n, tmp.x * n, tmp.y * n, tmp.z * n);;
}

Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, double t)
{
	// 補間係数 t を 0.0?1.0 にクランプして球面線形補間を実行
	if (t > 1) t = 1;
	if (t < 0) t = 0;
	return SlerpUnclamped(from, to, (float)t);
}

inline float SIGN(float x) {
	// 数値の符号を +1.0 / -1.0 で返す
	return (x >= 0.0f) ? +1.0f : -1.0f;
}

inline float NORM(float a, float b, float c, float d) {
	// 4要素ベクトルのユークリッドノルムを算出
	return sqrtf(a * a + b * b + c * c + d * d);
}

Quaternion Quaternion::FromToRotation(const VECTOR& fromDir, const VECTOR& toDir)
{
	// 外積で回転軸を、内積角で回転量を算出（真逆方向時は直交軸を代替生成）
	VECTOR axis = VCross(fromDir, toDir);
	double angle = AsoUtility::AngleDeg(fromDir, toDir);
	if (angle >= OPPOSITE_ANGLE_DEG_THRESHOLD)
	{
		auto r = VCross(fromDir, AsoUtility::DIR_R);
		axis = VCross(r, fromDir);
		//if (axis.sqrMagnitude < 0.000001f)
		float len = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
		if (len < AsoUtility::EPSILON)
		{
			axis = AsoUtility::DIR_U;
		}
	}

	axis = AsoUtility::VNormalize(axis);
	return Quaternion::AngleAxis(AsoUtility::Deg2RadD(angle), axis);
}

Quaternion Quaternion::RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta)
{
	// 2つの回転間の角度を求め、最大変化角度でクランプして補間
	double num = Quaternion::Angle(from, to);
	if (num == 0.0)
	{
		return to;
	}
	float t = min(1.0f, maxDegreesDelta / (float)num);
	return Quaternion::SlerpUnclamped(from, to, t);
}

double Quaternion::Angle(const Quaternion& q1, const Quaternion& q2)
{
	// 内積から角度(度)を算出
	double cos = Quaternion::Dot(q1, q2);
	double ac = acos(cos);
	return AsoUtility::Rad2DegD(ac);
}

Quaternion Quaternion::SlerpUnclamped(Quaternion a, Quaternion b, float t)
{
	// if either input is zero, return the other.
	if (a.LengthSquared() == 0.0f)
	{
		if (b.LengthSquared() == 0.0f)
		{
			return Identity();
		}
		return b;
	}
	else if (b.LengthSquared() == 0.0f)
	{
		return a;
	}


	float cosHalfAngle = (float)(a.w * b.w) + VDot(a.xyz(), b.xyz());

	if (cosHalfAngle >= 1.0f || cosHalfAngle <= -1.0f)
	{
		// angle = 0.0f, so just return one input.
		return a;
	}
	else if (cosHalfAngle < 0.0f)
	{
		//b.xyz() = -b.xyz();
		b.x = b.x * -1.0f;
		b.y = b.y * -1.0f;
		b.z = b.z * -1.0f;
		b.w = -b.w;
		cosHalfAngle = -cosHalfAngle;
	}

	float blendA;
	float blendB;
	if (cosHalfAngle < SLERP_DOT_THRESHOLD)
	{
		// do proper slerp for big angles
		float halfAngle = acosf(cosHalfAngle);
		float sinHalfAngle = sinf(halfAngle);
		float oneOverSinHalfAngle = 1.0f / sinHalfAngle;
		blendA = sinf(halfAngle * (1.0f - t)) * oneOverSinHalfAngle;
		blendB = sinf(halfAngle * t) * oneOverSinHalfAngle;
	}
	else
	{
		// do lerp if angle is really small.
		blendA = 1.0f - t;
		blendB = t;
	}

	//Quaternion result = Quaternion(blendA * a.xyz() + blendB * b.xyz(), blendA * a.w + blendB * b.w);
	VECTOR v = VAdd(VScale(a.xyz(), blendA), VScale(b.xyz(), blendB));
	//Quaternion result = Quaternion(v.x, v.y, v.z, blendA * a.w + blendB * b.w);
	Quaternion result = Quaternion(blendA * a.w + blendB * b.w, v.x, v.y, v.z);
	if (result.LengthSquared() > 0.0f)
	{
		return Normalize(result);
	}
	else
	{
		return Identity();
	}
}

void Quaternion::ToAngleAxis(float* angle, VECTOR* axis)
{
	// w が [-1, 1] の範囲外なら正規化してから回転角を算出
	if (abs(this->w) > 1.0f)
	{
		this->Normalize();
	}
	*angle = AsoUtility::DOUBLE * acosf((float)this->w); // angle

	// 虚部がすべて0なら無回転として角度を0に設定
	if (x == 0 && y == 0 && z == 0)
	{
		*angle = 0.0f;
	}

	// sin(theta/2) から回転軸ベクトルを復元（極小時はX軸を代替）
	float den = sqrtf(1.0f - (float)(this->w * this->w));
	if (den > AsoUtility::EPSILON_SMALL)
	{
		//axis = q->xyz / den;
		auto v = this->xyz();
		axis->x = v.x / den;
		axis->y = v.y / den;
		axis->z = v.z / den;
	}
	else
	{
		*axis = { 1.0f, 0.0f, 0.0f };
	}
}
