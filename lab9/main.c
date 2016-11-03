
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

void DisableInterrupts(void); //Disable interrupts
void EnableInterrupts(void);	//Enable Interrupts
long StartCritical(void);			//previous I bit, disable interrupts
void EndCritical(long sr);		//restore I bit to previous value
void WaitForInterrupt(void);	//low power mode

//global variables
uint32_t Index = 0;
uint32_t data[100];
volatile uint32_t ADCvalue;



uint16_t const ADCdata[53]={0,9,44,79,116,153,190,229,268,308,348,							
     390,432,475,519,564,610,657,704,752,802,							
     852,903,956,1009,1063,1119,1175,1233,1291,1351,							
     1412,1474,1537,1601,1666,1733,1801,1870,1941,2012,							
     2085,2160,2235,2312,2391,2470,2552,2634,2718,2803,2890,4096};							

		 
uint16_t const Tdata[53]={4000,4000,3940,3880,3820,3760,3700,3640,3580,3520,3460,		
     3400,3340,3280,3220,3160,3100,3040,2980,2920,2860,		
     2800,2740,2680,2620,2560,2500,2440,2380,2320,2260,		
     2200,2140,2080,2020,1960,1900,1840,1780,1720,1660,		
     1600,1540,1480,1420,1360,1300,1240,1180,1120,1060,1000,1000};		

		 
uint16_t const Rdata[53]={5027,5027,5125,5225,5328,5434,5542,5652,5765,5881,5999,									
     6121,6245,6373,6503,6637,6774,6914,7058,7206,7357,									
     7512,7670,7833,8000,8172,8347,8527,8712,8902,9096,									
     9296,9500,9711,9926,10148,10375,10608,10848,11094,11346,									
     11606,11872,12146,12427,12716,13013,13318,13632,13954,14286,14626,14626};									








// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init1000HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 79999;         // start value for 1000 Hz interrupts
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
	}
	
}

int main(void){
	DisableInterrupts();
	PLL_Init(Bus80MHz);		//bus clock at 80MHz
	UART_Init();
	ST7735_InitR(INITR_REDTAB);
	ADC0_InitSWTriggerSeq3_Ch9();
	Timer0A_Init1000HzInt();
	EnableInterrupts();
	
	UART_OutString("dataPoints: \r\n");
	while(Index < 100){};
	for(int i =0; i<100; i++){
		UART_OutUDec(data[i]);
		UART_OutString("\r\n");
	}
	
	return 0;
}

