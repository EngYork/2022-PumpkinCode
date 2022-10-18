#ifndef NEOPIXELH
#define NEOPIXELH
#include <stdint.h>
#define PIXEL_NUMBER 10
/* SPI Bytes to generate Bit at 6MHz */
#define ZERO 0b11000000
#define ONE 0b11111000
extern uint8_t DATA_BUFFER[3*PIXEL_NUMBER]; //RGB,RGB,RGB,...

/* Write LED data */
void writeData(void);

void sendReset(void);

#endif
