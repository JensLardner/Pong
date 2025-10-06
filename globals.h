#ifndef GLOBALS_H
#define GLOBALS_H

#define CHARACTER_WIDTH 8
#define CHARACTER_HEIGHT 8

#include <stdbool.h>

extern volatile unsigned char* frame;
extern volatile int* DMA_Control;

extern unsigned short SCREEN_WIDTH;
extern unsigned short SCREEN_HEIGHT;

extern bool running;
extern bool inMenu;

typedef enum{MENU, PVP, PVC}GameState;

extern GameState gameState;

extern volatile char nextFrame;
extern int score1;
extern int score2;

#endif 