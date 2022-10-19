#include "Sys.h"
#include "neopixel.h"
#include "stm32l0xx_it.h"
#include "fx.h"
#include <stdint.h>

/* Check for an input press. Debounced. Returns 1 if pressed, 0 if not. */
uint8_t checkInput(void);

uint8_t INIT_DATA_BUFFER[PIXEL_NUMBER][3] = {
  {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0},
  {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0}};
uint8_t ALT_DATA_BUFFER[PIXEL_NUMBER][3] = {
  {15, 50, 100}, {0, 50, 100}, {100, 0, 10}, {20, 150, 75}, {200, 50, 10},
  {15, 125, 5}, {50, 100, 100}, {15, 50, 100}, {5, 25, 150},{100, 10, 75}};
uint8_t (*ACTIVE_DATA_BUFFER)[][3] = &INIT_DATA_BUFFER;

int main(void)
{
  uint32_t last_tick;
  InitSys();
  clearData(PIXEL_NUMBER);
  for (uint8_t i = 0; i <= PIXEL_NUMBER; i++) {
    uint32_t initTick = getTicks();
    while (getTicks() < initTick+300);
    writeData(*ACTIVE_DATA_BUFFER, i);
  }
  //writeData(*ACTIVE_DATA_BUFFER, (uint32_t)PIXEL_NUMBER);
  while(1){
    if(checkInput()){
      if (ACTIVE_DATA_BUFFER == &INIT_DATA_BUFFER){
	ACTIVE_DATA_BUFFER = &ALT_DATA_BUFFER;
      } else {
	ACTIVE_DATA_BUFFER = &INIT_DATA_BUFFER;
      }
      clearData(PIXEL_NUMBER);
      for (uint8_t i = 0; i <= PIXEL_NUMBER; i++) {
	uint32_t initTick = getTicks();
	while (getTicks() < initTick+300);
	writeData(*ACTIVE_DATA_BUFFER, i);
      }
    }
  }
}

uint8_t checkInput(void) {
  uint8_t presses = 0;
  //Spin-loop for debounce
  for (uint8_t i = 0; i < 200; i++) {
    if (readButton()) {
      presses++;
    }
  }
  return presses >= 100? 1:0;
}
