#pragma once
class Vector2F
{
public:

	float x;
	float y;

    // コンストラクタ
    Vector2F(void) : x(0.0f), y(0.0f) {}

    // コンストラクタ
    Vector2F(float vX, float vY) : x(vX), y(vY) {}

    // デストラクタ
    ~Vector2F(void) = default;
};

