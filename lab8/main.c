#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Timer0A.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "Switch.h"
#include "ST7735.h"
#include "SysTick.h"
#include "speaker.h"
#include "PWM.h"
#include "LCDscreen.h"

#define PF3             (*((volatile uint32_t *)0x40025020))
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

extern uint32_t time_update;

uint32_t digiOrAna = 0;
uint32_t alarm_On_Off = 0;

void PortFInit(void){
	SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
	while ((SYSCTL_PRGPIO_R&0x20) == 0) {};		// waiting for clock to be ready
	GPIO_PORTF_DIR_R |= 0x0E; //sets PF3-1 to output (builtin ledz)
	GPIO_PORTF_AFSEL_R &= ~0x0E; //disable alternate function F3-1
	GPIO_PORTF_DEN_R |= 0x0E; //enable I/O
	//GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
}


int main(void){

	DisableInterrupts();
	PLL_Init(Bus80MHz);
	Switch_Init();
	Board_Init();
	PortFInit();
	ST7735_InitR(INITR_REDTAB);
	Timer0A_Init(80000000);		// call handler every 1 second
	PWM0A_Init(65535, 30000); //initialize pwm0, 1000 Hz, 75% duty
	PWM0_ENABLE_R &= ~0x01;
	EnableInterrupts();
	
	while(1){
	PF1 ^= 0x02;		//heartbeating
	
		//dealing with the printing the right display
	if (Board_Input() == 0x00){		//if both PF0&PF4 are pressed change display
		while(Board_Input() == 0x00){} //wait till one is released
		if(digiOrAna==0)
			digiOrAna=1;
		else
			digiOrAna=0;
	}
	if((digiOrAna==0)&(time_update == 1)){	//call type of display based on request
		DisplayTime();
		time_update = 0;
	}
	else if(time_update==1){
		DisplayAnalogTime();
		time_update = 0;
	}

	
	//dealing with setting the alarm on or off
	if (Board_Input() == 0x10){	//check to see if PF4 is pressed
		while(Board_Input() == 0x10){}	//wait till this button is released
		if(alarm_On_Off == 0)
			alarm_On_Off = 1;
		else{
			alarm_On_Off = 0;
			SpeakerToggle(alarm_On_Off);
		}
	}
	
	//dealing with setting the actual clock time
	if((GPIO_PORTE_DATA_R&0x01)==0x01){
		while((GPIO_PORTE_DATA_R&0x01)==1){}
		setClock();
	}
	
	//dealing with setting the alarm time
	if(Board_Input()==0x01){
		while(Board_Input()==0x01){}
		setAlarmClock();
	}
	}
}
