#pragma once

// Vector2テンプレートクラス
template <typename T>
class Vector2Template
{
public:

	// テンプレート変数
	T x, y;

	// コンストラクタ
	constexpr Vector2Template(void) : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

	// コンストラクタ
	constexpr Vector2Template(T vX, T vY) : x(vX), y(vY) {}

	template <typename U>
	constexpr Vector2Template(const Vector2Template<U>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {
	}

	// デストラクタ
	~Vector2Template(void) = default;

};

using Vector2 = Vector2Template<int>;

using Vector2F = Vector2Template<float>;

