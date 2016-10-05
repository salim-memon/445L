// PeriodicTimer0AInts.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6
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

// oscilloscope or LED connected to PF3-1 for period measurement
// When using the color wheel, the blue LED on PF2 is on for four
// consecutive interrupts then off for four consecutive interrupts.
// Blue is off for: dark, red, yellow, green
// Blue is on for: light blue, blue, purple, white
// Therefore, the frequency of the pulse measured on PF2 is 1/8 of
// the frequency of the Timer0A interrupts.

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
#include "PLL.h"
#include "Timer0A.h"
#include "DAC.h"
//#include "SysTick.h"
#include "Switch.h"
#include "Music.h"
#include "Timer0A.h"

#define PF1             (*((volatile uint32_t *)0x40025008))



void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// if desired interrupt frequency is f, Timer0A_Init parameter is busfrequency/f
#define F16HZ (50000000/16)
#define F20KHZ (50000000/20000)
#define F64HZ	(50000000/64)

bool play_song = false;

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
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
	DAC_Init(0);
	PortFInit();
  //DAC_Init(0x1000);                  // initialize with command: Vout = Vref
	init_Song(); 
  Switch_Init();
	Board_Init();
	Timer0A_Init(F64HZ);  // initialize timer0A (64 Hz)
	Timer1_Init(F64HZ);
	//Timer2_Init();
	EnableInterrupts();


  while(1){
		PF1 ^= 0x02;		//heartbeating
		DAC_Output(0xFC0);
		for(int i =0; i < 100000; i++){
			Switch_Poll();
		}
  }
}

