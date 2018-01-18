#include "./button_EXINT/button.h"
#include "lpc17xx.h"
#include "serial/serial.h"
#include <string.h>
#include "TouchPanel.h"
#include "GLCD.h"
#include "stdlib.h"
#include <stdio.h>
#include "GLCD_TTT.h"

extern char buffer_out[100];
char stringa[100];
void EINT0_IRQHandler (void)	  
{
  strcpy(buffer_out, "\n\rINT0 pressed\n");
	SER_putFirstChar(0,buffer_out[0]);
	
	LPC_SC->EXTINT |= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  
{
	strcpy(buffer_out, "\n\rKEY1 pressed\n");
	SER_putFirstChar(0,buffer_out[0]);
	
  LPC_SC->EXTINT |= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  
{
	strcpy(buffer_out, "\n\rKEY2 pressed\n");
	SER_putFirstChar(0,buffer_out[0]);
	
  LPC_SC->EXTINT |= (1 << 2);     /* clear pending interrupt         */    
}

void EINT3_IRQHandler (void){
		Coordinate * coord;
		//char * stringa=malloc(20*sizeof(char));
	
	
		strcpy(buffer_out, "\n\rTOUCH SCREEN pressed \n");
		SER_putFirstChar(0,buffer_out[0]);
	
		coord = Read_Ads7846();
	if(coord->x != 0 && coord->y != 0){
	  sprintf(stringa, "x=%d y=%d", coord->x, coord->y);
			 
	}
	  
		LPC_SC->EXTINT |= (1 << 3);
	  //delay(50);
	  if(coord->x != 0 && coord->y != 0){
			//GLCD_Clear(Navy);
			//GLCD_DisplayString(0,0,(unsigned char *)stringa);
			TTT_YourMove(coord->x,coord->y);
		}
}

