#pragma once
#define SPRITEWIDTH 64

using namespace DirectX::SimpleMath;
using namespace std;

class Paddle
{
public:
	Paddle();
	Paddle(int, int);

private:
	// Paddle position
	Vector2 position;
	int screenWidth;

public:
	Vector2 GetPosition();
	void MovePaddle(bool);
};