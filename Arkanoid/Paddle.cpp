#include "pch.h"
#include "Paddle.h"

#define PADDLESPEED 1.5f

Paddle::Paddle() {}

Paddle::Paddle(int w, int h)
{
	screenWidth = w;
	position = Vector2((w / 2)-(SPRITEWIDTH/1.5f), h - 20);
}

Vector2 Paddle::GetPosition() {
	return position;
}

void Paddle::MovePaddle(bool dir)
{
	if (dir && position.x+BORDEROFFSET+SPRITEWIDTH/1.5f < screenWidth) {
		position.x+= PADDLESPEED;
	}
	else if (!dir && position.x > BORDEROFFSET-SPRITEWIDTH) {
		position.x-= PADDLESPEED;
	}
}


