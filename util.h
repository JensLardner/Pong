#ifndef UTIL_H
#define UTIL_H

extern void enable_interrupt();

void handle_interrupt(unsigned cause);

void labinit(void);

void set_leds(int led_mask);

void set_display(int display_number, int value);

int get_sw();

int get_btn();

void drawCharacter(int x, int y, char character, char color );

void drawRectangle(int x, int y, int width, int height, char color);

void frameBuffer();


#endif 