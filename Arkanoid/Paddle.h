#pragma once
#define SPRITEWIDTH 64
#define BORDEROFFSET 78
#define PADDLESPEED 1.5f

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
	Vector2 screenSize;

public:
	Vector2 GetPosition();
	void SetPosition(Vector2);
	void MovePaddle(bool);
	Vector2 GetScreenSize();
};