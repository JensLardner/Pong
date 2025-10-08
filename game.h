#ifndef GAME_H
#define GAME_H

#include "stdbool.h"

#define MAX_SCORE 1

#define PADDLE_X_OFFSET 10
#define PADDLE_RELATIVE_HEIGHT 5
#define PADDLE_RELATIVE_WIDTH 40
#define PADDLE_MOVEMENT_SPEED 1

#define BALL_MOVEMENT_SPEED 1


typedef struct{
  short x;
  int y;
  int width;
  int height;
  bool up;
  bool down;
}Paddle;

typedef struct{
  int x;
  int y;
  int width;
  int height;
  int movX;
  int movY;
}Ball;

void clearMovingElements(Paddle* paddle1, Paddle* paddle2, Ball* ball);

void input(Paddle* paddle1, Paddle* paddle2, Ball* ball);

void paddleMovement(Paddle * paddle1);

void resetBall(Ball* ball);

void ballMovement(Ball* ball);

void paddleCollision(Paddle* paddle1, Ball* ball);


void runGameLoop();

void resetScores();

#endif 