#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "speaker.h"
#include "LCD.h"

int main (void){

	PLL_Init(Bus80MHz);
	portDInit();
	
	
}