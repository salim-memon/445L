
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

const uint32_t hex[13] = { 0, 74, 81, 84, 81, 74, 64, 54, 47, 44, 47, 54, 64 };
const uint32_t hey[13] = { 0, 47, 54, 64, 74, 81, 84, 81, 74, 64, 54, 47, 44 };
const uint32_t mex[60] = { 64, 68, 72, 76, 80, 84, 88, 91, 94, 96, 99, 101, 102, 103, 104, 104, 104, 103, 102, 101, 99, 96, 94, 91, 88, 84, 80, 76, 72, 68, 64, 60, 56, 52, 48, 44, 40, 37, 34, 32, 29, 27, 26, 25, 24, 24, 24, 25, 26, 27, 29, 32, 34, 47, 40, 44, 48, 52, 56, 60 };
const uint32_t mey[60] = { 24, 24, 25, 26, 27, 29, 32, 34, 37, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 91, 94, 96, 99, 101, 102, 103, 104, 104, 104, 103, 102, 101, 99, 96, 94, 91, 88, 84, 80, 76, 72, 68, 64, 60, 56, 52, 48, 44, 40, 37, 34, 32, 29, 27, 26, 25, 24 };



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
	ST7735_Circle(64,64,60,ST7735_WHITE);
	ST7735_DrawClock();	
	ST7735_Line(64, 64, hex[hours], hey[hours], ST7735_BLUE);
	ST7735_Line(64, 64, mex[minutes], mey[minutes], ST7735_GREEN);
	ST7735_Line(64, 64, mex[seconds], mey[seconds], ST7735_RED);
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
