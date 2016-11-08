#include <stdint.h>

uint16_t const ADCdata[53]={0,9,44,79,116,153,190,229,268,308,348,							
     390,432,475,519,564,610,657,704,752,802,							
     852,903,956,1009,1063,1119,1175,1233,1291,1351,							
     1412,1474,1537,1601,1666,1733,1801,1870,1941,2012,							
     2085,2160,2235,2312,2391,2470,2552,2634,2718,2803,2890,4096};							

		 
uint16_t const Tdata[53]={4000,4000,3940,3880,3820,3760,3700,3640,3580,3520,3460,		
     3400,3340,3280,3220,3160,3100,3040,2980,2920,2860,		
     2800,2740,2680,2620,2560,2500,2440,2380,2320,2260,		
     2200,2140,2080,2020,1960,1900,1840,1780,1720,1660,		
     1600,1540,1480,1420,1360,1300,1240,1180,1120,1060,1000,1000};		

		 
uint16_t const Rdata[53]={5027,5027,5125,5225,5328,5434,5542,5652,5765,5881,5999,									
     6121,6245,6373,6503,6637,6774,6914,7058,7206,7357,									
     7512,7670,7833,8000,8172,8347,8527,8712,8902,9096,									
     9296,9500,9711,9926,10148,10375,10608,10848,11094,11346,									
     11606,11872,12146,12427,12716,13013,13318,13632,13954,14286,14626,14626};									

/*
		 typecasting issues resolved
uint32_t num32 = 0xdeadbeef;
uint8_t a = num32;       // 0xef 
uint8_t b = num32 >> 8;  // 0xbe 
uint8_t c = num32 >> 16; // 0xad 
uint8_t d = num32 >> 24; // 0xde 
*/
		 
//used to approximate inbetween value by subtracting upperBound from lowerBound
uint16_t Linear_Interpolation(uint16_t newData){
	uint16_t upperBound = 0, lowerBound = 0, ADCDifference = 0;
	uint16_t TemperatureDifference = 0, data_difference = 0, subtraction_value = 0;
	for(int i = 0; i<53; i++){
		if(ADCdata[i] >= newData)
			upperBound = i;
	}
	if (upperBound == 0)
		return Tdata[upperBound];
	lowerBound = upperBound-1;
	/*
	return (((((Tdata[upperBound])+(Tdata[lowerBound]))<<4)/2)>>4)
	*/
	data_difference = (newData-(ADCdata[lowerBound]));
	ADCDifference = ((ADCdata[upperBound])-(ADCdata[lowerBound]));
	TemperatureDifference = ((Tdata[lowerBound])-(Tdata[upperBound])); //lowerbound has higher temperature values so to prevenet -ve using difference between lower and upper
	subtraction_value = ((TemperatureDifference * (data_difference<<4)/ADCDifference)>>4);
	return Tdata[lowerBound]-subtraction_value;
}
