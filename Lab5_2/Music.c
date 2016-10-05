#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "Timer0A.h"
#include "Switch.h"
#include "Systick.h"
#include "DAC.h"

extern bool play_song;

#define C 1493 // 523.251 Hz
#define D 2660
#define E 2370
#define F 2237
#define Fsharp 2112
#define G 1993
#define A	1776
#define B 1582 
#define C_Octaveup 2986
#define D_Octaveup 5321
#define G_Octaveup 3986
#define B_Octavedown 791
#define A_Octavedown 888
#define G_Octavedown 997

#define sixteenth 1
#define eigth 2
#define quarter 4
//#define third 7
#define half	8
#define whole	16

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts	
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

struct Song{
	uint32_t index;
	uint32_t* Note;
	uint32_t* Beat;
	int end;
}starwarstheme;

int end = 88;
uint32_t duration=0;
uint32_t wave_index = 0;


uint32_t StarWars_Notes[88] = 
	{D, D, D, 
	 G, D_Octaveup, 
	 C_Octaveup, B, A, G_Octaveup, D_Octaveup, 
	 C_Octaveup, B, A, G_Octaveup, D_Octaveup,
	 C_Octaveup, B, C_Octaveup, A, 
	 D, D, D, 
	 G, D_Octaveup, 
	 C_Octaveup, B, A, G_Octaveup, D_Octaveup, 
	 C_Octaveup, B, A, G_Octaveup, D_Octaveup,
	 C_Octaveup, B, C_Octaveup, A, 
	 D, D,
	 E, E, C_Octaveup, B, A, G,
	 G, A, B, A, E, Fsharp, D, D, 
	 E, E, C_Octaveup, B, A, G, 
	 D_Octaveup, A, D, D,
	 E, E, C_Octaveup, B, A, G, 
	 G, A, B, A, E, Fsharp, D, D,
	 G, F, E, D, C, B_Octavedown, A_Octavedown, G_Octavedown, 
	 D	
};
	

uint32_t StarWars_Beat[88] = 
	{eigth, eigth, eigth,
	 half, half,
	 eigth, eigth, eigth, half, eigth,
	 eigth, eigth, eigth, half, eigth,
	 eigth, eigth, eigth, half,  
	 eigth, eigth, eigth,
	 half, half, 
	 eigth, eigth, eigth, half, eigth,
	 eigth, eigth, eigth, half, eigth,
	 eigth, eigth, eigth, half, 
	 eigth, eigth,
	 quarter, sixteenth, eigth, eigth, eigth, eigth,
   eigth, eigth, eigth, eigth, eigth, eigth, eigth, eigth,		
	 quarter, sixteenth, eigth, eigth, eigth, eigth,
	 quarter, half, eigth, eigth,
	 quarter, sixteenth, eigth, eigth, eigth, eigth,
	 eigth, eigth, eigth, eigth, eigth, eigth, eigth, eigth,
	 eigth, eigth, eigth, eigth, eigth, eigth, eigth, eigth,
	 whole
	
};

const unsigned short Wave[64] = {
	2048,2224,2399,2571,2737,2897,3048,3190,3321,3439,3545,3635,3711,3770,3813,3839,3848,3839,3813,3770,
	3711,3635,3545,3439,3321,3190,3048,2897,2737,2571,2399,2224,2048,1872,1697,1525,1359,1199,1048,906,775,
	657,551,461,385,326,283,257,248,257,283,326,385,461,551,657,775,906,1048,1199,1359,1525,1697,1872
};

const unsigned short oboe[64] = {
	2126,2164,2238,2550,3356,3496,2550,1510,1322,1322,1406,1462,1538,1690,2078,2268,2418,2664,2930,
	3090,2854,3176,2740,2172,1416,1038,896,980,1132,1368,1604,1984,2126,2164,2238,2550,3356,3496,2550,
	1510,1322,1322,1406,1462,1538,1690,2078,2268,2418,2664,2930,3090,2854,3176,2740,2172,1416,1038,
	896,980,1132,1368,1604,1984
};

