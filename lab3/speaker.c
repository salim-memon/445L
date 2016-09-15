#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

void portDInit(void){

	
	SYSCTL_RCGCGPIO_R |= 0x08; //init port D
	while ((SYSCTL_PRGPIO_R&0x08) == 0) {};		// waiting for clock to be ready
	GPIO_PORTD_DIR_R |= 0x01; //sets PB0 to output
	GPIO_PORTD_AFSEL_R &= ~0x01; //disable alternate function
	GPIO_PORTD_DEN_R |= 0x01; //enable I/O
	
}