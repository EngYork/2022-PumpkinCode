#include "Sys.h"
#include "stm32l010x4.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"
#include <stdint.h>

void InitClocks(void);
void InitSPI(void);
void InitGPIO(void);

volatile uint8_t buttonPressed = 0;
volatile uint32_t msTick = 0;
volatile uint32_t btnDdebCnt = 0;

void InitSys(void) {
  SystemInit();
  __disable_irq();
  InitClocks();
  InitGPIO();
  InitSPI();
  __enable_irq();
}

#ifdef STM32L010x4
void InitClocks(void) {
  RCC->APB2ENR |= (uint32_t)1 << 22; //Enable DBG clock
  RCC->APB2ENR |= (uint32_t)1; //Enable SYSCFG clock
  RCC->AHBENR |= (uint32_t)1; //Enable DMA clock
  
  FLASH->ACR |= (uint32_t)1; //Set 1 wait state on NVM
  RCC->CFGR |= (uint32_t)1 << 15; //Wakeup to HSI16
  RCC->CR &= ~((uint32_t)1 << 24); //Disable PLL
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
  SysTick_Config(SystemCoreClock / 1000);
  NVIC_EnableIRQ(SysTick_IRQn);
  NVIC_SetPriority(SysTick_IRQn, 0);
}

void InitSPI(void) {
  RCC->APB2RSTR ^= (uint32_t)1 << 12; //Reset SPI1
  RCC->APB2RSTR ^= (uint32_t)1 << 12;
  RCC->APB2ENR |= (uint32_t)1 << 12; //Enable SPI Clock
  SPI1->CR1 |= (uint32_t)0b001 << 3; //Set Baud to 6MHz (Bus 24MHz/4)
  SPI1->CR1 |= (uint32_t)1 << 2; //Master mode
  SPI1->CR1 |= (uint32_t)1 << 9; //SSM
  SPI1->CR1 |= (uint32_t)1 << 8; //SSI
  SPI1->CR1 &= ~((uint32_t)1<<10);
  SPI1->CR1 |= (uint32_t)0b11 << 14; //Single Line, TX only
  DMA1_Channel3->CCR |= (uint32_t)0b11 << 12; //Very high priority
  DMA1_Channel3->CCR |= (uint32_t)0b1 << 7; //Increment Source
  DMA1_Channel3->CCR |= (uint32_t)0b1 << 4; //Memory to Peripheral
  DMA1_Channel3->CCR |= (uint32_t)0b1 << 1; //Transfer Complete IRQ
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);
}

void InitGPIO(void) {
  RCC->IOPENR |= (uint32_t)0b11; //Enable Port A and B
  GPIOB->MODER &= ~((uint32_t)0b11 << 2);
  GPIOB->MODER |= (uint32_t)0b10 << 2; //Set AF mode on PB1
  GPIOB->AFR[0] |= (uint32_t)1 << 4; //Set SPI1 MOSI AF on PB1
  GPIOB->OSPEEDR |= ((uint32_t)0b10 << 2); //Set PB1 to High Speed
  GPIOA->MODER &= ~(((uint32_t)0b11111111)<<6); //Set A3,4,5,6 to Input
  GPIOA->PUPDR |= (uint32_t) 0b01010101 << 6; //Set Pull-up on A3,4,5,6
  EXTI->IMR |= (uint32_t)0b1111 << 3; //Enable EXTI lines 3,4,5,6
  EXTI->FTSR |= (uint32_t)0b1111 << 3; //Enable falling trigger on EXTI lines 3,4,5,6
  NVIC_EnableIRQ(EXTI4_15_IRQn);
  NVIC_SetPriority(EXTI4_15_IRQn, 0);
  EXTI->IMR |= (uint32_t)1<<6; //Enable EXTI Interrupt for PA6
  EXTI->RTSR |= (uint32_t)1<<6; //Rising Trigger
}

uint8_t getButtonPressed(void) {
  return buttonPressed;
}

void clearButtonPress(void) {
  buttonPressed = 0;
}

/* Enable DMA Channel 3 On/Off */
void EnableSpiDMA(const uint8_t* source, const uint16_t numBytes) {
  DMA1_Channel3->CNDTR = numBytes;
  DMA1_Channel3->CPAR = (uint32_t)(&(SPI1->DR)); //SPI1 DR address
  DMA1_Channel3->CMAR = (uint32_t)(source); //Source Array
  DMA1_CSELR->CSELR |= (uint32_t)0b001 << 8; //SPI1 TX DMA enable
  DMA1_Channel3->CCR |= (uint32_t)0b1; //Enable DMA channel
  SPI1->CR2 |= (uint32_t)1 << 1; //TXDMAEN
}
/* Disable DMA Channel 3 On/Off */
void DisableSpiDMA(void) {
  DMA1_Channel3->CCR &= ~((uint32_t)0b1);
  while (SPI1->SR & (uint32_t)1<<7); //Wait till not busy;
}


void EXTI4_15_IRQHandler(void){
  __disable_irq();
  if (EXTI->PR & (uint32_t)1<<6) { //Interrupt on correct line.
    EXTI->PR |= (uint32_t)1<<6; //Clear flag
    //Buttons are on pull-ups, so high when open low when short
    if (!(GPIOA->IDR & (uint32_t)1 << 6)) { 
      buttonPressed = 1; //Button pressed
      EXTI->IMR &= ~((uint32_t)1<<6); //Mask EXTI Interrupt for PA6
      btnDdebCnt = getTicks();
    }
  }
  __enable_irq();
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  msTick++;
  //If Button IRQ masked and debounce count expired, unmask
  if ((!(EXTI->IMR & (uint32_t)1<<6))&&(msTick > btnDdebCnt + DEB_mS)) {
    EXTI->IMR |= (uint32_t)1<<6; //Unmask EXTI Interrupt for PA6
  }
}

uint32_t getTicks(void) {
  return msTick;
}


#endif
