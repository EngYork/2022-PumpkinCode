#ifndef NEOPIXELH
#define NEOPIXELH
#include <stdint.h>
#define PIXEL_NUMBER 10
/* SPI Bytes to generate Bit at 6MHz */
#define ZERO 0xC0
#define ONE 0xF0
extern uint8_t DATA_BUFFER[3*PIXEL_NUMBER]; //RGB,RGB,RGB,...

/* Write LED data */
void writeData(void);

#endif
