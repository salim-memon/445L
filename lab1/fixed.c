/* ******** fixed.c ************** 
*Priscilla Chang, pc23384
*Salim Memon, sam6345
*Initial Creation Date: 1 Sep 2016
*Descritpion: Contains definitions for functions that print 
*fixed point display functions and plot functions.
*Lab 1
*TA: Corey
*Date of Last Revision: 2 Sep 2016
 */

#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"

 /****************ST7735_sDecOut3***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.001
 range -9.999 to +9.999
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " *.***"
  2345    " 2.345"  
 -8100    "-8.100"
  -102    "-0.102" 
    31    " 0.031" 
-12345    " *.***"
 */ 

void ST7735_sDecOut3(int32_t n){
  if( n >= 10000 || n <= -10000){ //making sure the number is within limit
     printf("*.***");
    }
     else{
  int fstElm = n/1000; //extracting first element
  int restElm = n%1000; // extracting the rest of the elements
  if (restElm < 0)
     restElm = -restElm; //making the number positive
  if(fstElm < 0)
     fstElm = -fstElm; // making the number positive
  if (n < 0)
     printf("-%d.%d",fstElm,restElm); //if the number was negative print with a negative
  else
     printf("%d.%d",fstElm,restElm); //print the number as is
  }
}


/**************ST7735_uBinOut8***************
 unsigned 32-bit binary fixed-point with a resolution of 1/256. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 256000, it signifies an error. 
 The ST7735_uBinOut8 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0    "  0.00"
     2    "  0.01"
    64    "  0.25"
   100    "  0.39"
   500    "  1.95"
   512    "  2.00"
  5000    " 19.53"
 30000    "117.19"
255997    "999.99"
256000    "***.**"
*/

void ST7735_uBinOut8(uint32_t n){
  uint32_t num = n*100/256;   //obtaining the required number
  uint32_t extraNum = n*1000/256; // grabbing an additional character
  int lastElm = extraNum%10;  //extracting the extra character
  int fstElm = num/100;
  int restElm = num%100;
  if (lastElm >= 5)   //if the last element is greater than 5 then round up
     restElm += 1;
  if(n >= 256000){    //if the value is too big report in *'s
     printf("***.**");
  }
  else{
     if (restElm < 10)    //if the decimal value is a single digit add an extra zero
         printf("%d.0%d",fstElm,restElm);
     else
         printf("%d.%d",fstElm,restElm); 
  }
}

/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/

//globals for data usage

int32_t static smallestX;
int32_t static largestX;
int32_t static smallestY;
int32_t static largestY;

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0,0);
	ST7735_OutString(title);
	smallestX =  minX;
	largestX =  maxX;
	smallestY =  minY;
	largestY =  maxY;
	
	
}
/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	uint32_t i;
	for(i = 0; i < num; i++) {
		if (bufX[i] < smallestX && bufX[i] > largestX && bufY[i] < smallestY && bufY[i] > largestY ) {
			continue;
		}
    int32_t x = (((bufX[i] - smallestX)*127))/(largestX - smallestX);
    int32_t y = 32 + (((largestY - bufY[i])*127))/(largestY - smallestY);
    ST7735_DrawPixel(x, y, ST7735_BLUE);
	}
}
