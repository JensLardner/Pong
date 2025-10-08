#include "game.h"
#include "globals.h"
#include "util.h"


  bool prints = true;

/**
 * Clears the frame of the moving elements
 */
void clearMovingElements(Paddle* paddle1, Paddle* paddle2, Ball* ball){
    drawRectangle(paddle1->x, paddle1->y, paddle1->width, paddle1->height, 0x00);
    drawRectangle(paddle2->x, paddle2->y, paddle2->width, paddle2->height, 0x00);
    drawRectangle(ball->x, ball->y, ball->width, ball->height, 0x00);

    drawCharacter(SCREEN_WIDTH/2 - CHARACTER_WIDTH, 10, score1 + '0', 0x00, false);
    drawCharacter(SCREEN_WIDTH/2 + CHARACTER_WIDTH, 10, score2 + '0', 0x00, false);

}

/**
 * Draws the moving elements on the screen
 */
void drawMovingElements(Paddle* paddle1, Paddle* paddle2, Ball* ball){

    drawRectangle(paddle1->x, paddle1->y, paddle1->width, paddle1->height, 0xFF);
    drawRectangle(paddle2->x, paddle2->y, paddle2->width, paddle2->height, 0xFF);
    drawRectangle(ball->x, ball->y, ball->width, ball->height, 0xFF);

    drawCharacter(SCREEN_WIDTH/2 - CHARACTER_WIDTH, 10, score1 + '0', 0xFF, false);
    drawCharacter(SCREEN_WIDTH/2, 10,':', 0xFF,false);
    drawCharacter(SCREEN_WIDTH/2 + CHARACTER_WIDTH, 10, score2 + '0', 0xFF, false);

    frameBuffer();
}


/**
 * Enables input with the game
 */
void input(Paddle* paddle1, Paddle* paddle2, Ball* ball){
  int switchInput = get_sw();

  paddle1->up = switchInput & 0x200;

  paddle1->down = switchInput & 0x100; 

  if(gameState == PVP){
    paddle2->up = switchInput & 0x2;
    paddle2->down = switchInput & 0x1;
  }
  else{
    int deadZone = 15;
    int ballDistance = SCREEN_WIDTH/2;
    
    paddle2->up = (((paddle2->y + paddle2->height/2) - (ball->y + ball->height/2)) > deadZone) 
    && (paddle2->x - (ball->x + ball->width))< ballDistance;
    paddle2->down = ((ball->y + ball->height/2) - (paddle2->y + paddle2->height/2 ) > deadZone)
      && (paddle2->x - (ball->x + ball->width))< ballDistance;
  }
}


/**
 * Handles the movement of a paddle
 */
void paddleMovement(Paddle * paddle1){
  if(paddle1->up && paddle1->y > 0)
    paddle1->y -= PADDLE_MOVEMENT_SPEED;
  
  if(paddle1->down && (paddle1->y + paddle1->height) < SCREEN_HEIGHT)
      paddle1->y += PADDLE_MOVEMENT_SPEED;
}
/**
 * Resets the ball to the center of the screen
 */
void resetBall(Ball* ball){
  ball->x = SCREEN_WIDTH/2 - ball->width/2;
  ball->y = SCREEN_HEIGHT/2 - ball->height/2;
  ball->movY = -ball->movY;
  ball->movX = -ball->movX;
}


  void pointScored(Ball * ball, int* score){
      resetBall(ball);
      (*score)++;
      if(score == &score1)
        set_display(4, score1);
      else
         set_display(0, score2);
        
      if(*score == MAX_SCORE){
        // score1 = 0;
        // score2 = 0;
        gameState = WINNER_SCREEN;
        //gameState = MENU;
      }
  }


  void ballMovement(Ball* ball){

    if(ball->x <= 0){
      pointScored(ball, &score2);
    }

    else if (ball->x + ball->width >= SCREEN_WIDTH){
      pointScored(ball, &score1);
    }

     
    if(ball->y < 0 || (ball->y + ball->height) > SCREEN_HEIGHT){
      ball->movY = -ball->movY;
    }
      ball->y += ball->movY;
      ball->x += ball->movX;

  }

  void resetGame(){
    score1 = 0;
    score2 = 0;
  }

  
  void paddleCollision(Paddle* paddle1, Ball* ball){
   if( paddle1->y < (ball->y + ball->height)
    && ball->y < (paddle1->y + paddle1->height)
    && paddle1->x < (ball->x + ball->width) 
    && ball->x < (paddle1->x + paddle1->width)){
      ball->movX = -ball->movX;

      if(ball->x <SCREEN_WIDTH/2)
        ball->x += ball->width;
      else
        ball->x -= ball->width;
   }
  }

  /**
   * Handles the main game loop
   */
  void runGameLoop(){
    score1 = 0;
    score2 = 0;

    Paddle paddle1;
    paddle1.width = SCREEN_WIDTH/PADDLE_RELATIVE_WIDTH;
    paddle1.height = SCREEN_HEIGHT/PADDLE_RELATIVE_HEIGHT;
    paddle1.x = PADDLE_X_OFFSET;
    paddle1.y = SCREEN_HEIGHT/2 - paddle1.height/2;

    Paddle paddle2;
    paddle2.width = SCREEN_WIDTH/PADDLE_RELATIVE_WIDTH;
    paddle2.height = SCREEN_HEIGHT/PADDLE_RELATIVE_HEIGHT;
    paddle2.x = SCREEN_WIDTH - paddle2.width - PADDLE_X_OFFSET;
    paddle2.y = SCREEN_HEIGHT/2 - paddle2.height/2;

    Ball ball;
    ball.height = 4;
    ball.width = 4;
    ball.x = SCREEN_WIDTH/2 - ball.width/2;
    ball.y = SCREEN_HEIGHT/2 - ball.height/2;
    ball.movY = BALL_MOVEMENT_SPEED;
    ball.movX = 1;

    clearBuffer();


    int blankDisplay = -1;
    for(int i = 0; i<6; i++){
      if(i == 3)
        set_display(i, blankDisplay);
      else if(i == 2)
        set_display(i, blankDisplay);
      else
        set_display(i,0);
    }

    set_leds(0x303);

     unsigned int begin[8];
    unsigned int counters[8];

    while(gameState == PVP || gameState == PVC){
      if(nextFrame){

       reset_counters(begin);

        clearMovingElements(&paddle1, &paddle2, &ball);
    
        input(&paddle1, &paddle2, &ball);

        paddleMovement(&paddle1);

        paddleMovement(&paddle2);
        
        ballMovement(&ball);

        paddleCollision(&paddle1, &ball);

        paddleCollision(&paddle2, &ball);
        
        drawMovingElements(&paddle1, &paddle2, &ball);

        nextFrame = 0;

        read_counters(counters);

        if(prints){
          prints = false;
        for(int i = 0; i<8; i++){
          print_dec(counters[i] - begin[i]);
          print("\n");
        }
         }
        
        break;
      }
    }
  }

 
  