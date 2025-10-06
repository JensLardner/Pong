#include "util.h"
#include "globals.h"
#include "bitmaps.h"



void handle_interrupt(unsigned cause) 
{
  volatile unsigned short* Timeout_Status = (volatile unsigned short*) 0x04000020;
  *Timeout_Status = 0;
  nextFrame = 1;
}

/* Add your code here for initializing interrupts. */
void labinit(void)
{
  volatile unsigned short*  Period_Low = (unsigned short *) 0x04000028;

  volatile unsigned short*  Period_High = (unsigned short *) 0x0400002C;

  *Period_Low = (29999999/60) & 0xFFFF;

  *Period_High = (29999999/60) >> 16;

   volatile unsigned short* Control = (volatile unsigned short*) 0x04000024;

  *Control = 0x7;

  enable_interrupt();

}

void set_leds(int led_mask){
  volatile int* p = (volatile int*)0x04000000;
  *p = led_mask & 0x3FF;
}

void set_display(int display_number, int value){
  volatile int* displayAddress = (volatile int*)(0x04000050 + 0x10 * display_number);

  switch(value){

    //case -2:
    //    *displayAddress = 0xF9;
    //    break;
    case -1: 
        //*displayAddress = 0xCF;
        *displayAddress = 0xFF;
        break;
    case 0:
        *displayAddress = 0x40;
        break;
    case 1: 
         *displayAddress = 0x79;
        break;
    case 2:
      *displayAddress = 0x24;
        break;
    case 3:
      *displayAddress = 0x30;
        break;
    case 4: 
      *displayAddress = 0x19;
        break;
    case 5:
    *displayAddress = 0x12;
        break;
    case 6:
      *displayAddress = 0x02;
        break;
    case 7: 
    *displayAddress = 0x78;
        break;
    case 8:
      *displayAddress = 0x00;
        break;
    case 9:
      *displayAddress = 0x10;
        return;
    default: return;

  }
}

int get_sw(){
  volatile int* address = (volatile int*) 0x04000010;
  return *address & 0x3FF;
}

int get_btn(){
 volatile int* address = (volatile int*) 0x040000d0;
  return *address & 0x1;
  }

void drawCharacter(int x, int y, char character, char color ){
 for(int bitmapRow = 0; bitmapRow < 8; bitmapRow++){
        char pixels = font8x8_basic[(int)character][bitmapRow];
        for(int bitmapColumn = 0; bitmapColumn<8; bitmapColumn++){
           if (pixels & (0x1 << bitmapColumn)) {
           frame[(y + bitmapRow) * SCREEN_WIDTH + x + bitmapColumn] = color;
           }
        }
      }
}

void drawRectangle(int x, int y, int width, int height, char color){
      for(int i = y; i < y + height; i++){
        for(int j = x; j< x + width; j++){
            frame[i*SCREEN_WIDTH + j] = color; 
        }
      }
}


void frameBuffer(){
    *(DMA_Control+1) = (unsigned int) (frame);
    *(DMA_Control+0) = 0; 
}
