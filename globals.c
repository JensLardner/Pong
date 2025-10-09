//Jens
#include "globals.h"
//frame buffer, pixel values wrriten into this buffer
volatile unsigned char* frame = (volatile unsigned char*) 0x8000000; 
//direct memory access
volatile int* DMA_Control = (volatile int *) 0x4000100; 

unsigned short SCREEN_WIDTH;
unsigned short SCREEN_HEIGHT;

GameState gameState = MENU;

volatile char nextFrame = 1;
int score1 = 0;
int score2 = 0;