/******************************************************************************/
/* Blinky.c: LED Flasher and graphic demo                                     */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2009 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/
                  
#include "LPC17xx.H"                    /* NXP LPC17xx definitions            */
#include "string.h"
#include "GLCD.h"
#include <stdlib.h>
#include "GLCD_TTT.h"
#define DBG 1
#define LED_NUM     8                   /* Number of user LEDs                */
const  unsigned long led_mask[] = { 1<<0, 1<<1, 1UL<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7 };

extern unsigned char ClockLEDOn;
extern unsigned char ClockLEDOff;
extern unsigned char ClockANI;
extern unsigned char Bg_16bpp_t[];
extern unsigned char Bg_16bpp_l[];
extern unsigned char Bg_16bpp_r[];
extern unsigned char Bg_16bpp_b[];
extern unsigned char ARM_Ani_16bpp[];

int main (void) {                       /* Main Program                       */
 
  SystemInit();
  GLCD_Init   ();
  GLCD_Clear  (Navy);
  GLCD_SetBackColor(Navy);
  
#if DBG
  //GLCD_DisplayString(50,0,"Hello, World!");
  GLCD_DisplayTTTLattice(60,18,200,200,Red,3);
  //GLCD_DisplayString(85,45,"X");
  //GLCD_DisplayString(85,45+60,"X");
  //GLCD_DisplayString(85,45+125,"X");
  //GLCD_DisplayString(85+130,45+130,"O");
  //GLCD_DisplayString(85+64,45+63,"O");
  //GLCD_DisplayLine(85+8,35,85+8, 45+155,Yellow,5);
  //GLCD_SetTextColor(Red);					  
  //GLCD_DisplayVerticalString(20,10,"TicTacToe");
  //GLCD_DisplayDiagonalLine(72,35,165,Yellow, 5);
  //GLCD_DisplayAntidiagonalLine(245,35,165,Yellow, 5);
  
  TTT_DisplayMove(0,0,Red,1); //
  TTT_DisplayMove(1,0,Red,1);	   //
  TTT_DisplayMove(2,0,Red,1);
  TTT_DisplayMove(1,1,Red,1);	//
  TTT_DisplayMove(0,1,Red,0);	//
  TTT_DisplayMove(0,2,Red,1);	 //
  TTT_DisplayMove(1,2,Red,0);
  TTT_DisplayMove(2,1,Red,1);
  TTT_DisplayMove(2,2,Red,0);
  







#endif
}
