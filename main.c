/* main.c

   This file written 2025 by Jens Lardner and Tahmid Lasker

   For copyright and licensing, see file COPYING */

#include "globals.h"
#include "util.h"
#include "menu.h"
#include "game.h"

/* Jens & Tahmid
* The main function
*/
 int main ( void ) {

  init();
  SCREEN_WIDTH = *(DMA_Control + 2) & 0xFFFF; 
  SCREEN_HEIGHT = (*(DMA_Control + 2) >> 16) & 0xFFFF;

  gameState = MENU;

  while(1){
    if(gameState == MENU)
      menuInteraction();
  
    if(gameState == PVP || gameState == PVC)
      runGameLoop();

    if(gameState == WINNER_SCREEN ){
      clearBuffer();
      char* winnerText = score1 > score2 ? "PLAYER 1 WINS!" : "PLAYER 2 WINS!";
      drawWinner(winnerText, stringLength(winnerText));
    }
  }
}






