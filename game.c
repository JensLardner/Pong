#include "game.h"
#include "globals.h"
#include "util.h"

/** Jens
 * Draws the paddles and ball with a specified color
 */
void drawPaddlesAndBalls(Paddle *paddle1, Paddle *paddle2, Ball *ball, char color){
  drawRectangle(paddle1->x, paddle1->y, paddle1->width, paddle1->height, color);
  drawRectangle(paddle2->x, paddle2->y, paddle2->width, paddle2->height, color);
  drawRectangle(ball->x, ball->y, ball->width, ball->height,color);
}

/** Jens
 * Draws the score on the screen
 */
void drawScore(char color){
  bool scaleBy2 = false;
  drawCharacter(SCREEN_WIDTH / 2 - CHARACTER_WIDTH, 10, score1 + '0', color, scaleBy2);
  drawCharacter(SCREEN_WIDTH / 2 + CHARACTER_WIDTH, 10, score2 + '0', color, scaleBy2);
}

/** Tahmid
 * Clears the frame of the moving elements
 */
void clearMovingElements(Paddle *paddle1, Paddle *paddle2, Ball *ball)
{
  char black = 0x00;
  drawPaddlesAndBalls(paddle1, paddle2, ball, black);
  drawScore(black);
}

/** Jens & Tahmid
 * Draws the moving elements on the screen
 */
void drawMovingElements(Paddle *paddle1, Paddle *paddle2, Ball *ball)
{
  char white = 0xFF;
  drawPaddlesAndBalls(paddle1, paddle2, ball, white);

  drawScore(white);
  drawCharacter(SCREEN_WIDTH / 2, 10, ':', 0xFF, white);

  frameBuffer();
}

/** Jens & Tahmid
 * Enables input with the game
 */
void input(Paddle *paddle1, Paddle *paddle2, Ball *ball)
{
  int switchInput = get_sw();

  int leftmostSwitch = switchInput & 0x100;
  int secondLeftmostSwitch = switchInput & 0x200;

  paddle1->up = leftmostSwitch;
  paddle1->down = secondLeftmostSwitch;

  if (gameState == PVP)
  {
    int secondRightmostSwitch = switchInput & 0x2;
    int rightmostSwitch = switchInput & 0x1;
    paddle2->up = secondRightmostSwitch;
    paddle2->down = rightmostSwitch;
  }
  else //PVC
  {
    int deadZone = 20;
    int ballDistance = SCREEN_WIDTH / 2 - 65;

    int paddle2Center = paddle2->y + paddle2->height / 2;
    int ballCenter = ball->y + ball->height / 2;
    int horizontalDistance = paddle2->x - (ball->x + ball->width);

    bool ballAbove = (paddle2Center - ballCenter) > deadZone;
    bool ballBelow = (ballCenter - paddle2Center) > deadZone;
    bool ballClose = horizontalDistance < ballDistance;

    paddle2->up = ballAbove && ballClose;

    paddle2->down = ballBelow && ballClose;

    // paddle2->up = (((paddle2->y + paddle2->height / 2) - (ball->y + ball->height / 2)) > deadZone)
    //   && (paddle2->x - (ball->x + ball->width)) < ballDistance;

    // paddle2->down = ((ball->y + ball->height / 2) - (paddle2->y + paddle2->height / 2) > deadZone) 
    // && (paddle2->x - (ball->x + ball->width)) < ballDistance;
  }
}

/** Jens
 * Handles the movement of a paddle
 */
void paddleMovement(Paddle *paddle1)
{
  if (paddle1->up && paddle1->y > 0)
    paddle1->y -= PADDLE_MOVEMENT_SPEED;

  if (paddle1->down && (paddle1->y + paddle1->height) < SCREEN_HEIGHT)
    paddle1->y += PADDLE_MOVEMENT_SPEED;
}

/** Jens
 * Resets the ball to the center of the screen
 */
