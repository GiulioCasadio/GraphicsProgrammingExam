#pragma once
#define PADDLEOFFSET 50

using namespace DirectX::SimpleMath;
using namespace std;

class Ball
{
public:
	Ball();
	Ball(Vector2);
private:
	// Paddle position
	Vector2 position;

public:
	Vector2 GetPosition();
};

