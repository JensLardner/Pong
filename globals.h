//Jens
#ifndef GLOBALS_H
#define GLOBALS_H

#define CHARACTER_WIDTH 8
#define CHARACTER_HEIGHT 8

#include <stdbool.h>

extern volatile unsigned char* frame;
extern volatile int* DMA_Control;

extern unsigned short SCREEN_WIDTH;
extern unsigned short SCREEN_HEIGHT;

typedef enum{MENU, PVP, PVC, WINNER_SCREEN}GameState;

extern GameState gameState;

extern volatile char nextFrame;
extern int score1;
extern int score2;

#endif 