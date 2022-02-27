#pragma once

using namespace DirectX::SimpleMath;
using namespace std;

class Brick
{
public:
	Brick();
	Brick(Vector2);

private:
	Vector2 position;
	bool isDestroyed;

public:
	Vector2 GetPosition();
	bool IsDestroyed();
};

