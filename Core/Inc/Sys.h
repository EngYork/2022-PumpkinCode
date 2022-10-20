#ifndef SYSH
#define SYSH
#include <stdint.h>
#define DEB_mS 400

/* Init the system */
void InitSys(void);
/* Disable the DMA for the SPI */
void DisableSpiDMA(void);
/* Enable the DMA for the SPI unit */
void EnableSpiDMA(const uint8_t* source, const uint16_t numBytes);
/* Check if button pressed.*/
uint8_t getButtonPressed(void);
/* Clears button state */
void clearButtonPress(void);

void SysTick_Handler(void);
/* Get current Ticks */
uint32_t getTicks(void);


#endif
