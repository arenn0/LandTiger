/*--------------------------------------------------------------------------------------------
Name: GLCD_TTT.h
Purpose: Prototypes declaration for TTT functions 
Note(s): This work is licensed under the Creative Commons Attribution 3.0 Italy License. 
				 To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/it/ 
				 or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
--------------------------------------------------------------------------------------------*/
#ifndef _GLCD_TTT_H
#define _GLCD_TTT_H

#include "GLCD.h"

extern void GLCD_DisplayTTTLattice(unsigned int x0, unsigned int y0, unsigned int width, unsigned int height, unsigned int color, unsigned int thickness);
extern void TTT_DisplayMove(unsigned int ln, unsigned int col, unsigned int color, unsigned int move);
void TTT_YourMove(int x, int y);
void _wait_delay(int i);

#endif
