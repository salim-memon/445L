#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Timer0A.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "Switch.h"
#include "SysTick.h"
#include "speaker.h"
#include "ST7735.h"
#include "Switch.h"

extern uint32_t hours, minutes, seconds, alarm_On_Off;

uint32_t alarm_seconds = 0;
uint32_t alarm_minutes = 0;
uint32_t alarm_hours = 0;

void alarm_hoursCounter(void){

	if (alarm_hours >= 12)
		alarm_hours = 0;
	
	return;

}

void alarm_minutesCounter(void){

	if(alarm_minutes >= 60){
	
		alarm_minutes = 0;
		alarm_hours += 1;
		alarm_hoursCounter();
	
	}
	return;

}


void alarm_secondsCounter(void){

	if(alarm_seconds >= 60){
	
		alarm_seconds = 0;
		alarm_minutes += 1;
		alarm_minutesCounter();
		
	}
	return;

}


void DisplayTime(void){
	ST7735_FillScreen(0);
	ST7735_SetCursor(7,15);
	
	if (hours < 10) {printf(" ");}
	if(minutes<=9){
		if(seconds<=9){
			printf("%d:0%d:0%d",hours,minutes,seconds);
		}
		else{
			printf("%d:0%d:%d",hours,minutes,seconds);
		}
	}
	else{
		if(seconds<=9){
			printf("%d:%d:0%d",hours,minutes,seconds);
		}
		else{
			printf("%d:%d:%d",hours,minutes,seconds);
		}
	}
}

void DisplayAlarmTime(void){
	ST7735_FillScreen(0);
	ST7735_SetCursor(7,15);
	
	if (alarm_hours < 10) {printf(" ");}
	if(alarm_minutes<=9){
		if(alarm_seconds<=9){
			printf("%d:0%d:0%d",alarm_hours,alarm_minutes,alarm_seconds);
		}
		else{
			printf("%d:0%d:%d",alarm_hours,alarm_minutes,seconds);
		}
	}
	else{
		if(seconds<=9){
			printf("%d:%d:0%d",hours,minutes,seconds);
		}
		else{
			printf("%d:%d:%d",alarm_hours,alarm_minutes,alarm_seconds);
		}
	}
}

void DisplayAnalogTime(void){
	ST7735_FillScreen(ST7735_BLACK);
//	ST7735_Circle(64,64,60,ST7735_WHITE);
//	ST7735_DrawClock();		
//  ST7735_Line(64,64,hours,hours,ST7735_BLUE);			
//	ST7735_Line(64,64,minutes,minutes,ST7735_BLUE);					
//  ST7735_Line(64,64,seconds,seconds,ST7735_RED); 	
}

void setAlarmClock(void){

	while(Board_Input()!=0x01){
		alarm_On_Off = 0;			//setting the alarm off
		//switch for seconds
		if((GPIO_PORTE_DATA_R&0x02)==0x02){
			while((GPIO_PORTE_DATA_R&0x02)==0x02){}
			alarm_seconds += 1;
			alarm_secondsCounter();
			DisplayAlarmTime();
		}
		//switch for minutes
		if((GPIO_PORTE_DATA_R&0x04)==0x04){
			while((GPIO_PORTE_DATA_R&0x04)==0x04){}
			alarm_minutes += 1;
			alarm_minutesCounter();
			DisplayAlarmTime();
		}
		
	//switch for hours
		if((GPIO_PORTE_DATA_R&0x08)==0x08){
			while((GPIO_PORTE_DATA_R&0x08)==0x08){}
			alarm_hours += 1;
			alarm_hoursCounter();
			DisplayAlarmTime();
		}
	}
	while(Board_Input()==0x01){} // wait till button released

}
