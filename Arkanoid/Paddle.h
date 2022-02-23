#pragma once
using namespace DirectX::SimpleMath;

using namespace std;

class Paddle
{
public:
	Paddle();
	Paddle(int, int);

protected:
	// Paddle position
	Vector2 position;

public:
	Vector2 GetPosition();
};