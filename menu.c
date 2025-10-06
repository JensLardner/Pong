#include "menu.h"
#include "globals.h"
#include "util.h"

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