#include <stdio.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "LCDscreen.h"

uint32_t hours = 5;
uint32_t minutes = 22;
uint32_t seconds = 43;

void PortF_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x14;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x14;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x14;         // 7) enable digital port
}

int main(void){
  PLL_Init(Bus80MHz);
  PortF_Init();
  ST7735_InitR(INITR_REDTAB);
	//DisplayTime();
	//printf("%d", hours);
	//DisplayAlarmTime();
	DisplayAnalogTime();
	
}