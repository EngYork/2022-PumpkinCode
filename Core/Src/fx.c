#include "fx.h"
#include "stm32l0xx_it.h"
#include <stdint.h>

uint8_t *pxData;
uint8_t usedPixels;
uint32_t lastTicks = 0;
uint8_t currentPixel = 0;

void setDataArray(uint8_t arr[], const uint8_t pixels) {
  usedPixels = pixels;
  pxData = (uint8_t*)arr;
}

void forwardFadeIn(uint8_t mSdelay) {
  
}
