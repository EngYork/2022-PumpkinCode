#include "neopixel.h"
#include <stdint.h>
#include "Sys.h"
#include "stm32l010x4.h"
uint8_t DATA_BUFFER[3 * PIXEL_NUMBER] = {10,10,0,10,10,0,10,200,0,10,10,0,10,10,0,10,200,0,10,10,0,10,10,0,10,10,0,10,10,0};
uint8_t BIT_DATA_BUFFER[1+(3 * 8 * PIXEL_NUMBER)] = {0}; //Start empty byte, then data
/* Prepare Bit Data for transfer */
void prepBitData(uint8_t DATA_IN[3 * PIXEL_NUMBER], uint8_t BIT_DATA_OUT[3 * 8 * PIXEL_NUMBER]);

volatile uint8_t write_done = 0;
volatile uint8_t* spiDRpt = (uint8_t*)&(SPI1->DR);

uint8_t getBitByte(uint8_t bit) {
  return bit >= 1? ONE:ZERO;
}

void sendReset(void) {
  SPI1->CR2 &= ~((uint32_t)1 << 1); //Disable TXDMA
  for (uint16_t i = 0; i < 300; i++) {
    while(SPI1->SR & (uint32_t)1<<7); //Wait till not busy
    SPI1->DR = 0; //Send a 0
  }
}

void writeData(void) {
  write_done = 0;
  volatile SPI_TypeDef* spi = SPI1;
  prepBitData(DATA_BUFFER, BIT_DATA_BUFFER);
  SPI1->CR1 |= (uint32_t)1 << 6; //Enable SPI
  sendReset();
  EnableSpiDMA(BIT_DATA_BUFFER,1+( 3 * 8 * PIXEL_NUMBER));
  while (!write_done) {
     
  }
  SPI1->DR = 0; //Send a 0
  SPI1->CR2 &= ~((uint32_t)1 << 1); //Disable TXDMA
}

void prepBitData(uint8_t DATA_IN[3 * PIXEL_NUMBER], uint8_t BIT_DATA_OUT[3 * 8 * PIXEL_NUMBER]){
  uint32_t bitind = 1;
  for (uint32_t i = 0; i < 3 * PIXEL_NUMBER; i++){
    for (uint8_t bit = 0; bit < 8; bit++) {
      BIT_DATA_OUT[bitind++] = getBitByte(DATA_IN[i] & (uint8_t)0b10000000 >> bit); //Store bit-byte
    }
  }
}


/**
 * DMA IRQ Handler for SPI1 DMA channel
 */
void DMA1_Channel2_3_IRQHandler(void) {
  if (DMA1->ISR & (uint32_t)1<<9) { //TX Complete
    DMA1->IFCR |= (uint32_t)1<<9; //Clear TX Complete flag
    DisableSpiDMA();
    write_done = 1;
  }
}

/**
 * SPI IRQ
 */
void SPI1_IRQHandler(void) {
  if (SPI1->SR & 1<<1) {
  }
}
