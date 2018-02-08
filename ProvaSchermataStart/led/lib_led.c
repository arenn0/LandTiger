/*--------------------------------------------------------------------------------------------
Name: lib_led.c
Purpose: Atomic led init functions
Note(s): This work is licensed under the Creative Commons Attribution 3.0 Italy License. 
				 To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/it/ 
				 or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
--------------------------------------------------------------------------------------------*/

#include "../lpc17xx.h"
#include "led.h"

/*----------------------------------------------------------------------------
  Function that initializes LEDs and switch them off
 *----------------------------------------------------------------------------*/

unsigned char led_value;

void LED_init(void) {

  LPC_PINCON->PINSEL4 &= 0xFFFF0000;	//PIN mode GPIO (00b value per p2.0 p2.1 p2.2 p2.3 
	LPC_GPIO2->FIODIR   |= 0x000000FF;  //P2.0...P2.7 Output LEDs on PORT2 defined as Output
  LPC_GPIO2->FIOSET    = 0x000000FF;	//all LEDs on 
	LPC_GPIO2->FIOCLR    = 0x000000FF;  //all LEDs off
	
	led_value = 0;
}

void LED_deinit(void) {

  LPC_GPIO2->FIODIR &= 0xFFFFFF00;  //P2.0...P2.7 Output LEDs on PORT2 defined as Output
}
