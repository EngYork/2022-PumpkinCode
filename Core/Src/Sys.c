#include "Sys.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"
#include <stdint.h>

void InitClocks(void);
void InitSPI(void);
void InitGPIO(void);

void InitSys(void) {
  SystemInit();
  InitClocks();
  InitGPIO();
  InitSPI();
}

void InitClocks(void) {
  RCC->APB2ENR |= (uint32_t)1 << 22; //Enable DBG clock
  FLASH->ACR |= (uint32_t)1; //Set 1 wait state on NVM
  RCC->CFGR |= (uint32_t)1 << 15; //Wakeup to HSI16
  RCC->CR &= !((uint32_t)1 << 24); //Disable PLL
  while (RCC->CR & ((uint32_t)1 << 25)); //Wait till PLL is off
  RCC->CR |= (uint32_t)1; //Enable HSI16
  while (!(RCC->CR & (uint32_t)1 << 2)); //Wait till HSI16 is ready
  //Set PLL for 24MHz output ((16/4)*6 = 24)
  RCC->CFGR |= (uint32_t)0b11 << 22; //Divide HSI16 by 4
  RCC->CFGR |= (uint32_t)0b0010 << 18; //Multiply by 6
  RCC->CR |= (uint32_t)1 << 24; //Enable PLL
  while (!(RCC->CR & ((uint32_t)1 << 25))); //Wait till PLL is stable
  RCC->CFGR |= (uint32_t)0b11; //Set PLL as system clock
  while (!(RCC->CFGR & (uint32_t)0b11 << 2)); //Wait till PLL is set as system clock
  SystemCoreClockUpdate();
  RCC->APB1ENR |= (uint32_t)1 << 28; //Enable PWR config clock
}

void InitSPI(void) {
  RCC->APB2RSTR ^= (uint32_t)1 << 12; //Reset SPI1
  RCC->APB2RSTR ^= (uint32_t)1 << 12;
  RCC->APB2ENR |= (uint32_t)1 << 12; //Enable SPI Clock
  SPI1->CR2 |= (uint32_t)1 << 7;
  SPI1->CR1 |= (uint32_t)0b001 << 3; //Set Baud to 6MHz (Bus 24MHz/4)
  SPI1->CR1 |= (uint32_t)1 << 2; //Master mode
  SPI1->CR1 |= (uint32_t)1 << 15; //Single Line
  SPI1->CR1 |= (uint32_t)1 << 14; //TX Only
}

void InitGPIO(void) {
  RCC->IOPENR |= (uint32_t)0b11; //Enable Port A and B
  GPIOB->MODER &= !((uint32_t)0b11 << 2);
  GPIOB->MODER |= (uint32_t)0b10 << 2; //Set AF on PB1
  GPIOB->OSPEEDR |= ((uint32_t)0b10 << 2); //Set PB1 to High Speed
  GPIOA->MODER &= !(((uint32_t)0xf)<<6); //Set A3,4,5,6 to Input
  GPIOA->PUPDR |= (uint32_t) 0b01010101 << 6; //Set Pull-up on A3,4,5,6
  EXTI->IMR |= (uint32_t)0b1111 << 3; //Enable EXTI lines 3,4,5,6
  EXTI->FTSR |= (uint32_t)0b1111 << 3; //Enable falling trigger on EXTI lines 3,4,5,6
}
