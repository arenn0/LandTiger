#ifndef _GLCD_TTT_H
#define _GLCD_TTT_H

#include "GLCD.h"

extern void GLCD_DisplayTTTLattice(unsigned int x0, unsigned int y0, unsigned int width, unsigned int height, unsigned int color, unsigned int thickness);
extern void TTT_DisplayMove(unsigned int ln, unsigned int col, unsigned int color, unsigned int move);

#endif
