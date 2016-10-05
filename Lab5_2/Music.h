#include <stdint.h>
#include "tm4c123gh6pm.h"

void init_Song (void);
void Timer0A_Handler(void);
void Timer1A_Handler(void);
// *******Play*******
// Plays music
// Input: none
// Output: none
void Play(void);

// *******Pause*******
// Pauses Music
// Input: none
// Output: none
void Pause(void);

// *******Reset*******
// Puts music back to the beginning
// Input: none
// Output: none
void Reset(void);

// *******Mode_Instrument*******
// Choose Instrument
// Input: none
// Output: none
void Mode_Instrument(void);



//void Timer2_Init(void);

