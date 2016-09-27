#include <stdio.h>
#include <stdint.h>

//returns a pointer that points to the start of the value of the temperature

char* Temp_Num_Start_Pntr(char* Recvbuff){

	uint32_t found = 0, i = 0;
	char* retVal = 0;
	
	while(found != 1){
	
		if(Recvbuff[i] == 't'){
		
			if(Recvbuff[i+1] == 'e'){
			
				if(Recvbuff[i+2] == 'm'){
				
					if(Recvbuff[i+3] == 'p'){
					
						retVal = &(Recvbuff[i+6]); // the +6 skips out on ": starting with the number directly
						found = 1;
						
					}
				
				}
				
			}
		
		}
		i += 1; //get new position in Recvbuff
	
	}
	
	return retVal;

}
