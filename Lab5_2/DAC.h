// DAC.h
// Runs on TM4C123
// DAC Driver


#include <stdint.h>
#include "tm4c123gh6pm.h"

#ifndef DAC_Hf
#define DAC_H 1

/**
 * Initialize the DAC, setting up any necessary ports and pins.
 */
void DAC_Init(uint16_t initialData);

/**
 * Set the DAC output voltage to the specified value.
 * The output is limited to 12 bits so `outputVoltage` will be truncated to 12 bits before setting the output.
 */
void DAC_Output(uint16_t outputVoltage);

#endif
