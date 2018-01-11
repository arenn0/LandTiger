/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through EINT buttons
 * Note(s):
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"

/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in funct_led								*/




__asm void nop( void )
{
    nop
    nop
    nop
}










/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/









int main (void) {
  int i=0;
  SystemInit();  												/* System Initialization (i.e., PLL)  */
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */
	
  while (1) {                           /* Loop forever                       */	
  }

}
 
void convert (uint8_t number){

  LPC_PINCON->PINSEL4 &= 0xFFFF0000;	//PIN mode GPIO (00b value per p2.0 p2.1 p2.2 p2.3 
  LPC_GPIO2->FIODIR   |= 0x000000FF;  //P2.0...P2.7 Output LEDs on PORT2 defined as Output 
  
  LPC_PINCON->PINSEL1 &= (0 << 10);
  LPC_PINCON->PINSEL1 &= (0 << 11);	  //P0.21 on PORT0 defined as GPIO
  LPC_GPIO0->FIODIR   |= (1 << 21);	  //P0.21 on PORT0 configured as Output

  LPC_PINCON->PINSEL1 &= (0 << 6);
  LPC_PINCON->PINSEL1 &= (0 << 7);	  //P0.19 on PORT0 defined as GPIO
  LPC_GPIO0->FIODIR   &= (0 << 19);	  //P0.19 on PORT0 configured as Buffer Output

  LPC_PINCON->PINSEL1 &= (0 << 8);
  LPC_PINCON->PINSEL1 &= (0 << 9);	  //P0.20 on PORT0 defined as GPIO
  LPC_GPIO0->FIODIR   |= (1 << 20);	  //P0.20 on PORT0 configured as Latch Enable On

  LPC_GPIO2->FIOMASK = 0xFFFFFF00;
  LPC_GPIO2->FIOPIN	 = number;

  LPC_GPIO0->FIODIR   &= (0 << 20);	  //P0.20 on PORT0 configured as Latch Enable Off
  
  LPC_GPIO2->FIOCLR	 = 0x000000FF;	  //Assign 0 to MSBs

  LPC_PINCON->PINSEL1 &= (0 << 12);
  LPC_PINCON->PINSEL1 &= (0 << 13);	  //P0.22 on PORT0 defined as GPIO
  LPC_GPIO0->FIODIR   &= (0 << 22);	  //P0.22 on PORT0 configured as Device Selection

  LPC_PINCON->PINSEL1 &= (0 << 14);
  LPC_PINCON->PINSEL1 &= (0 << 15);	  //P0.23 on PORT0 defined as GPIO
  LPC_GPIO0->FIODIR   &= (0 << 23);	  //P0.23 on PORT0 configured as Data Register

  LPC_PINCON->PINSEL1 &= (0 << 16);
  LPC_PINCON->PINSEL1 &= (0 << 17);	  //P0.24 on PORT0 defined as GPIO
  LPC_GPIO0->FIODIR   &= (0 << 24);	  //P0.24 on PORT0 configured as Data START

  // NOP NOP NOP
  for (i=0; i<=1000; i++){
  	nop();
  }
  LPC_GPIO0->FIODIR   |= (1 << 24);	  //P0.24 on PORT0 configured as Data END

  LPC_GPIO0->FIODIR   |= (1 << 22);	  //P0.22 on PORT0 switched off as Device Selection
}
