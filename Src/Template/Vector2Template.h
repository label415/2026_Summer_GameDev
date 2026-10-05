#pragma once

// Vector2テンプレートクラス
template <typename T>
class Vector2Template
{
public:
	// テンプレート変数
	T x;
	T y;

	// コンストラクタ
	Vector2Template(void) : x(0), y(0) {}

	// コンストラクタ
	Vector2Template(T vX, T vY) : x(vX), y(vY){}

	// デストラクタ
	~Vector2Template(void) = default;
};

using Vector2 = Vector2Template<int>;
using Vector2F = Vector2Template<float>;

