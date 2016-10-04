#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "Switch.h"
#include "Systick.h"
#include "Music.h"
#include "Timer0A.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


void Play(void){
	EnableInterrupts();
}
	
void Pause(void){	
	DisableInterrupts();
}
	
void Reset(void){
	DisableInterrupts();

}

void Mode_Intrument(void){

	long sr = StartCritical();
	if (Instrument == flute){
		Instrument = trumpet;
	}
	EndCritical(sr);

}
	
