#include "../led/led.h"
#include "Serial.h"
#include "../LPC17xx.H"                              /* LPC17xx definitions    */

char buffer_out[100]="\0";

void UART0_IRQHandler(){	
	static int i = 0;
	char car;
	
	switch(LPC_UART0->IIR & 0xFE){
		case 0x04:			
			car = LPC_UART0->RBR;
			LED_Out(car);
			LPC_UART0->THR = car;		/* ECHO */		
			break;
		case 0x02:
			if(buffer_out[i]!='\0'){ 
					i++;
					LPC_UART0->THR = buffer_out[i];
			}
			else{
					i=0;
					buffer_out[0]='\0';
			}
			break;
		case 0x03:
			LPC_UART0->THR = '@';
			LED_Out(0xFF);
			break;
	}
	return;
}
