//Jens & Tahmid
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );
extern void enable_interrupt();
void set_display(int, int);

void handle_interrupt(unsigned cause);

void init(void);

void set_leds(int led_mask);

void set_display(int display_number, int value);

int get_sw();

int get_btn();

void clearMovingElements();

void drawCharacter(int x, int y, char character, char color, bool scale );

void drawRectangle(int x, int y, int width, int height, char color);

void frameBuffer();

void clearBuffer();

int stringLength(char* str);

#endif 