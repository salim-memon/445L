
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


const uint8_t Hoursx[14] = { 64, 74, 81, 84, 81, 74, 64, 54, 47, 44, 47, 54, 64, 74 };
const uint8_t Hoursy[14] = { 44, 47, 54, 64, 74, 81, 84, 81, 74, 64, 54, 47, 44, 47 };
const uint8_t Minutesx[60] = { 64, 68, 72, 76, 80, 84, 88, 91, 94, 96, 99, 101, 102, 103, 104, 104, 104, 103, 102, 101, 99, 96, 94, 91, 88, 84, 80, 76, 72, 68, 64, 60, 56, 52, 48, 44, 40, 37, 34, 32, 29, 27, 26, 25, 24, 24, 24, 25, 26, 27, 29, 32, 34, 37, 40, 44, 48, 52, 56, 60 };
const uint8_t Minutesy[60] = { 24, 24, 25, 26, 27, 29, 32, 34, 37, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 91, 94, 96, 99, 101, 102, 103, 104, 104, 104, 103, 102, 101, 99, 96, 94, 91, 88, 84, 80, 76, 72, 68, 64, 60, 56, 52, 48, 44, 40, 37, 34, 32, 29, 27, 26, 25, 24 };
const uint8_t Currentx[60] = { 64, 69, 74, 79, 84, 89, 93, 97, 101, 104, 107, 109, 112, 113, 114, 114, 114, 113, 112, 109, 107, 104, 101, 97, 93, 89, 84, 79, 74, 69, 64, 59, 54, 49, 44, 39, 35, 31, 27, 24, 21, 18, 16, 15, 14, 14, 14, 15, 16, 18, 21, 24, 27, 31, 35, 39, 44, 49, 54, 59 };
const uint8_t Currenty[60] = { 14, 14, 15, 16, 18, 21, 24, 27, 31, 35, 39, 44, 49, 54, 59, 64, 69, 74, 79, 84, 89, 93, 97, 101, 104, 107, 109, 112, 113, 114, 114, 114, 113, 112, 109, 107, 104, 101, 97, 93, 89, 84, 79, 74, 69, 64, 59, 54, 49, 44, 39, 35, 31, 27, 24, 21, 18, 16, 15, 14 };


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
	ST7735_Line(64, 64, Hoursx[hours], Hoursy[hours], ST7735_BLUE);
	ST7735_Line(64, 64, Minutesx[minutes], Minutesy[minutes], ST7735_GREEN);
	ST7735_Line(64, 64, Currentx[seconds], Currenty[seconds], ST7735_RED);
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
