/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
#include "globals.h"
#include "util.h"
#include "menu.h"
#include "game.h"





  /* Your code goes into main as well as any needed functions. */
 int main ( void ) {

    labinit();
    SCREEN_WIDTH = *(DMA_Control + 2) & 0xFFFF; 
    SCREEN_HEIGHT = (*(DMA_Control + 2) >> 16) & 0xFFFF;

    gameState = MENU;

    while(1){
      if(gameState == MENU){
        clearBuffer();
        menuInteraction();
      }
    
      if(gameState == PVP || gameState == PVC)
        runGameLoop();
    }
    
}






