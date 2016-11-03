// Timer0A.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.8

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 7.5, example 7.6

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
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "LCDscreen.h"
#include "speaker.h"

#define PF3             (*((volatile uint32_t *)0x40025020))
#define PF2             (*((volatile uint32_t *)0x40025010))

extern uint32_t alarm_seconds, alarm_minutes, alarm_hours, alarm_On_Off, digiOrAna;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


// Global Variables
uint32_t seconds=0;
uint32_t minutes=0;
uint32_t hours=0;
uint32_t time_update = 0;




void hoursCounter(void){

	if (hours >= 12)
		hours = 0;
	
	return;

}

void minutesCounter(void){

	if(minutes >= 60){
	
		minutes = 0;
		hours += 1;
		hoursCounter();
	
	}
	return;

}


void secondsCounter(void){

	if(seconds >= 60){
	
		seconds = 0;
		minutes += 1;
		minutesCounter();
		
	}
	return;

}

void setClock(void){ // manually setting the time

	DisableInterrupts();
	while((GPIO_PORTE_DATA_R&0x01)!=0x01){
		PF3 ^= 0x08;
		//switch for seconds
		if((GPIO_PORTE_DATA_R&0x02)==0x02){
			while((GPIO_PORTE_DATA_R&0x02)==0x02){}
			seconds += 1;
			secondsCounter();
			if(digiOrAna==0)	//call type of display based on request
				DisplayTime();
			else
				DisplayAnalogTime();
		}
		//switch for minutes
		if((GPIO_PORTE_DATA_R&0x04)==0x04){
			while((GPIO_PORTE_DATA_R&0x04)==0x04){}
			minutes += 1;
			minutesCounter();
			if(digiOrAna==0)	//call type of display based on request
				DisplayTime();
			else
				DisplayAnalogTime();
		}
		
	//switch for hours
		if((GPIO_PORTE_DATA_R&0x08)==0x08){
			while((GPIO_PORTE_DATA_R&0x08)==0x08){}
			hours += 1;
			hoursCounter();
			if(digiOrAna==0)	//call type of display based on request
				DisplayTime();
			else
				DisplayAnalogTime();
		}
	}
	EnableInterrupts();
	while((GPIO_PORTE_DATA_R&0x01)==0x01){} // wait till button released

}

// ***************** Timer0A_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0A_Init(uint32_t period){long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
  EndCritical(sr);
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	PF2 ^= 0x04;
	seconds += 1;
	secondsCounter();
	time_update = 1;
		// dealing with sounding the alarm or not
	if(alarm_seconds==seconds&alarm_minutes==minutes&alarm_hours==hours)
		SpeakerToggle(alarm_On_Off);
}

// ***************** TIMER1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
//(void(*task)(void), uint32_t period){
void Timer1_Init(){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 0xFFFFFFFE;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
}
