#include "Sys.h"
#include "neopixel.h"
#include "stm32l0xx_it.h"
#include <stdint.h>

int main(void)
{
  InitSys();
  while(1){
    writeData();
  }
}
