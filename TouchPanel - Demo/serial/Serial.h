/*--------------------------------------------------------------------------------------------
Name: Serial.h
Purpose: Prototypes of functions for serial communication  
Note(s): This work is licensed under the Creative Commons Attribution 3.0 Italy License. 
				 To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/it/ 
				 or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
--------------------------------------------------------------------------------------------*/

#ifndef __SERIAL_H
#define __SERIAL_H

/* init_serial.c */
void SER_init      		(int uart);
void SER_putFirstChar (int uart, char c);

/* IRQ_serial.c */
void UART0_IRQHandler(void);

#endif
