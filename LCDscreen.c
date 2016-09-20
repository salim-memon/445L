#include <stdint.h>
#include <stdio.h>
#include "ST7735.h"
#include "Switch.h"

extern uint32_t hours, minutes, seconds, alarm_On_Off;

uint32_t alarm_seconds = 4;
uint32_t alarm_minutes = 24;
uint32_t alarm_hours = 5;

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
  ST7735_Line(64,64,hours,hours,ST7735_BLUE);			
	ST7735_Line(64,64,minutes,minutes,ST7735_BLUE);					
  ST7735_Line(64,64,seconds,seconds,ST7735_RED); 	
}