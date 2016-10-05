
// DAC.c
// Runs on TM4C123
// DAC Driver


#include <stdint.h>
#include "tm4c123gh6pm.h"

/**
 * Initialize the DAC, setting up any necessary ports and pins.
 */
void DAC_Init(){
  SYSCTL_RCGCSSI_R |= 0x08;       // activate SSI3
  SYSCTL_RCGCGPIO_R |= 0x08;      // activate port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){};// ready?
  GPIO_PORTD_AFSEL_R |= 0x0B;     // enable alt funct on PD 0,1,3
  GPIO_PORTD_DEN_R |= 0x0B;       // configure PD 0,1,3 as SSI
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0F00)+0x00001011;
  GPIO_PORTD_AMSEL_R = 0;         // disable analog functionality on PD
  SSI3_CR1_R = 0x00000000;        // disable SSI, master mode
  SSI3_CPSR_R = 0x04;             // 20 MHz SSIClk 
  //SSI3_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 0, SPO = 0 Freescale
  SSI3_CR0_R |= 0x04F;            // DSS = 16-bit data, SPO = 1, SPH = 0
  SSI3_CR1_R |= 0x00000002;       // enable SSI
}

/**
 * Set the DAC output voltage to the specified value.
 * The output is limited to 12 bits so `outputVoltage` will be truncated to 12 bits before setting the output.
 */
void DAC_Output(uint16_t outputVoltage) {
  while ((SSI_SR_TNF & SSI3_SR_R) == 0) {}
  SSI3_DR_R = outputVoltage & 0x0FFF;                  // data out
}
