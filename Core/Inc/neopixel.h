#ifndef NEOPIXELH
#define NEOPIXELH
#include <stdint.h>
#define PIXEL_NUMBER 10
#define DATA_BYTE_NUMBER 1+( 3 * 8 * PIXEL_NUMBER)
#define RST_TX_CNT 300
/* SPI Bytes to generate Bit at 6MHz */
#define ZERO 0b11000000
#define ONE 0b11111000

/* Write LED data */
void writeData(const uint8_t GRB_DAT[PIXEL_NUMBER][3], const uint8_t pixels);
/* Clear pixels pixels */
void clearData(const uint8_t pixels);
//void writeData(void);
void sendReset(void);

#endif
