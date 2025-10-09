#include "menu.h"
#include "globals.h"
#include "util.h"

#define PLAYER_VS_PLAYER 0
#define PLAYER_VS_CPU 1

static int activeMenuItem = PLAYER_VS_PLAYER;
static int prevSwitch = 0;

/** Tahmid
 * Handles the interaction in the menu
 */
void menuInteraction()
{
  clearBuffer();
  frameBuffer();

  while (gameState == MENU)
  {
    if (nextFrame)
    {
      nextFrame = 0;
      updateMenuSelection();
      handleMenuSelection();
      drawMenu(activeMenuItem);
    }
  }
}

/** Tahmid
 * Updates the active menu item
 */
void updateMenuSelection()
{
  int rightmostSwitch = 0x1;
  int numberOfMenuItems = 2;

  int switchInput = get_sw();
  int currentSwitch = switchInput & rightmostSwitch;

  if (currentSwitch && !prevSwitch)
  {
    activeMenuItem = (activeMenuItem + 1) % numberOfMenuItems;
  }
  prevSwitch = currentSwitch;
}

/** Tahmid
 * Handles the selection of a menu item
 */
void handleMenuSelection()
{
  int btnInput = get_btn();
  if (btnInput)
  {
    if (activeMenuItem == PLAYER_VS_PLAYER)
    {
      gameState = PVP;
    }
    else if (activeMenuItem == PLAYER_VS_CPU)
    {
      gameState = PVC;
    }
  }
}

/** Tahmid
 * Draws a single menu item on the screen
 */
void drawMenuItem(char *item, int position)
{
  int nullTerminator = 0;
  int white = 0xFF;
  bool scaleBy2 = false;

  int i = 0;
  while (item[i] != nullTerminator) 
  {
    drawCharacter(10 + i * CHARACTER_WIDTH, CHARACTER_HEIGHT * position, item[i], white, scaleBy2);
    i++;
  }
}

/** Tahmid
 * Draws the menu on the screen
 */
void drawMenu(int activeMenuItem)
{
  int rightmostLED = 0x1;
  set_leds(rightmostLED);
  int numberOfMenuItems = 2;
  char selectedItemColor = 0xE0;
  int black = 0x00;

  for (int i = 0; i < numberOfMenuItems; i++)
  {
    if (i == activeMenuItem)
      drawRectangle(0, i * CHARACTER_HEIGHT, SCREEN_WIDTH, CHARACTER_HEIGHT, selectedItemColor);
    else //not selected
      drawRectangle(0, i * CHARACTER_HEIGHT, SCREEN_WIDTH, CHARACTER_HEIGHT, black);
  }

  char* menuItems[] = {"PLAYER VS PLAYER", "PLAYER VS CPU"};

  for (int i = 0; i < numberOfMenuItems; i++)
    drawMenuItem(menuItems[i], i);

  frameBuffer();
}
 
/** Jens & Tahmid 
 * Draws the winner text on the screen
 */
void drawWinner(char *winner, int stringLength)
{
  int nullTerminator = 0;
  int red = 0xE0;
  bool scaleBy2 = true;
  
  int i = 0;
  while (winner[i] != nullTerminator)
  {
    int x = SCREEN_WIDTH / 2 - (CHARACTER_WIDTH * stringLength);
    drawCharacter(x + CHARACTER_WIDTH * 2 * i, SCREEN_HEIGHT / 2 - CHARACTER_HEIGHT, winner[i], red, scaleBy2);
    i++;
  }

  int millisecondsToShowWinner = 3000;
  delay(millisecondsToShowWinner);
  clearBuffer();
  gameState = MENU;
}
