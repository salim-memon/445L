// Switch.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize a GPIO as an input pin and
// allow reading of two negative logic switches on PF0 and PF4
// and an external switch on PA5.
// Use bit-banded I/O.
// Daniel and Jonathan Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Section 4.2    Program 4.2

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Example 2.3, Program 2.9, Figure 2.36

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

// negative logic switches connected to PF0 and PF4 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad
// NOTE: The NMI (non-maskable interrupt) is on PF0.  That means that
// the Alternate Function Select, Pull-Up Resistor, Pull-Down Resistor,
// and Digital Enable are all locked for PF0 until a value of 0x4C4F434B
// is written to the Port F GPIO Lock Register.  After Port F is
// unlocked, bit 0 of the Port F GPIO Commit Register must be set to
// allow access to PF0's control registers.  On the LM4F120, the other
// bits of the Port F GPIO Commit Register are hard-wired to 1, meaning
// that the rest of Port F can always be freely re-configured at any
// time.  Requiring this procedure makes it unlikely to accidentally
// re-configure the JTAG and NMI pins as GPIO, which can lock the
// debugger out of the processor and make it permanently unable to be
// debugged or re-programmed.
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "Timer1.h"
#include "Music.h"
#include "DAC.h"



extern bool play_song;
//#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
//#define PF0                     (*((volatile uint32_t *)0x40025004))
//#define PF4                     (*((volatile uint32_t *)0x40025040))
//#define PA5                     (*((volatile uint32_t *)0x40004080))
//#define SWITCHES                (*((volatile uint32_t *)0x40025044))
//#define SW1       0x10                      // on the left side of the Launchpad board
//#define SW2       0x01                      // on the right side of the Launchpad board
//#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

//------------Switch_Init------------
// Initialize GPIO Port E bit 5 for input
// Input: none
// Output: none
void Switch_Init(void){ 
	//activating for port E pins 0-3
	
	SYSCTL_RCGCGPIO_R |= 0x10;     // 1) activate clock for Port E
  while ((SYSCTL_PRGPIO_R&0x10) == 0){};// ready?
	GPIO_PORTE_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // 3) disable analog on PE?
  //GPIO_PORTE_PCTL_R &= ~0x000F0F00; // 4) PCTL GPIO on PA5
  //GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFF000F)+0x00000000;
	GPIO_PORTE_DIR_R &= ~0x0F;        // 5) direction PA5 input
  GPIO_PORTE_AFSEL_R &= ~0x0F;      // 6) PA5 regular port function
  GPIO_PORTE_DEN_R |= 0x0F;         // 7) enable PA5 digital port
	Timer1_Init();
}

	
void Switch_Poll(void){
	if(((GPIO_PORTE_DATA_R&0x01)==0x01) && (play_song == false)){
		while((GPIO_PORTE_DATA_R&0x01)==0x01){}
		Play();
	}
	if(((GPIO_PORTE_DATA_R&0x01)==0x01) && (play_song == true)){
		while((GPIO_PORTE_DATA_R&0x01)==0x01){}
		Pause();
	}
			else if((GPIO_PORTE_DATA_R&0x02)==0x02){	 // see if switch2 pressed
				Pause();
				while((GPIO_PORTE_DATA_R&0x02)==0x02){} //waiting for switch to be released
					Reset(); //reset song pointer
						}
			
						else if((GPIO_PORTE_DATA_R&0x04)==0x04){	 //see if  switch3 pressed
							  while((GPIO_PORTE_DATA_R&0x04)==0x04){} //waiting for switch to be released
								increasetempo();

						}
					}