void resetBall(Ball *ball)
{
  ball->x = SCREEN_WIDTH / 2 - ball->width / 2;
  ball->y = SCREEN_HEIGHT / 2 - ball->height / 2;
  ball->movY = -ball->movY;
  ball->movX = -ball->movX;
}

/** Jens & Tahmid
 * Handles when a point is scored
 */
void pointScored(Ball *ball, int *score)
{
  resetBall(ball);
  (*score)++;
  if (score == &score1)
    set_display(4, score1);
  else
    set_display(0, score2);

  if (*score == MAX_SCORE)
  {
    gameState = WINNER_SCREEN;
  }
}

/** Jens
 * Handles the movement of the ball
 */
void ballMovement(Ball *ball)
{
  if (ball->x <= 0)
  {
    pointScored(ball, &score2);
  }

  else if (ball->x + ball->width >= SCREEN_WIDTH)
  {
    pointScored(ball, &score1);
  }

  if (ball->y < 0 || (ball->y + ball->height) > SCREEN_HEIGHT)
  {
    ball->movY = -ball->movY;
  }
  ball->y += ball->movY;
  ball->x += ball->movX;
}

/** Tahmid
 * Resets information in the game
 */
void resetScores()
{
  score1 = 0;
  score2 = 0;
}

/** Jens
 * Handles the collision between a paddle and the ball
 */
void paddleCollision(Paddle *paddle1, Ball *ball)
{
  if (paddle1->y < (ball->y + ball->height) && ball->y < (paddle1->y + paddle1->height) && paddle1->x < (ball->x + ball->width) && ball->x < (paddle1->x + paddle1->width))
  {
    ball->movX = -ball->movX;

    if (ball->x < SCREEN_WIDTH / 2)
      ball->x += ball->width;
    else
      ball->x -= ball->width;
  }
}

/* Jens & Tahmid
 * Sets a paddle to it's inital position
 */
void setPaddle(Paddle *paddle, bool isLeftPaddle)
{
  paddle->width = SCREEN_WIDTH / PADDLE_RELATIVE_WIDTH;
  paddle->height = SCREEN_HEIGHT / PADDLE_RELATIVE_HEIGHT;
  if (isLeftPaddle)
  {
    paddle->x = PADDLE_X_OFFSET;
  }
  else
  {
    paddle->x = SCREEN_WIDTH - paddle->width - PADDLE_X_OFFSET;
  }
  paddle->y = SCREEN_HEIGHT / 2 - paddle->height / 2;
}

/* Jens
 * Sets the balll to it's initial position
 */
void setBall(Ball *ball)
{
  ball->height = 4;
  ball->width = 4;
  ball->x = SCREEN_WIDTH / 2 - ball->width / 2;
  ball->y = SCREEN_HEIGHT / 2 - ball->height / 2;
  ball->movY = BALL_MOVEMENT_SPEED;
  ball->movX = 1;
}

/** Jens & Tahmid
 * Handles the main game loop
 */
void runGameLoop()
{
  resetScores();

  Paddle paddle1;
  Paddle paddle2;
  bool isLeftPaddle = true;
  bool isRightPaddle = false;
  setPaddle(&paddle1, isLeftPaddle);
  setPaddle(&paddle2, isRightPaddle);

  Ball ball;
  setBall(&ball);

  clearBuffer();

  int blankDisplay = -1;
  for (int i = 0; i < 6; i++)
  {
    if (i == 3)
      set_display(i, blankDisplay);
    else if (i == 2)
      set_display(i, blankDisplay);
    else
      set_display(i, 0);
  }

  set_leds(0x303);

  while (gameState == PVP || gameState == PVC)
  {
    if (nextFrame)
    {
      clearMovingElements(&paddle1, &paddle2, &ball);

      input(&paddle1, &paddle2, &ball);

      paddleMovement(&paddle1);

      paddleMovement(&paddle2);

      ballMovement(&ball);

      paddleCollision(&paddle1, &ball);

      paddleCollision(&paddle2, &ball);

      drawMovingElements(&paddle1, &paddle2, &ball);

      nextFrame = 0;
    }
  }
}