/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
#include "bitmaps.h"
#include <stdbool.h>
#include <string.h>

#define MAX_SCORE 9

#define PADDLE_X_OFFSET 10
#define PADDLE_RELATIVE_HEIGHT 5
#define PADDLE_RELATIVE_WIDTH 40
#define PADDLE_MOVEMENT_SPEED 1

#define BALL_MOVEMENT_SPEED 1

#define CHARACTER_WIDTH 8
#define CHARACTER_HEIGHT 8

extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );
extern void enable_interrupt();
void set_display(int, int);


volatile unsigned char* frame = (volatile unsigned char*) 0x8000000;
volatile int* DMA_Control = (volatile int *) 0x4000100;


unsigned short SCREEN_WIDTH;
unsigned short SCREEN_HEIGHT;

bool running = true;
bool inMenu = true;
enum {MENU, PVP, PVC} gameState;

volatile char nextFrame = 1;
int score1;
int score2;

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

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{
  volatile unsigned short* Timeout_Status = (volatile unsigned short*) 0x04000020;
  *Timeout_Status = 0;
  nextFrame = 1;
}

/* Add your code here for initializing interrupts. */
void labinit(void)
{
  volatile unsigned short*  Period_Low = (unsigned short *) 0x04000028;

  volatile unsigned short*  Period_High = (unsigned short *) 0x0400002C;

  *Period_Low = (29999999/60) & 0xFFFF;

  *Period_High = (29999999/60) >> 16;

   volatile unsigned short* Control = (volatile unsigned short*) 0x04000024;

  *Control = 0x7;

  enable_interrupt();

}

void set_leds(int led_mask){
  volatile int* p = (volatile int*)0x04000000;
  *p = led_mask & 0x3FF;
}

void set_display(int display_number, int value){
  volatile int* displayAddress = (volatile int*)(0x04000050 + 0x10 * display_number);

  switch(value){

    //case -2:
    //    *displayAddress = 0xF9;
    //    break;
    case -1: 
        //*displayAddress = 0xCF;
        *displayAddress = 0xFF;
        break;
    case 0:
        *displayAddress = 0x40;
        break;
    case 1: 
         *displayAddress = 0x79;
        break;
    case 2:
      *displayAddress = 0x24;
        break;
    case 3:
      *displayAddress = 0x30;
        break;
    case 4: 
      *displayAddress = 0x19;
        break;
    case 5:
    *displayAddress = 0x12;
        break;
    case 6:
      *displayAddress = 0x02;
        break;
    case 7: 
    *displayAddress = 0x78;
        break;
    case 8:
      *displayAddress = 0x00;
        break;
    case 9:
      *displayAddress = 0x10;
        return;
    default: return;

  }
}

int get_sw(){
  volatile int* address = (volatile int*) 0x04000010;
  return *address & 0x3FF;
}

int get_btn(){
 volatile int* address = (volatile int*) 0x040000d0;
  return *address & 0x1;
  }

void drawCharacter(int x, int y, char character, char color ){
 for(int bitmapRow = 0; bitmapRow < 8; bitmapRow++){
        char pixels = font8x8_basic[(int)character][bitmapRow];
        for(int bitmapColumn = 0; bitmapColumn<8; bitmapColumn++){
           if (pixels & (0x1 << bitmapColumn)) {
           frame[(y + bitmapRow) * SCREEN_WIDTH + x + bitmapColumn] = color;
           }
        }
      }

}

void drawRectangle(int x, int y, int width, int height, char color){
      for(int i = y; i < y + height; i++){
        for(int j = x; j< x + width; j++){
            frame[i*SCREEN_WIDTH + j] = color; 
        }
      }
}

void clearBackBuffer(Paddle* paddle1, Paddle* paddle2, Ball* ball){
    drawRectangle(paddle1->x, paddle1->y, paddle1->width, paddle1->height, 0x00);
    drawRectangle(paddle2->x, paddle2->y, paddle2->width, paddle2->height, 0x00);
    drawRectangle(ball->x, ball->y, ball->width, ball->height, 0x00);

    drawCharacter(SCREEN_WIDTH/2 - CHARACTER_WIDTH, 10, score1 + '0', 0x00);
    drawCharacter(SCREEN_WIDTH/2 + CHARACTER_WIDTH, 10, score2 + '0', 0x00);

}

void frameBuffer(){
    *(DMA_Control+1) = (unsigned int) (frame);
    *(DMA_Control+0) = 0; 
}

