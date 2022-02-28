#include "pch.h"
#include "Ball.h"
#include <cmath>

Ball::Ball() {}
	
Ball::Ball(Vector2 paddlePos)
{
	screenSize = paddlePos;
	position = Vector2(paddlePos.x+(PADDLEOFFSET), paddlePos.y - 20);
	controlled = true;
}

Vector2 Ball::GetPosition() {
	return position;
}

void Ball::SetPosition(Vector2 v) {
	position= Vector2(v.x + (PADDLEOFFSET), v.y - 20);
}

Vector2 Ball::GetScreenSize()
{
	return screenSize;
}

Vector2 Ball::GetVelocity()
{
	return velocity;
}

void Ball::SetVelocity(Vector2 v)
{
	velocity = v;
}

bool Ball::IsAttached() {
	return controlled;
}

void Ball::Attach()
{
	controlled = true;
}

bool Ball::IsUnderPaddle()
{
	if (position.y > screenSize.y-BALLOFFSET/3) {
		return true;
	}
	return false;
}

void Ball::DetachBall()
{
	controlled = false;

	switch (pitDirection) {
	case 1:
		velocity=Vector2(BALLSPEED,-BALLSPEED);
		break;
	case -1:
		velocity = Vector2(-BALLSPEED,-BALLSPEED);
		break;
	default:
		velocity = Vector2(0,-BALLSPEED);
	}
}

void Ball::SetDirection(int dir)
{
	pitDirection = dir;
}

void Ball::ChangeVerticalDir()
{
	velocity.y *= -1;
}

void Ball::ChangeHorizontalDir()
{
	velocity.x *= -1;
}

void Ball::UpdatePosition(int paddlex)
{
	// check height
	if (position.y >= screenSize.y - BALLOFFSET && position.x >= paddlex - PADDLEOFFSET / 3 && position.x <= paddlex + PADDLEOFFSET * 2.5f) {
		if (position.x >= paddlex - PADDLEOFFSET / 3 && position.x <= paddlex + PADDLEOFFSET/1.8f) {
			SetVelocity(Vector2(-BALLSPEED, -BALLSPEED)); // bounce to the left
		}
		else if(position.x >= paddlex + BALLOFFSET*2.2f && position.x <= paddlex + PADDLEOFFSET * 2.5f){ // bounce to the right
			SetVelocity(Vector2(BALLSPEED, -BALLSPEED));
		}
		else {
			ChangeVerticalDir();
		}	

	} 
	else if (position.y <= 0) {
		ChangeVerticalDir();
	}

	// check width
	if (position.x <= 0 || position.x >= screenSize.x) {
		ChangeHorizontalDir();
	}

	position.x += velocity.x;
	position.y += velocity.y;
}
