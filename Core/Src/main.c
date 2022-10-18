#include "Sys.h"
#include "neopixel.h"
#include "stm32l0xx_it.h"
#include "fx.h"
#include <stdint.h>

uint8_t INIT_DATA_BUFFER[ PIXEL_NUMBER][3] = {{15,50,0},{15,50,0},{15,50,0},{15,50,0},{15,50,0},{15,50,0},{15,50,0},{15,50,0},{15,50,0},{15,50,0}};

int main(void)
{
  uint32_t last_tick;
  InitSys();
  clearData(PIXEL_NUMBER);
  for (uint8_t i = 0; i < PIXEL_NUMBER; i++) {
    uint32_t initTick = getTicks();
    while (getTicks() < initTick+300);
    writeData(INIT_DATA_BUFFER, i);
  }
  
  while(1){
    last_tick = getTicks();
    if (getTicks() > last_tick + 5) {
      //fxFunction();
    }
    writeData(INIT_DATA_BUFFER, (uint32_t)PIXEL_NUMBER);
    //writeData();
  }
}
