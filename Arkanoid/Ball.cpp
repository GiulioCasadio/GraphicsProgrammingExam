#include "pch.h"
#include "Ball.h"

Ball::Ball() {}

Ball::Ball(Vector2 paddlePos)
{
	position = Vector2(paddlePos.x+(PADDLEOFFSET), paddlePos.y - 24);
}

Vector2 Ball::GetPosition() {
	return position;
}