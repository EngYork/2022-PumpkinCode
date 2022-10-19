#ifndef SYSH
#define SYSH
#include <stdint.h>
/* Init the system */
void InitSys(void);
/* Disable the DMA for the SPI */
void DisableSpiDMA(void);
/* Enable the DMA for the SPI unit */
void EnableSpiDMA(uint8_t* source, uint16_t numBytes);
/* Read the button state. Return 1 if pressed, 0 if not */
uint8_t readButton(void);

#endif
