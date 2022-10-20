#include "neopixel.h"
#include <stdint.h>
#include "Sys.h"
#include "stm32l010x4.h"

uint8_t BIT_DATA_BUFFER[DATA_BYTE_NUMBER] = {0}; // Start empty byte, then data
const uint8_t RESET_BIT_BUFFER[RST_TX_CNT] = {0}; //Empty data buffer to reset bus.
/* Prepare Bit Data for transfer */
void prepBitData(const uint8_t DATA_IN[][3], uint8_t BIT_DATA_OUT[]);
void transmitBitBuffer(const uint8_t* buffer, const uint32_t pixels);

volatile uint8_t write_done = 1;
volatile uint8_t* spiDRpt = (uint8_t*)&(SPI1->DR);

uint8_t getBitByte(uint8_t bit) {
  return bit >= 1? ONE:ZERO;
}

void sendReset(void) {
  transmitBitBuffer(RESET_BIT_BUFFER, RST_TX_CNT);
}

void writeData(const uint8_t GRB_DAT[PIXEL_NUMBER][3], const uint8_t pixels) {
  prepBitData(GRB_DAT, BIT_DATA_BUFFER);
  sendReset();
  transmitBitBuffer(BIT_DATA_BUFFER, 1+(3*8*pixels));
}

void clearData(const uint8_t pixels) {
  for (uint32_t i = 1; i <= 3*8*pixels; i++) {
    BIT_DATA_BUFFER[i] = getBitByte(0);
  }
  sendReset();
  transmitBitBuffer(BIT_DATA_BUFFER, 3*8*pixels);
}

void prepBitData(const uint8_t DATA_IN[][3], uint8_t BIT_DATA_OUT[]){
  uint32_t bitind = 1;
  for (uint32_t i = 0; i < PIXEL_NUMBER; i++){
    for (uint8_t col = 0; col < 3; col++) {
      for (uint8_t bit = 0; bit < 8; bit++) {
	BIT_DATA_OUT[bitind++] = getBitByte(DATA_IN[i][col] & (uint8_t)0b10000000 >> bit); //Store bit-byte
      }
    }
  }
}

void transmitBitBuffer(const uint8_t* buffer, const uint32_t length) {
  while (!write_done) {
    //Wait for previous write to complete
  }
  write_done = 0;
  SPI1->CR1 |= (uint32_t)1 << 6; //Enable SPI
  EnableSpiDMA(buffer,length);
}

/**
 * DMA IRQ Handler for SPI1 DMA channel
 */
void DMA1_Channel2_3_IRQHandler(void) {
  if (DMA1->ISR & (uint32_t)1<<9) { //TX Complete
    DMA1->IFCR |= (uint32_t)1<<9; //Clear TX Complete flag
    DisableSpiDMA();
    SPI1->DR = 0; //Send a 0
    SPI1->CR2 &= ~((uint32_t)1 << 1); //Disable TXDMA
    write_done = 1;
  }
}

/**
 * SPI IRQ
 */
void SPI1_IRQHandler(void) {
}
