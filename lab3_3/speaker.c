#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

void SpeakerToggle(uint32_t alarm_On_Off){

	if(alarm_On_Off == 1)
		PWM0_ENABLE_R |= 0x01;
	else
		PWM0_ENABLE_R &= ~0x01;
	
}
