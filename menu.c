#include "menu.h"
#include "globals.h"
#include "util.h"

/**
 * Handles the interaction in the menu
 */
void menuInteraction(){
  int playerVsPlayer = 0;
  int playerVsCPU = 1;
  int activeMenuItem = playerVsPlayer;

  int prevSwitch = 0;
  while(gameState == MENU){
    if(nextFrame){
      nextFrame = 0;
      int switchInput = get_sw();
      int currentSwitch = switchInput & 0x1;
      
      if(currentSwitch && !prevSwitch){
        activeMenuItem = (activeMenuItem + 1) % 2;
        
      }
      prevSwitch = currentSwitch;

      int btnInput = get_btn();
      if(btnInput){
        if(activeMenuItem == playerVsPlayer){
          gameState = PVP;
        }
        else if(activeMenuItem == playerVsCPU){
          gameState = PVC;
        }
      }

      drawMenu(activeMenuItem);
    }
  }
}

/**
 * Draws a single menu item on the screen
 */
void drawMenuItem(char* item, int position){
  int i = 0;
  bool scaleBy2 = false;
  while(item[i] != 0){
    drawCharacter(10 + i*CHARACTER_WIDTH, CHARACTER_HEIGHT*position, item[i], 0xFF, scaleBy2);
    i++;
  }
}
  
/**
 * Draws the menu on the screen
 */
void drawMenu(int activeMenuItem){
  set_leds(1);
  int numberOfMenuItems = 2;
  char selectedItemColor = 0xE0;
  
  for(int i = 0; i<numberOfMenuItems; i++){
    if(i == activeMenuItem)
      drawRectangle(0, i*CHARACTER_HEIGHT, SCREEN_WIDTH, CHARACTER_HEIGHT, selectedItemColor);
    else
      drawRectangle(0, i*CHARACTER_HEIGHT, SCREEN_WIDTH, CHARACTER_HEIGHT, 0x00);
  }

  char* menuItems[] = {"PLAYER VS PLAYER", "PLAYER VS CPU"};

  for(int i = 0; i<numberOfMenuItems; i++)
    drawMenuItem(menuItems[i], i);
  
  frameBuffer();
}

/**
 * Draws the winner text on the screen
 */
void drawWinner(char* winner, int stringLength){
  
  bool scaleBy2 = true;
  
  int i = 0;
  while(winner[i] != 0){
    int x = SCREEN_WIDTH/2 - (CHARACTER_WIDTH * stringLength);
    drawCharacter(x+CHARACTER_WIDTH*2*i, SCREEN_HEIGHT/2 - CHARACTER_HEIGHT ,winner[i], 0xE0, scaleBy2);
    i++;
  }
  delay(10000);
  clearBuffer();
  gameState = MENU;
}



