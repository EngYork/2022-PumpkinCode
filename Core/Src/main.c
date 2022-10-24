#include "Sys.h"
#include "neopixel.h"
#include "stm32l010x4.h"
#include "stm32l0xx_it.h"
#include "fx.h"
#include <stdint.h>
#include <string.h>

volatile uint8_t press = 0;

//Initial static data buffer
const uint8_t INIT_DATA_BUFFER[PIXEL_NUMBER][3] = {
  {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0},
  {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0}, {15, 50, 0}};
//Secondary static data buffer
const uint8_t ALT_DATA_BUFFER[PIXEL_NUMBER][3] = {
  {15, 50, 100}, {50, 50, 0},  {100, 0, 10},  {20, 150, 75}, {200, 50, 10},
  {15, 125, 5},  {50, 0, 100}, {15, 125, 25}, {5, 25, 150},  {100, 10, 75}};
//Modifyable data buffer for effects.
uint8_t FX_DATA_BUFFER[PIXEL_NUMBER][3] = {0};
const uint8_t (*ACTIVE_DATA_BUFFER)[][3] = &INIT_DATA_BUFFER;

#define FX_DELAY_mS 20
#define LED_MAX 200
#define EDGE_LED_CNT 8

//Base indexes for each "Column" of LEDs.
const uint8_t COL_BASES[4] = {2, 5, 8, 9};
//Size of each LED column. Reverse indexed from the "base".
const uint8_t COL_SIZES[4] = {3, 3, 3, 1};
//The leds around the edge
const uint8_t EDGE_LEDS[EDGE_LED_CNT] = {3,6,9,8,5,2,1,0};

//Clear a data buffer
void resetBuffer(uint8_t buffer[PIXEL_NUMBER][3]);
// Generate a (not very) random number with an LFSR
uint32_t l_rand(void);
//Init LFSR Seed
uint32_t LFSR = 0xDEADBEEF;
uint32_t LFSR_BIT = 0;
//End of effect
uint8_t effect_done;
//Current position in effect sequence (if applicable)
uint32_t current_fx_index;

int main(void)
{
  uint32_t lastFxRun = 0;
  int8_t effectDir = 1;
  InitSys();
  clearData(PIXEL_NUMBER);
  for (uint8_t i = 0; i <= PIXEL_NUMBER; i++) {
    uint32_t initTick = getTicks();
    while (getTicks() < initTick+200);
    writeData(*ACTIVE_DATA_BUFFER, i);
  }

  while(1){
    switch (press) {
    case 0:
      ACTIVE_DATA_BUFFER = &INIT_DATA_BUFFER;
      break;
    case 1:
      ACTIVE_DATA_BUFFER = &ALT_DATA_BUFFER;
      break;
    case 2: { //Fade in fade out
      if (getTicks() > lastFxRun + 20){
	if (FX_DATA_BUFFER[0][0] >= LED_MAX - effectDir) {
	  effectDir = -effectDir; //Go Down
	} else if (FX_DATA_BUFFER[0][0] <=  -effectDir){
	  effectDir = -effectDir; //Go Up
	}
	for (uint8_t i = 0; i < PIXEL_NUMBER; i++){
	  FX_DATA_BUFFER[i][i%3] += effectDir;
	}
	lastFxRun = getTicks();
      }
    }
      break;
    case 3: { //Cheapo flame effect
      if (getTicks() > lastFxRun + 250){
	resetBuffer(FX_DATA_BUFFER);
	for (uint8_t i = 0; i < 4; i++) {
	  uint8_t RAN = (uint8_t)l_rand();
	  uint8_t leds_on; //Number of LEDS on
	  if (COL_SIZES[i] == 1){
	    leds_on = RAN % 2;
	  } else {
	    leds_on = (RAN % COL_SIZES[i]) + 1;
	  }
	  uint8_t j = 0;
	  for (int8_t led = COL_BASES[i]; led >= COL_BASES[i] - (leds_on-1); led--){
	    FX_DATA_BUFFER[led][0] = (10 + (20*j)) / (j+1);
	    FX_DATA_BUFFER[led][1] = (100 - (10*j))/(j+1);
	    FX_DATA_BUFFER[led][2] = 0;
	    j++;
	  }
	}
	lastFxRun = getTicks();
      }
    }
      break;
    case 4: { //Slightly rubbish chase effect
      if (getTicks() > lastFxRun + 125){
        resetBuffer(FX_DATA_BUFFER);
	if (current_fx_index < EDGE_LED_CNT) {
	  FX_DATA_BUFFER[EDGE_LEDS[current_fx_index]][0] = 100; //Led n
	  FX_DATA_BUFFER[EDGE_LEDS[current_fx_index]][1] = 50;
	  FX_DATA_BUFFER[EDGE_LEDS[current_fx_index]][2] = 10;
	  
	  FX_DATA_BUFFER[EDGE_LEDS[(current_fx_index-1) % EDGE_LED_CNT]][0] = 10; //n-1
	  FX_DATA_BUFFER[EDGE_LEDS[(current_fx_index-1) % EDGE_LED_CNT]][1] = 5;
	  FX_DATA_BUFFER[EDGE_LEDS[(current_fx_index-1) % EDGE_LED_CNT]][2] = 1;
	  
	  FX_DATA_BUFFER[EDGE_LEDS[(current_fx_index+1) % EDGE_LED_CNT]][0] = 10; //n+1
	  FX_DATA_BUFFER[EDGE_LEDS[(current_fx_index+1) % EDGE_LED_CNT]][1] = 5;
	  FX_DATA_BUFFER[EDGE_LEDS[(current_fx_index+1) % EDGE_LED_CNT]][2] = 1;
	} else {
	  current_fx_index = 0;
	}
	current_fx_index = (current_fx_index+1) % EDGE_LED_CNT;
	lastFxRun = getTicks();
      }
    }
      break;
    }

    if(getButtonPressed()){
      press = (press+1)%5;
      clearButtonPress(); //I've consumed the press now
      clearData(PIXEL_NUMBER); //Clear the display
      if (press > 1) { //All the active stuff goes after press 1.
	resetBuffer(FX_DATA_BUFFER);
	lastFxRun = 0; //Reset FX delay count
	effect_done = 0;
	current_fx_index = 0;
	ACTIVE_DATA_BUFFER = &FX_DATA_BUFFER;
      }
    }
    
    writeData(*ACTIVE_DATA_BUFFER, PIXEL_NUMBER);
  }
}

uint32_t l_rand() {
  /* feedback polynomial: x^32 + x^30 + x^29 + x^27 + x^20 + x^15 + x^10 + x^5 + 1 */
  LFSR_BIT = ((LFSR >> 0) ^ (LFSR >> 2) ^ (LFSR >> 3) ^ (LFSR >> 5) ^ (LFSR >> 12) ^ (LFSR >> 17) ^ (LFSR >> 22) ^ (LFSR >> 27)) & 1u;
  return LFSR = (LFSR >> 1) | (((uint32_t)LFSR_BIT) << 31);
}

void resetBuffer(uint8_t buffer[PIXEL_NUMBER][3]){
  memset(buffer, 0, 3*PIXEL_NUMBER); //Clear data buffer
}