void drawMovingElements(Paddle* paddle1, Paddle* paddle2, Ball* ball){

    drawRectangle(paddle1->x, paddle1->y, paddle1->width, paddle1->height, 0xFF);
    drawRectangle(paddle2->x, paddle2->y, paddle2->width, paddle2->height, 0xFF);
    drawRectangle(ball->x, ball->y, ball->width, ball->height, 0xFF);

    drawCharacter(SCREEN_WIDTH/2 - CHARACTER_WIDTH, 10, score1 + '0', 0xFF);
    drawCharacter(SCREEN_WIDTH/2, 10,':', 0xFF);
    drawCharacter(SCREEN_WIDTH/2 + CHARACTER_WIDTH, 10, score2 + '0', 0xFF);

    frameBuffer();

}



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
        paddle2->up = ((paddle2->y + paddle2->height/2) - (ball->y + ball->height/2)) > deadZone;
        paddle2->down = (ball->y + ball->height/2) - (paddle2->y + paddle2->height/2 ) > deadZone;
       }
  }


  void paddleMovement(Paddle * paddle1){

    if(paddle1->up && paddle1->y > 0)
      paddle1->y -= PADDLE_MOVEMENT_SPEED;
    
    if(paddle1->down && (paddle1->y + paddle1->height) < SCREEN_HEIGHT)
        paddle1->y += PADDLE_MOVEMENT_SPEED;
    

  }

  inline void reset(Ball* ball){
    ball->x = SCREEN_WIDTH/2 - ball->width/2;
    ball->y = SCREEN_HEIGHT/2 - ball->height/2;
    ball->movY = -ball->movY;
    ball->movX = -ball->movX;
  }

  void ballMovement(Ball* ball, int * score1, int * score2){

    if(ball->x <= 0){
      reset(ball);
      (*score2)++;
      set_display(0,*score2);
      if(*score2 == MAX_SCORE)
        running = false;
    }

    else if (ball->x + ball->width >= SCREEN_WIDTH){
      reset(ball);
       (*score1)++;
      set_display(4,*score1);
      if(*score1 == MAX_SCORE)
        running = false;
    }

     
    if(ball->y < 0 || (ball->y + ball->height) > SCREEN_HEIGHT){
      ball->movY = -ball->movY;
    }
      ball->y += ball->movY;
      ball->x += ball->movX;

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

  int abs(int x){
    return (x>0)?x:-x;
  }


/**
 * Not sure if this is needed.
 */
void clearBackBufferMenu(){
 for(int i = 0; i<SCREEN_HEIGHT*SCREEN_HEIGHT; i++){
        frame[i] = 0x00;
    }
}



void drawMenuItem(char* item, int position){
    int i = 0;
    while(item[i] != 0){
      drawCharacter(10 + i*CHARACTER_WIDTH, CHARACTER_HEIGHT*position, item[i], 0xFF);
      i++;
    }
}
  
/**
 * Draws the menu on the screen
 */
void drawMenu(int activeMenuItem){
    //clearBackBufferMenu();
    //int characterHeight = 8;
    //int characterWidth = 8;
    int numberOfMenuItems = 3;
    char selectedItemColor = 0xE0;
    
  
    for(int i = 0; i<numberOfMenuItems; i++){
      if(i == activeMenuItem)
        drawRectangle(0, i*CHARACTER_HEIGHT, SCREEN_WIDTH, CHARACTER_HEIGHT, selectedItemColor);
      else
        drawRectangle(0, i*CHARACTER_HEIGHT, SCREEN_WIDTH, CHARACTER_HEIGHT, 0x00);
    }

    char* menuItems[] = {"PLAYER VS PLAYER", "PLAYER VS CPU", "EXIT"};
    
    //for(int i = 0; i<strlen(menuItems[1]); i++)
    //  drawCharacter(10 + i*CHARACTER_WIDTH, 0, menuItems[1][i], 0xFF);
    for(int i = 0; i<numberOfMenuItems; i++)
      drawMenuItem(menuItems[i], i);
    
    frameBuffer();
  }

  


  /**
   * Handles the main game loop
   */
  void runGameLoop(){
  

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

    for(int i = 0; i<SCREEN_HEIGHT*SCREEN_WIDTH; i++){
      frame[i] = 0x00;
    }

    score1 = 0;
    score2 = 0;

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

    //game loop
    while(running){

      if(nextFrame){

        clearBackBuffer(&paddle1, &paddle2, &ball);
    
        input(&paddle1, &paddle2, &ball);

        paddleMovement(&paddle1);

        paddleMovement(&paddle2);
        
        ballMovement(&ball, &score1, &score2);

        paddleCollision(&paddle1, &ball);

        paddleCollision(&paddle2, &ball);
        
        drawMovingElements(&paddle1, &paddle2, &ball);

        nextFrame = 0;
      }
    }
  }

  /* Your code goes into main as well as any needed functions. */
 int main ( void ) {

    labinit();
    SCREEN_WIDTH = *(DMA_Control + 2) & 0xFFFF; 
    SCREEN_HEIGHT = (*(DMA_Control + 2) >> 16) & 0xFFFF;

    gameState = MENU;

    int activeMenuItem = 0;
    int prevSwitch = 0;
    while(gameState == MENU){

      int switchInput = get_sw();
      int currentSwitch = switchInput & 0x1;
      
      if(currentSwitch && !prevSwitch){
        activeMenuItem = (activeMenuItem + 1) % 3;
      }
      prevSwitch = currentSwitch;

      int btnInput = get_btn();
      if(btnInput){
        if(activeMenuItem == 0){
          gameState = PVP;
        }
        else if(activeMenuItem == 1){
          gameState = PVC;
        }
        else if(activeMenuItem == 2){
          return 0;
        }
      }

      if(nextFrame){
        drawMenu(activeMenuItem);
        nextFrame = 0;
      }
    }
    
    if(gameState == PVP || gameState == PVC)
      runGameLoop();
}




