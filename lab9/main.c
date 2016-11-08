
/*
Q) Three ways to initiate ADC conversion process
A) 1. based on the rising/falling edge triggers or timer values (successive approximation)
	 2. based on the time difference between two rising or two falling edges
	 3. based on the time difference between rising and falling edges

Q) What is the way to know when the conversion process has been completed?
A) We can check the INR3 flag(set) in the ADC0_RIS_R register to see if we have finished.

*/


#include <stdio.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ADCSWTrigger.h"
#include "ST7735.h"
#include "UART.h"
#include "PLL.h"
#include "calib.h"
#include <stdbool.h>

void DisableInterrupts(void); //Disable interrupts
void EnableInterrupts(void);	//Enable Interrupts
long StartCritical(void);			//previous I bit, disable interrupts
void EndCritical(long sr);		//restore I bit to previous value
void WaitForInterrupt(void);	//low power mode

//global variables
uint32_t Index = 0;
uint32_t data[100];
bool	updatedData = false;
volatile uint32_t ADCvalue;


// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init(uint32_t period){
  volatile uint32_t delay;
  DisableInterrupts();
	//index init
	Index = 0;
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = period-1;         // start value for 1000 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  //PF2 ^= 0x04;                   // profile
  //PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
  //PF2 ^= 0x04;         	// profile
	if(Index <100){
		data[Index] = ADCvalue;
		Index++;
		updatedData = true;
	}
	else if(Index >= 100)	//added this part for interpolation
		Index = 0;
	
}


void NyquistTheorem(void){
	Timer0A_Init(160000);	//500HzInt
	UART_OutString("dataPoints: \r\n");
	while(Index < 100){};
	for(int i =0; i<100; i++){
		UART_OutUDec(data[i]);
		UART_OutString("\r\n");
	}
}
void ValvanoPostulate(void){
	Timer0A_Init(800000);	//100HzInt
	UART_OutString("dataPoints: \r\n");
	while(Index < 100){};
	for(int i =0; i<100; i++){
		UART_OutUDec(data[i]);
		UART_OutString("\r\n");
	}
}
void Aliasing(void){
	Timer0A_Init(40000);	//2000HzInt
	UART_OutString("dataPoints: \r\n");
	while(Index < 100){};
	for(int i =0; i<100; i++){
		UART_OutUDec(data[i]);
		UART_OutString("\r\n");
	}
}


int main(void){
	DisableInterrupts();
	PLL_Init(Bus80MHz);		//bus clock at 80MHz
	UART_Init();
	ST7735_InitR(INITR_REDTAB);
	ADC0_InitSWTriggerSeq3_Ch9();
	Timer0A_Init(80000);	//1000HzInt
	EnableInterrupts();
	
// use to directly just print out the the ADC values at 1000Hz	
	
//	UART_OutString("dataPoints: \r\n");
//	while(Index < 100){};
//	for(int i =0; i<100; i++){
//		UART_OutUDec(data[i]);
//		UART_OutString("\r\n");
//	}

//use to directly just print ouot the ADC values at nyquist theorem ()
	//NyquistTheorem();
//use to directly just print ouot the ADC values at Valvano Postulate ()
	//ValvanoPostulate();
//use to directly just print ouot the ADC values at Aliasing ()
	//Aliasing();

//used to constantly measure temperature based on calibration and print on screen
	while(1){
		if(updatedData){	//prevent it from repeating same value
			updatedData = false;
			uint16_t temperature_Value = Linear_Interpolation(data[Index]);
			//print values to screen
		}
	}
	
	
	return 0;
}

