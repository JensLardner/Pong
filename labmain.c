/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
#include "globals.h"
#include "util.h"
#include "menu.h"
#include "game.h"

extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );
extern void enable_interrupt();
void set_display(int, int);


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




