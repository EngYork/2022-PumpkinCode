#include "Sys.h"
#include "neopixel.h"
#include "stm32l010x4.h"
#include "stm32l0xx_it.h"
#include "fx.h"
#include <stdint.h>

volatile uint8_t press = 0;

const uint8_t INIT_DATA_BUFFER[PIXEL_NUMBER][3] = {
  {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0},
  {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0}};
const uint8_t ALT_DATA_BUFFER[PIXEL_NUMBER][3] = {
  {15, 50, 100}, {50, 50, 0},  {100, 0, 10},  {20, 150, 75}, {200, 50, 10},
  {15, 125, 5},  {50, 0, 100}, {15, 125, 25}, {5, 25, 150},  {100, 10, 75}};
uint8_t FX_DATA_BUFFER[PIXEL_NUMBER][3] = {0};
const uint8_t (*ACTIVE_DATA_BUFFER)[][3] = &INIT_DATA_BUFFER;

#define FX_DELAY_mS 20
#define LED_MAX 200

int main(void)
{
  uint32_t lastFxRun = 0;
  int8_t effectDir = 1;
  InitSys();
  clearData(PIXEL_NUMBER);
  for (uint8_t i = 0; i <= PIXEL_NUMBER; i++) {
    uint32_t initTick = getTicks();
    while (getTicks() < initTick+300);
    writeData(*ACTIVE_DATA_BUFFER, i);
  }

  while(1){
    switch (press) {
    case 0:
      ACTIVE_DATA_BUFFER = &INIT_DATA_BUFFER;
      break;
    case 1:
      ACTIVE_DATA_BUFFER = &ALT_DATA_BUFFER;
      break;
    case 3: {
      ACTIVE_DATA_BUFFER = &FX_DATA_BUFFER;
      if (getTicks() > lastFxRun + FX_DELAY_mS){
	if (FX_DATA_BUFFER[0][0] >= LED_MAX - effectDir) {
	  effectDir = -effectDir; //Go Down
	} else if (FX_DATA_BUFFER[0][0] <=  -effectDir){
	  effectDir = -effectDir; //Go Up
	}
	for (uint8_t i = 0; i < PIXEL_NUMBER; i++){
	  FX_DATA_BUFFER[i][i%3] += effectDir;
	}
	lastFxRun = getTicks();
      }
    }
      break;
    }
    writeData(*ACTIVE_DATA_BUFFER, PIXEL_NUMBER);
    if(getButtonPressed()){
      press = (press+1)%4;
      clearButtonPress();
      clearData(PIXEL_NUMBER);
    }
  }
}
