#pragma once
#include <cmath>
#include <DxLib.h>
#include <algorithm>
class Quaternion
{
public:

	// クォータニオンの正規化に使用する最小値
	static constexpr float kEpsilonNormalSqrt = 1e-15F;

	// クォータニオン成分（実部w、虚部x, y, z）
	double w;
	double x;
	double y;
	double z;

	// コンストラクタ
	Quaternion(void);
	explicit Quaternion(const VECTOR& rad);
	Quaternion(double w, double x, double y, double z);

	// デストラクタ
	~Quaternion(void) {}

	// オイラー角からクォータニオンへ変換
	static Quaternion Euler(const VECTOR& rad) { return Euler(rad.x, rad.y, rad.z); }
	static Quaternion Euler(double radX, double radY, double radZ);

	// クォータニオンの合成
	static Quaternion Mult(const Quaternion& q1, const Quaternion& q2);
	Quaternion Mult(const Quaternion& q) const { return Mult(Quaternion(w, x, y, z), q); }

	// 指定軸を指定角分、回転させる
	static Quaternion AngleAxis(double rad, VECTOR axis);

	// 座標を回転させる
	static VECTOR PosAxis(const Quaternion& q, VECTOR pos);
	VECTOR PosAxis(VECTOR pos) const { return PosAxis(Quaternion(w, x, y, z), pos); }

	// クォータニオンからオイラー角へ変換
	static VECTOR ToEuler(const Quaternion& q);
	VECTOR ToEuler(void) const { return ToEuler(Quaternion(w, x, y, z)); }

	// クォータニオンから行列へ変換
	static MATRIX ToMatrix(const Quaternion& q);
	MATRIX ToMatrix(void) const { return ToMatrix(Quaternion(w, x, y, z)); }

	// ベクトルからクォータニオンに変換
	static Quaternion LookRotation(const VECTOR& dir);
	static Quaternion LookRotation(const VECTOR& dir, const VECTOR& up);

	// 行列からクォータニオンに変換
	static Quaternion GetRotation(const MATRIX& mat);

	// 基本ベクトルを取得
	VECTOR GetForward(void) const;
	VECTOR GetBack(void) const;
	VECTOR GetRight(void) const;
	VECTOR GetLeft(void) const;
	VECTOR GetUp(void) const;
	VECTOR GetDown(void) const;

	// 内積
	static double Dot(const Quaternion& q1, const Quaternion& q2) { return (q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z); }
	double Dot(const Quaternion& b) const { return (w * b.w + x * b.x + y * b.y + z * b.z); }

	// 正規化
	static Quaternion Normalize(const Quaternion& q);
	Quaternion Normalized(void) const;
	void Normalize(void);

	// 逆クォータニオン
	Quaternion Inverse(void) const;

	// 球面補間
	static Quaternion Slerp(const Quaternion& from, const Quaternion& to, double t);

	// ２つのベクトル間の回転量を取得する
	static Quaternion FromToRotation(const VECTOR& fromDir, const VECTOR& toDir);

	// 指定角度上限で目標回転へ近づける
	static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta);

	// ２つのクォータニオン間の角度を取得
	static double Angle(const Quaternion& q1, const Quaternion& q2);

	// クランプなしの球面補間
	static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t);

	// 単位クォータニオンを取得
	static Quaternion Identity(void) { return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); }

	// 長さ
	double Length(void) const { return sqrt(x * x + y * y + z * z + w * w); }

	// 長さの2乗
	double LengthSquared(void) const { return x * x + y * y + z * z + w * w; }

	// 虚部ベクトル(x, y, z)を取得
	VECTOR xyz(void) const { return { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) }; }

	// 対象方向の回転
	void ToAngleAxis(float* angle, VECTOR* axis);

private:

	// 逆向き判定角度しきい値
	static constexpr double OPPOSITE_ANGLE_DEG_THRESHOLD = 179.9196;

	// Slerp時に線形補間へフォールバックする内積しきい値
	static constexpr float SLERP_DOT_THRESHOLD = 0.99f;

	// 基本ベクトルを取得
	VECTOR GetDir(VECTOR dir) const { return PosAxis(dir); }

	// スカラー乗算演算子
	Quaternion operator*(float rhs) { return Quaternion(w * rhs, x * rhs, y * rhs, z * rhs); }

	// 加算演算子
	Quaternion operator+(const Quaternion& rhs) { return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z); }

};
