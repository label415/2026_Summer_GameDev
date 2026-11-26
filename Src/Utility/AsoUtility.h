#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <DxLib.h>
#include "../Common/Quaternion.h"
#include "../Template/Vector2Template.h"

class AsoUtility
{
public:

	// ラジアン(rad)・度(deg)変換用
	static constexpr float RAD2DEG = (180.0f / DX_PI_F);
	static constexpr float DEG2RAD = (DX_PI_F / 180.0f);

	// ベクトルの定数
	static constexpr VECTOR VECTOR_ZERO = { 0.0f, 0.0f, 0.0f };
	static constexpr VECTOR VECTOR_ONE = { 1.0f, 1.0f, 1.0f };
	static constexpr Vector2 VECTOR2_ZERO = { 0, 0 };
	static constexpr Vector2F VECTOR2F_ZERO = { 0.0f, 0.0f };

	// 回転軸
	static constexpr VECTOR AXIS_X = { 1.0f, 0.0f, 0.0f };
	static constexpr VECTOR AXIS_Y = { 0.0f, 1.0f, 0.0f };
	static constexpr VECTOR AXIS_Z = { 0.0f, 0.0f, 1.0f };

	// 方向
	static constexpr VECTOR DIR_F = { 0.0f, 0.0f, 1.0f };
	static constexpr VECTOR DIR_B = { 0.0f, 0.0f, -1.0f };
	static constexpr VECTOR DIR_R = { 1.0f, 0.0f, 0.0f };
	static constexpr VECTOR DIR_L = { -1.0f, 0.0f, 0.0f };
	static constexpr VECTOR DIR_U = { 0.0f, 1.0f, 0.0f };
	static constexpr VECTOR DIR_D = { 0.0f, -1.0f, 0.0f };

	// 360度・180度・90度
	static constexpr float DEG_360 = 360.0f;
	static constexpr float DEG_180 = DEG_360 / 2;
	static constexpr float DEG_90 = 90.0f;

	// 数学係数・指数
	static constexpr float HALF = 0.5f;
	static constexpr double HALF_D = 0.5;
	static constexpr float QUARTER = 0.25f;
	static constexpr float DOUBLE = 2.0f;
	static constexpr double DOUBLE_D = 2.0;
	static constexpr int SQUARE_EXPONENT = 2;

	// 許容誤差
	static constexpr float EPSILON = 1e-6f;
	static constexpr float EPSILON_SMALL = 1e-4f;

	// デバッグ描画色・デフォルト長
	static constexpr int COLOR_BLACK = 0x000000;
	static constexpr int COLOR_RED = 0xff0000;
	static constexpr int COLOR_GREEN = 0x00ff00;
	static constexpr int COLOR_BLUE = 0x0000ff;
	static constexpr float DEFAULT_LINE_LEN = 50.0f;

	// 正規化の際のゼロ除算回避用
	static constexpr float kEpsilonNormalSqrt = 1e-15F;

	// 値を範囲に収める
	static float Clamp(float v, float min, float max);

	// 四捨五入
	static int Round(float v) { return static_cast<int>(roundf(v)); }

	// ラジアン(rad)から度(deg)
	static double Rad2DegD(double rad) { return rad * (DEG_180 / DX_PI); }
	static float Rad2DegF(float rad) { return rad * (DEG_180 / DX_PI_F); }
	static int Rad2DegI(int rad) { return rad * Round(DEG_180 / DX_PI_F); }

	// 度(deg)からラジアン(rad)
	static double Deg2RadD(double deg) { return deg * (DX_PI / DEG_180); }
	static float Deg2RadF(float deg) { return deg * (DX_PI_F / DEG_180); }
	static int Deg2RadI(int deg) { return deg * Round(DX_PI_F / DEG_180); }

	// 0～360度の範囲に収める
	static double DegIn360(double deg);

	// 0(0)～2π(360度)の範囲に収める
	static double RadIn2PI(double rad);

	// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
	static int DirNearAroundRad(float from, float to);

	// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
	static int DirNearAroundDeg(float from, float to);

	// 線形補間
	static int Lerp(int start, int end, float t);
	static float Lerp(float start, float end, float t);
	static double Lerp(double start, double end, double t);
	static Vector2 Lerp(const Vector2& start, const Vector2& end, float t);
	static VECTOR Lerp(const VECTOR& start, const VECTOR& end, float t);

	// 角度の線形補間
	static double LerpDeg(double start, double end, double t);

	// 色の線形補間
	static COLOR_F Lerp(const COLOR_F& start, const COLOR_F& end, float t);

	// ベジェ曲線
	static Vector2 Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t);
	static VECTOR Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t);

	// Y軸回転
	static VECTOR RotXZPos(const VECTOR& centerPos, const VECTOR& radiusPos, float rad);

	// ベクトルの長さ
	static double Magnitude(const Vector2& v) { return sqrt((v.x * v.x) + (v.y * v.y)); }
	static double Magnitude(const VECTOR& v) { return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)); }
	static float MagnitudeF(const VECTOR& v) { return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z)); }
	static int SqrMagnitude(const Vector2& v) { return v.x * v.x + v.y * v.y; }
	static float SqrMagnitudeF(const VECTOR& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
	static double SqrMagnitude(const VECTOR& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
	static double SqrMagnitude(const VECTOR& v1, const VECTOR& v2) { return pow(v2.x - v1.x, SQUARE_EXPONENT) + pow(v2.y - v1.y, SQUARE_EXPONENT) + pow(v2.z - v1.z, SQUARE_EXPONENT); }
	static double Distance(const Vector2& v1, const Vector2& v2) { return sqrt(pow(v2.x - v1.x, SQUARE_EXPONENT) + pow(v2.y - v1.y, SQUARE_EXPONENT)); }
	static double Distance(const VECTOR& v1, const VECTOR& v2) { return sqrt(pow(v2.x - v1.x, SQUARE_EXPONENT) + pow(v2.y - v1.y, SQUARE_EXPONENT) + pow(v2.z - v1.z, SQUARE_EXPONENT)); }

	// 球体同士の衝突判定
	static bool IsHitSpheres(
		const VECTOR& pos1, float radius1, const VECTOR& pos2, float radius2);

	// 球体とカプセルの衝突判定
	static bool IsHitSphereCapsule(
		const VECTOR& sphPos, float sphRadius,
		const VECTOR& capPos1, const VECTOR& capPos2, float capRadius);

	// 比較
	static bool Equals(const VECTOR& v1, const VECTOR& v2);
	static bool EqualsVZero(const VECTOR& v1);

	// 正規化
	static VECTOR Normalize(const Vector2& v);
	static VECTOR VNormalize(const VECTOR& v);

	// 2つのベクトルの間の角度
	static double AngleDeg(const VECTOR& from, const VECTOR& to);

	// 描画系
	static void DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len = DEFAULT_LINE_LEN);
	static void DrawLineXYZ(const VECTOR& pos, const MATRIX& rot, float len = DEFAULT_LINE_LEN);
	static void DrawLineXYZ(const VECTOR& pos, const Quaternion& rot, float len = DEFAULT_LINE_LEN);

	// 文字列の分割
	static std::vector<std::string> Split(std::string& line, char delimiter);

};

