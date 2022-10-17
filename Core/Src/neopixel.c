#include "neopixel.h"
#include <stdint.h>
#include "Sys.h"
#include "stm32l010x4.h"
uint8_t DATA_BUFFER[3 * PIXEL_NUMBER] = {128,128,128};
volatile uint32_t current_byte = 0;
volatile uint32_t current_bit = 0;
volatile uint8_t write_done = 0;

uint8_t getNextDat(void) {
  if (current_byte >= 3*PIXEL_NUMBER) return 0;
  uint8_t bit = (DATA_BUFFER[current_byte] & (1 << current_bit++)) != 0;
  if (current_bit == 8){
    current_bit = 0;
    current_byte ++;
  }
  return bit == 1? ONE:ZERO;
}

void writeData(void) {
  current_bit = current_byte = 0;
  SpiWrite(getNextDat());
  while (!write_done){
    if (current_byte >= 3*PIXEL_NUMBER) {
      write_done = 1;
    }
  }
}


/**
 * SPI IRQ
 */
void SPI1_IRQHandler(void) {
  __asm("bkpt");
  if (SPI1->SR & 1<<1) {
    SpiWrite(getNextDat());
  }
}
