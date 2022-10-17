#ifndef SYSH
#define SYSH
#include <stdint.h>
/* Init the system */
void InitSys(void);
/* Write a byte to the SPI. Non Blocking */
void SpiWrite(uint8_t);

#endif
