#include "pch.h"
#include "Ball.h"

Ball::Ball() {}
	
Ball::Ball(Vector2 paddlePos)
{
	position = Vector2(paddlePos.x+(PADDLEOFFSET), paddlePos.y - 20);
	controlled = true;
}

Vector2 Ball::GetPosition() {
	return position;
}

void Ball::SetPosition(Vector2 v) {
	position= Vector2(v.x + (PADDLEOFFSET), v.y - 20);
}

bool Ball::IsAttached() {
	return controlled;
}

void Ball::DetachBall()
{
	controlled = false;
}
