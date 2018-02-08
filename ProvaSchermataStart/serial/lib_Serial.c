/*--------------------------------------------------------------------------------------------
Name: lib_Serial.c
Purpose: Init and print detected interrupts  
Note(s): This work is licensed under the Creative Commons Attribution 3.0 Italy License. 
				 To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/it/ 
				 or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
--------------------------------------------------------------------------------------------*/

#include "../LPC17xx.H"                              /* LPC17xx definitions    */
#include "Serial.h"

/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
void SER_init (int uart) {
  LPC_UART_TypeDef *pUart;
   
  if (uart == 0) {                               /* UART0 */
    LPC_PINCON->PINSEL0 |= (1 << 4);             /* Pin P0.2 used as TXD0 (Com0) */
    LPC_PINCON->PINSEL0 |= (1 << 6);             /* Pin P0.3 used as RXD0 (Com0) */
		NVIC_EnableIRQ(UART0_IRQn);             		 /* enable UART0 Interrupt       */
    pUart = (LPC_UART_TypeDef *)LPC_UART0;
  } else {                                       /* UART2 */
    LPC_PINCON->PINSEL0 |= (2 << 8);             /* Pin P0.10 used as TXD1 (Com2) */
    LPC_PINCON->PINSEL0 |= (2 << 10);            /* Pin P0.11 used as RXD1 (Com2) */
		NVIC_EnableIRQ(UART2_IRQn);             		 /* enable UART2 Interrupt       */
    pUart = (LPC_UART_TypeDef *)LPC_UART2;
  }

  pUart->LCR    = 0x03;                          /* 8 bits, no Parity, 1 Stop bit  */
	pUart->LCR    |= 0x80;                         /* DLAB = 1  */
	/* 115200 Baud Rate @ 25.0 MHZ PCLK */
	/* without using prescaler and fractional divider */
	pUart->DLL    = 13;                          	 /* DLR = FR / (16*baud rate) */
	pUart->DLM    = 0;                             /* DLR = 25MHz /(16*11520) = 13.56 -> 13 */
	pUart->FDR    = 0x10;                        	 /* not used FDR -> DIVADDVAL = 1, MULVAL = 0 */
	/* real Baud Rate 120192 */
	
	/* 115200 Baud Rate @ 25.0 MHZ PCLK */
	/* using prescaler and fractional divider */
  //pUart->DLL    = 9;                    				 
  //pUart->DLM    = 0;                             
	//pUart->FDR    = 0x21; 
	/* real Baud Rate 115740 */	
  
  pUart->LCR    &= 0x7F;                          /* DLAB = 0                       */
	pUart->IER		= 0x03;													 /* RBR e THRE interrupt enabled 						*/

	
}

/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/

void SER_putFirstChar(int uart, char c) {
  LPC_UART_TypeDef *pUart;
  pUart = (uart == 0) ? (LPC_UART_TypeDef *)LPC_UART0 : (LPC_UART_TypeDef *)LPC_UART2;
  pUart->THR = c;
}

