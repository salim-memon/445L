//lab2 prep
//salim memon [sam6345]
//priscilla chang [pc23384]


// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ADCSWTrigger.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "Switch.h"
#include "ST7735.h"
#include "SysTick.h"
#include "speaker.h"


#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define SWITCHES  (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board




#define ADC_MaxValue 4095
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

uint32_t ADCvalArray [1000];
uint32_t TvalArray [1000];
uint32_t counter = 0;

//void Timer1_Init(){
//	volatile uint32_t delay;
//	DisableInterrupts();
//  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
//	delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
//  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
//  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
//  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
//  TIMER1_TAILR_R = 0xFFFFFFFE;    // 4) reload value
//  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
//  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
//  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
//  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
//// interrupts enabled in the main program after all devices initialized
//// vector number 37, interrupt number 21
//  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
//  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
//}


volatile uint32_t ADCvalue;
// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
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
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
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
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
  PF2 ^= 0x04;         	// profile
	if(counter <1000){
		ADCvalArray[counter] = ADCvalue;
		TvalArray[counter] = TIMER1_TAR_R;
		counter++;
	}
	
}


int ADC_Val_Count_Array[ADC_MaxValue];//location in array is the adc value and the value that cell holds is the number of counts of each value
void ADCvalues(void){

	int maxAdcValue = 0;
	int minAdcValue = ADCvalArray[0];//making sure the min value is not 0 which would be the lowest possible
	int minValCount = 0; // updated later to be changed from 0 as that is the lowest possible
	int maxValCount = 0;
	
	for (int i=0; i < 1000; ++i){ // getting range of ADCValues
		
		if(maxAdcValue<ADCvalArray[i])
			maxAdcValue = ADCvalArray[i];
		if(minAdcValue>ADCvalArray[i])
			minAdcValue=ADCvalArray[i];
		
	}
	
	for(int i=0; i<1000; ++i){//count the number of time an ADCvalue repeats itself
		
		ADC_Val_Count_Array[ADCvalArray[i]] += 1;
		
	}
	
	//for(int i=0; i<sizeof(ADC_Val_Count_Array); ++i){
	minValCount = ADC_Val_Count_Array[minAdcValue];	//setting min to a possible value which is to be reset as a lower one is found
	for(int i=minAdcValue; i<(maxAdcValue+1); ++i){	//obtaining range for the count of ADC values
		if(maxValCount<ADC_Val_Count_Array[i])
			maxValCount=ADC_Val_Count_Array[i];
		
		if(minValCount>ADC_Val_Count_Array[i])
			minValCount=ADC_Val_Count_Array[i];	
		
	}
	
}

int main(void){
	uint32_t jitter = 0;
  PLL_Init(Bus80MHz);   	// 80 MHz
	portDInit();
//  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();								// set up Timer0A for 100 Hz interrupts
	Timer1_Init();
//  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
//  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
//  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
//                                        // configure PF2 as GPIO
//  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
//  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
//  PF2 = 0;                      // turn off LED
  EnableInterrupts();
	
	//from switch test main
	
	
	Switch_Init();           // PA5 is input
//  status = Switch_Input(); // 0x00 or 0x20
//  status = Switch_Input(); // 0x00 or 0x20
	Board_Init();             // initialize PF0 and PF4 and make them inputs
                            // make PF3-1 out (PF3-1 built-in LEDs)
  GPIO_PORTF_DIR_R |= (RED|BLUE|GREEN);
                              // disable alt funct on PF3-1
  GPIO_PORTF_AFSEL_R &= ~(RED|BLUE|GREEN);
                              // enable digital I/O on PF3-1
  GPIO_PORTF_DEN_R |= (RED|BLUE|GREEN);
                              // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF
	
//	while(1){
//    status = Board_Input();
//    switch(status){                    // switches are negative logic on PF0 and PF4
//      case 0x01: LEDS = BLUE; break;   // SW1 pressed
//      case 0x10: LEDS = RED; break;    // SW2 pressed
//      case 0x00: LEDS = GREEN; break;  // both switches pressed
//      case 0x11: LEDS = 0; break;      // neither switch pressed
//      default: LEDS = (RED|GREEN|BLUE);// unexpected return value
//    }
//  }
	
	// end of switch test main
  while(1){
    PF1 ^= 0x02;  // toggles when running in main
		//GPIO_PORTF_DATA_R ^= 0x02;  // toggles when running in main
		//PF1 = (PF1*12345678)/1234567+0x02;  // causing jitter
		if (counter >= 1000){	//if array is full
			//calculate jitter
			uint32_t small = TvalArray[1] - TvalArray[0];
			uint32_t large = TvalArray[1] - TvalArray[0];
			for(int i = 2; i < 1000; i++){
				uint32_t delta = TvalArray[i] - TvalArray[i-1];
				if ( delta < small)
					small = delta;
				if (delta > large)
					large = delta;
			}
			jitter = large-small;
			
			
			// this is where we process the data recordings
			
			ADCvalues();
		}
  }
}


