#pragma once
#define PADDLEOFFSET 50
#define SPRITEWIDTH 64
#define BALLOFFSET 34

#define BALLSPEED 2

using namespace DirectX::SimpleMath;
using namespace std;

class Ball
{
public:
	Ball();
	Ball(Vector2);
private:
	Vector2 screenSize;
	Vector2 position;
	bool controlled;
	Vector2 velocity;
	int pitDirection;

public:
	Vector2 GetPosition();
	void SetPosition(Vector2);
	bool IsAttached();
	void DetachBall();
	void SetDirection(int);

	void UpdatePosition(int);
};