const unsigned short trumpet[64] = {
	2014,2176,2312,2388,2134,1578,606,198,1578,3020,2952,2346,2134,2074,2168,2124,2022,2030,2090,2124,
	2022,2022,2116,2226,2168,2150,2158,2210,2176,2098,2030,2090,2014,2176,2312,2388,2134,1578,606,198,
	1578,3020,2952,2346,2134,2074,2168,2124,2022,2030,2090,2124,2022,2022,2116,2226,2168,2150,2158,
	2210,2176,2098,2030,2090
};

const unsigned short flute[64] = {
	2014,2504,2748,3096,3396,3594,3650,3594,3350,3124,2766,2438,2184,2014,1826,1780,1666,1694,1620,1554,
	1488,1348,1196,1102,1018,952,990,1066,1178,1318,1516,1752,2014,2504,2748,3096,3396,3594,3650,3594,
	3350,3124,2766,2438,2184,2014,1826,1780,1666,1694,1620,1554,1488,1348,1196,1102,1018,952,990,1066,
	1178,1318,1516,1752
};

//defining play, pause, reset,and mode functions for the switches

void Play(void){
	play_song = true;
	DAC_Output(Wave[wave_index%64]);
	EnableInterrupts();
}
	
void Pause(void){	
	play_song = false;
	DisableInterrupts();
}
	
void Reset(void){
	DisableInterrupts();
	long sr = StartCritical();
	starwarstheme.index = 0;
	wave_index = 0;
  EndCritical(sr);

}

int instrument = 0;

void Mode_Instrument(void){
 //long sr = StartCritical();
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
	TIMER0_TAILR_R = 50000000/(starwarstheme.Note[starwarstheme.index]*64); // 4) reload value
	while((GPIO_PORTE_DATA_R&0x02)!=0x02){}
	if((GPIO_PORTE_DATA_R&0x02)==0x02){//external button is pressed
		while((GPIO_PORTE_DATA_R&0x02)==0x02){}
		instrument+=1;
		if(instrument >= 4){
				instrument = 0;
		}
		//determining instruments
		if(instrument==1){
			DAC_Output(oboe[wave_index]);
		}
		else if(instrument==2){
			DAC_Output(trumpet[wave_index]);
		}
		else if(instrument==3) {
			DAC_Output(flute[wave_index]);
			}
		}
		wave_index+=1;
		wave_index=wave_index%64;
	  //EndCritical(sr);
}

void init_Song(void){
	starwarstheme.index = 0;
	starwarstheme.Note=StarWars_Notes;
	starwarstheme.Beat=StarWars_Beat;
	starwarstheme.end=end;
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	TIMER0_TAILR_R = 50000000/(starwarstheme.Note[starwarstheme.index]*64);
}

void Timer1_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1 timeout
	duration+=1;
	if(duration > starwarstheme.end){
	  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A
		TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1
		Pause();
		starwarstheme.index = 0;
		duration = 0;
		wave_index = 0;
	}
	else if(duration <= starwarstheme.end){
		starwarstheme.index+=1;
		TIMER1_TAILR_R = starwarstheme.Beat[starwarstheme.index];
		TIMER0_TAILR_R = starwarstheme.Note[starwarstheme.index];
	}
}





/*
void Timer2_Init(void){ 
	unsigned long volatile delay; 
	SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2 
	delay = SYSCTL_RCGCTIMER_R;
	TIMER2_CTL_R = 0x00000000;   // 1) disable timer2A 
	TIMER2_CFG_R = 0x00000000;   // 2) 32-bit mode 
	TIMER2_TAMR_R = 0x00000002;  // 3) periodic mode 
	TIMER2_TAILR_R = 0;   // 4) reload value 
	TIMER2_TAPR_R = 0;           // 5) clock resolution 
	TIMER2_ICR_R = 0x00000001;   // 6) clear timeout flag 
	TIMER2_IMR_R = 0x00000001;   // 7) arm timeout 
	NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4 
	NVIC_EN0_R = 1<<23;          // 9) enable IRQ 23 in 
	TIMER2_CTL_R = 0x00000001;   // 10) enable timer2A 
}
void Timer2A_Handler(void){
 TIMER2_ICR_R = 0x01;// acknowledge TIMER1A timeout
 Speed_Up();
}
 void Speed_Up(void){
	TIMER2_TAILR_R = TIMER2_TAILR_R/2;
}*/
