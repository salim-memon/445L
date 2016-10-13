#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "Switch.h"
#include "Systick.h"
#include "DAC.h"
#include "Timer0A.h"
#include "Timer1.h"
#include "Music.h"


#define F64HZ	(80000000/64)
#define F16HZ (80000000/16)
#define ENVELOPE_SCALE 1000
#define A 440
#define B 494
#define D 588
#define G 392
#define C 523 
#define E 330
#define F 349
#define Fsharp 370
 
#define C_Octaveup 1046
#define D_Octaveup 587
#define G_Octaveup 783
#define B_Octavedown 246
#define A_Octavedown 220
#define G_Octavedown 196

#define Sixteenth 2
#define Eighth 4
#define Quarter 8
#define Half	16
#define Whole	32

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts	
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

extern bool play_song;

struct Song{
	uint32_t index;
	uint32_t* Note;
	uint32_t* Beat;
	//uint32_t* wave_index
}starwarstheme;

int songlength = 88;
uint32_t duration=88;
uint32_t wave_index = 0;


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

const unsigned short bassoon[64] = {
	2136,2338,2350,2322,2260,2226,2204,2152,2064,1970,1926,1974,2164,2686,3474,3726,3150,2062,1076,618,
	660,944,1252,1614,2076,2540,2840,2922,2750,2402,2010,1638,1316,1064,992,1188,1608,2110,2496,2646,
	2466,2098,1790,1652,1652,1700,1724,1722,1798,1922,2012,2046,2092,2184,2354,2448,2372,2266,2196,
	2204,2218,2152,2054,2006
};


starwarssong StarWars [] = {
	 {D, Eighth,Bassoon},{D,Eighth,Bassoon}, {D,Eighth,Bassoon},
	 {G, Half,Bassoon},{D_Octaveup, Half ,Bassoon}, 
	 
	 {C_Octaveup, Eighth ,Trumpet},{B, Eighth ,Trumpet},{A, Eighth ,Trumpet},{G_Octaveup, Half ,Trumpet}, {D_Octaveup, Half ,Trumpet}, 
	 {C_Octaveup, Eighth ,Trumpet},{B, Eighth ,Trumpet},{A, Eighth ,Trumpet},{G_Octaveup, Half ,Trumpet}, {D_Octaveup, Half ,Trumpet}, 
	 {C_Octaveup, Eighth ,Trumpet}, {B, Eighth ,Trumpet},{C_Octaveup, Eighth ,Trumpet}, {A, Half ,Trumpet},
	 
	 {D, Eighth,Bassoon},{D,Eighth,Bassoon}, {D,Eighth,Bassoon},
	 {G, Half,Bassoon},{D_Octaveup, Half ,Bassoon}, 
	 
	 {C_Octaveup, Eighth ,Trumpet},{B, Eighth ,Trumpet},{A, Eighth ,Trumpet},{G_Octaveup, Half ,Trumpet}, {D_Octaveup, Half ,Trumpet}, 
	 {C_Octaveup, Eighth ,Trumpet},{B, Eighth ,Trumpet},{A, Eighth ,Trumpet},{G_Octaveup, Half ,Trumpet}, {D_Octaveup, Half ,Trumpet}, 
	 {C_Octaveup, Eighth ,Trumpet}, {B, Eighth ,Trumpet},{C_Octaveup, Eighth ,Trumpet}, {A, Half ,Trumpet},
	 
	{D, Eighth,Trumpet},{D,Eighth,Trumpet},
	{E, Quarter,Trumpet}, {E, Eighth ,Trumpet}, {C_Octaveup, Eighth ,Trumpet},{B, Eighth ,Trumpet},{A, Eighth ,Trumpet},{G, Eighth ,Trumpet}, 
	{G, Sixteenth ,Trumpet}, {A, Sixteenth ,Trumpet}, {B, Sixteenth ,Trumpet},{A, Eighth ,Trumpet}, {E, Eighth ,Trumpet}, {Fsharp,Quarter,Trumpet},{D, Eighth,Trumpet},{D,Eighth,Trumpet},
	{E, Quarter,Trumpet}, {E, Eighth ,Trumpet}, {C_Octaveup, Eighth ,Trumpet},{B, Eighth ,Trumpet},{A, Eighth ,Trumpet},{G, Eighth ,Trumpet}, 
	{D_Octaveup, Quarter, Trumpet},{A, Half,Trumpet},{D, Eighth,Trumpet},{D,Eighth,Trumpet},
	{E, Quarter,Trumpet}, {E, Sixteenth ,Trumpet}, {C_Octaveup, Eighth ,Trumpet},{B, Eighth ,Trumpet},{A, Eighth ,Trumpet},{G, Eighth ,Trumpet},  
	{G, Sixteenth ,Trumpet}, {A, Sixteenth ,Trumpet}, {B, Sixteenth ,Trumpet},{A, Eighth ,Trumpet}, {E, Eighth ,Trumpet}, {Fsharp,Quarter,Trumpet},	{D, Eighth,Trumpet},{D,Eighth,Trumpet},
	{G, Eighth ,Trumpet}, {F, Eighth ,Trumpet}, {E, Eighth ,Trumpet},{D, Eighth ,Trumpet}, {C, Eighth ,Trumpet}, {B_Octavedown,Eighth,Trumpet}, {A_Octavedown, Eighth,Trumpet},{G_Octavedown,Eighth,Trumpet}, 
	{D,Whole,Bassoon},
   {0, Whole, Trumpet}	
};




void init_Song(void){
	starwarstheme.index = 0;
	starwarstheme.Note=StarWars_Notes;
	starwarstheme.Beat=StarWars_Beat;
}

//plays the song and increase index until the end
void playsong(void)
	StarWars_Beat[starwarstheme.index] = *starwarstheme.Beat;
	 starwarstheme.Note=StarWars_Notes;
	starwarstheme.index += 1;
	if(starwarstheme.index <= songlength) {
		starwarstheme.index +=1;
		beatIndex = 0;
	  }
	}
	
}


void Timer1_Init(void){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 0xFFFFFFFE;    // 4) reload value
	//TIMER1_TAILR_R = period-1; // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

//Timer 1 Handles the frequency (notelength)
void Timer1_Handler(void){
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	duration +=1 ;
	if(duration == songlength){
	  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A
		TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1
		Pause();
		starwarstheme.index = 0;
		duration = 0;
		wave_index = 0;
		}
		else {
			starwarstheme.index+=1;
			TIMER1_TAILR_R = starwarstheme.Beat[starwarstheme.index];
			TIMER0_TAILR_R = 0;
		}
}


//defining play, pause, reset,and mode functions for the switches
void Play(void){
	play_song = true;
  EnableInterrupts();
	
}

void Pause(void){	
	play_song = false;
	DisableInterrupts();

}

void Reset(void){
	DisableInterrupts();
	NoteIndex = 0;
	BeatIndex = 0;
	EnableInterrupts();
}

void increasetempo(void){
	TIMER0_TAILR_R = F64HZ-1;// increase tempo
}

