#ifndef SYSH
#define SYSH
#include <stdint.h>
/* Init the system */
void InitSys(void);
void DisableSpiDMA(void);
void EnableSpiDMA(uint8_t* source, uint16_t numBytes);

#endif
