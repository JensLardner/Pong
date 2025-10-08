#ifndef MENY_H
#define MENY_H

void menuInteraction();

void drawMenuItem(char* item, int position);
  
void drawMenu(int activeMenuItem);

void drawWinner(char* winner, int length);

void handleMenuSelection();

void updateMenuSelection();

#endif