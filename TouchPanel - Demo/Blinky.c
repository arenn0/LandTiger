#include "LPC17xx.H"                    /* NXP LPC17xx definitions */
#include "string.h"
#include "GLCD.h"
#include <stdlib.h>
#include "GLCD_TTT.h"
#include "TouchPanel.h"
#include <stdio.h>
#include "./serial/serial.h"
#include "./timer/timer.h"
#include "./led/led.h"
#include "./button_EXINT/button.h"

#define DBG 0 // Graphic demo to simulate a game configuration
int main (void) {                      
  
  SystemInit();
  GLCD_Init   ();
  GLCD_Clear  (Navy);
  GLCD_SetBackColor(Navy);
	
	//Set start interface
	GLCD_SetTextColor(Red);
	GLCD_DisplayString(72,108,"Tic-Tac-Toe");
	
	_wait_delay(40000000);
	GLCD_Clear  (Navy);
	
	//Set lattice 
  GLCD_DisplayTTTLattice(60,18,200,200,Red,3);
	GLCD_DisplayVerticalString(20,10,"TicTacToe");
	
#if DBG
  GLCD_DisplayString(50,0,"Hello, World!");
  
  GLCD_DisplayString(85,45,"X");
  GLCD_DisplayString(85,45+60,"X");
  GLCD_DisplayString(85,45+125,"X");
  GLCD_DisplayString(85+130,45+130,"O");
  GLCD_DisplayString(85+64,45+63,"O");
  GLCD_DisplayLine(85+8,35,85+8, 45+155,Yellow,5);
  GLCD_SetTextColor(Red);					  
  
  GLCD_DisplayDiagonalLine(72,35,165,Yellow, 5);
  GLCD_DisplayAntidiagonalLine(245,35,165,Yellow, 5);
  
  TTT_DisplayMove(0,0,Red,1); 
  TTT_DisplayMove(1,0,Red,1);	
  TTT_DisplayMove(2,0,Red,1);
  TTT_DisplayMove(1,1,Red,1);	
  TTT_DisplayMove(0,1,Red,0);	
  TTT_DisplayMove(0,2,Red,1);	
  TTT_DisplayMove(1,2,Red,0);
  TTT_DisplayMove(2,1,Red,1);
  TTT_DisplayMove(2,2,Red,0);
	SER_init(0); 								// Serial communication is used to check the correct execution of interrupt
	
	
#endif
	LED_init();									// LED Initialization
  BUTTON_init();							// Button Initialization
	
	init_timer(0,4000); 
	enable_timer(0);	

  TP_Init();									// Touch Screen initialization

	while(1){
	}
	
}
