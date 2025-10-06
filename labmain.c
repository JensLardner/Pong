/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
#include "bitmaps.h"
#include <stdbool.h>

#define MAX_SCORE 9

#define PADDLE_X_OFFSET 10
#define PADDLE_RELATIVE_HEIGHT 5
#define PADDLE_RELATIVE_WIDTH 40
#define PADDLE_MOVEMENT_SPEED 1

#define BALL_MOVEMENT_SPEED 1

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
volatile char nextFrame = 1;

typedef struct{
  short x;
  int y;
  int width;
  int height;
  bool up;
  bool down;
}paddle;

typedef struct{
  int x;
  int y;
  int width;
  int height;
  int movX;
  int movY;
}ball;

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
 for(int i = 0; i < 8; i++){
        char pixels = font8x8_basic[(int)character][i];
        for(int j = 0; j<8; j++){
           if (pixels & (1 << j)) {
           frame[(y + i) * SCREEN_WIDTH + x + j] = color;
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

void clearBackBuffer(paddle* paddle1, paddle* paddle2, ball* ball1){
    drawRectangle(paddle1->x, paddle1->y, paddle1->width, paddle1->height, 0x00);
    drawRectangle(paddle2->x, paddle2->y, paddle2->width, paddle2->height, 0x00);
    drawRectangle(ball1->x, ball1->y, ball1->width, ball1->height, 0x00);
}

void draw(paddle* paddle1, paddle* paddle2, ball* ball1){

    drawRectangle(paddle1->x, paddle1->y, paddle1->width, paddle1->height, 0xFF);
    drawRectangle(paddle2->x, paddle2->y, paddle2->width, paddle2->height, 0xFF);
    drawRectangle(ball1->x, ball1->y, ball1->width, ball1->height, 0xFF);

    *(DMA_Control+1) = (unsigned int) (frame);
    *(DMA_Control+0) = 0; 

}



  void input(paddle* paddle1, paddle* paddle2){

      int switchInput = get_sw();
     
        paddle1->up = switchInput & 0x200;

        paddle1->down = switchInput & 0x100; 

        paddle2->up = switchInput & 0x2;

        paddle2->down = switchInput & 0x1;
  }


  void paddleMovement(paddle * paddle1){

    if(paddle1->up && paddle1->y > 0)
      paddle1->y -= PADDLE_MOVEMENT_SPEED;
    
    if(paddle1->down && (paddle1->y + paddle1->height) < SCREEN_HEIGHT)
        paddle1->y += PADDLE_MOVEMENT_SPEED;
    

  }

  inline void reset(ball* ball1){
    ball1->x = SCREEN_WIDTH/2 - ball1->width/2;
    ball1->y = SCREEN_HEIGHT/2 - ball1->height/2;
    ball1->movY = -ball1->movY;
    ball1->movX = -ball1->movX;
  }

  void ballMovement(ball* ball1, int * score1, int * score2){

    if(ball1->x <= 0){
      reset(ball1);
      (*score2)++;
      set_display(0,*score2);
      if(*score2 == MAX_SCORE)
        running = false;
    }

    else if (ball1->x + ball1->width >= SCREEN_WIDTH){
      reset(ball1);
       (*score1)++;
      set_display(4,*score1);
      if(*score1 == MAX_SCORE)
        running = false;
    }

     
    if(ball1->y < 0 || (ball1->y + ball1->height) > SCREEN_HEIGHT){
      ball1->movY = -ball1->movY;
    }
      ball1->y += ball1->movY;
      ball1->x += ball1->movX;

  }

  void paddleCollision(paddle* paddle1, ball* ball1){
   if( paddle1->y < (ball1->y + ball1->height)
    && ball1->y < (paddle1->y + paddle1->height)
    && paddle1->x < (ball1->x + ball1->width) 
    && ball1->x < (paddle1->x + paddle1->width)){
      ball1->movX = -ball1->movX;
      ball1->x += ball1->movX;
   }
  }

clearBackBufferMenu( ){
  
    for(int i = 0; i<numberOfMenuItems; i++){
      if(i == activeMenuItem)
        drawRectangle(0, i*characterHeight, SCREEN_WIDTH, characterHeight, selectedItemColor);
      else
        drawRectangle(0, i*characterHeight, SCREEN_WIDTH, characterHeight, 0x00);
    }

  
}
  
  void drawMenu(int activeMenuItem){

    int characterHeight = 8;
    int characterWidth = 8;
    int numberOfMenuItems = 3;
    char selectedItemColor = 0xE0;
    //char* menuItems[] = {"PLAYER VS CPU", "PLAYER VS PLAYER", "EXIT"};
    
    for(int j = 0; j<16; j++)
      drawCharacter(10 + j*characterWidth, 0, "PLAYER VS PLAYER"[j], 0xFF);
    

    for(int i = 0; i<numberOfMenuItems; i++){
      if(i == activeMenuItem)
        drawRectangle(0, i*characterHeight, SCREEN_WIDTH, characterHeight, selectedItemColor);
      else
        drawRectangle(0, i*characterHeight, SCREEN_WIDTH, characterHeight, 0x00);
    }

    *(DMA_Control+1) = (unsigned int) (frame);
    *(DMA_Control+0) = 0; 
  }


  void runGameLoop(){
  

    paddle paddle1;
    paddle1.width = SCREEN_WIDTH/PADDLE_RELATIVE_WIDTH;
    paddle1.height = SCREEN_HEIGHT/PADDLE_RELATIVE_HEIGHT;
    paddle1.x = PADDLE_X_OFFSET;
    paddle1.y = SCREEN_HEIGHT/2 - paddle1.height/2;

    paddle paddle2;
    paddle2.width = SCREEN_WIDTH/PADDLE_RELATIVE_WIDTH;
    paddle2.height = SCREEN_HEIGHT/PADDLE_RELATIVE_HEIGHT;
    paddle2.x = SCREEN_WIDTH - paddle2.width - PADDLE_X_OFFSET;
    paddle2.y = SCREEN_HEIGHT/2 - paddle2.height/2;

    ball ball;
    ball.height = 4;
    ball.width = 4;
    ball.x = SCREEN_WIDTH/2 - ball.width/2;
    ball.y = SCREEN_HEIGHT/2 - ball.height/2;
    ball.movY = BALL_MOVEMENT_SPEED;
    ball.movX = 1;

    for(int i = 0; i<SCREEN_HEIGHT*SCREEN_WIDTH; i++){
      frame[i] = 0x00;
    }

    int score1 = 0;
    int score2 = 0;

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
    
        input(&paddle1, &paddle2);

        paddleMovement(&paddle1);

        paddleMovement(&paddle2);
        
        ballMovement(&ball, &score1, &score2);

        paddleCollision(&paddle1, &ball);

        paddleCollision(&paddle2, &ball);
        
        draw(&paddle1, &paddle2, &ball);

        nextFrame = 0;
      }
    }
  }

  /* Your code goes into main as well as any needed functions. */
 int main ( void ) {

    labinit();
    SCREEN_WIDTH = *(DMA_Control + 2) & 0xFFFF; 
    SCREEN_HEIGHT = (*(DMA_Control + 2) >> 16) & 0xFFFF;

    int activeMenuItem = 0;
    while(inMenu){
  
      drawMenu(activeMenuItem);
    }
    

    runGameLoop();
}




