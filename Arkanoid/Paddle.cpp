#include "pch.h"
#include "Paddle.h"


Paddle::Paddle(){}

Paddle::Paddle(int w, int h)
{
	screenWidth = w;
	position = Vector2( w / 2, h - 20);
}

Vector2 Paddle::GetPosition() {
	return position;
}

void Paddle::MovePaddle(bool dir)
{
	if (dir && position.x + SPRITEWIDTH <screenWidth) {
		position.x++;
	}
	else if (!dir && position.x > SPRITEWIDTH) {
		position.x--;
	}
}


