#include "pch.h"
#include "Paddle.h"


Paddle::Paddle()
{
	//PaddlePos = Vector2(0, 0);
}

Paddle::Paddle(int w, int h)
{
	position = Vector2( w / 2, h - 20);
}

Vector2 Paddle::GetPosition() {
	return position;
}


