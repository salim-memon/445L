


#include <stdint.h>
#include "tm4c123gh6pm.h"





void init_song();
void playsong (void);
void Timer1_Init(void);
void Timer1_Handler(void);

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

// *******Increase_Tempo*******
void increasetempo(void);





