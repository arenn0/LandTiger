#include "GLCD_TTT.h"
#include <stdint.h>

#define CHAR_H      24                  /* Character Height (in pixels)       */
#define CHAR_W      16                  /* Character Width (in pixels)        */

typedef struct{
	unsigned int x;
	unsigned int y;
} point;

typedef struct{
		point p;
		unsigned char value;
} cell;

typedef struct{
	point start;
    unsigned int width; 
    unsigned int height; 
    unsigned int color; 
    unsigned int thickness;
	unsigned int horizontalDisp;
    unsigned int verticalDisp;
    unsigned int detach;
	cell pos[3][3];
} lattice;


static lattice lattice0;
// static positions coordinates;



/*	PROTOTYPES	*/

void TTT_init(unsigned int x0, unsigned int y0, unsigned int width, unsigned int height, unsigned int color, unsigned int thickness, unsigned int horizontalDisp, unsigned int verticalDisp, unsigned int detach);
void GLCD_DisplayTTTLattice(unsigned int x0, unsigned int y0, unsigned int width, unsigned int height, unsigned int color, unsigned int thickness);
point computePos(unsigned int i, unsigned int j);
void TTT_DisplayMove(unsigned int ln, unsigned int col, unsigned int color, unsigned int move);
void TTT_CheckVictory(unsigned int ln,unsigned int col,unsigned int move);

/*	END OF PROTOTYPES	*/


#define TTT_CheckMove(ln,col,move) (lattice0.pos[ln][col].value == move? 1:0)
#define Convert(i) (i?'O':'X')


void _wait_delay(int i){
	int j=0;
	for(;j<i;j++);
}

						  

/*******************************************************************************
* Display Lattice for Tic-Tac-Toe game.                                        *
*   Parameter:      x0:         horizontal starting position                   *
*                   y0:         vertical starting position                     *
*                   width:                                                     *
*                   height:                                                    *
*                   color:      color of line                                  *
*                   thickness:  thickness of line                              *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_DisplayTTTLattice(unsigned int x0, unsigned int y0, unsigned int width, unsigned int height, unsigned int color, unsigned int thickness){

  unsigned int HorizontalDisp = width / 3;
  unsigned int VerticalDisp = height /3;
  unsigned int detach = 10;

  TTT_init(x0,y0,width,height,color,thickness,HorizontalDisp,VerticalDisp,detach);
  


  GLCD_DisplayLine(x0 + HorizontalDisp, y0 + detach, x0 + HorizontalDisp, y0 + height -detach, color, thickness); /*      1 Vertical Line       */
  GLCD_DisplayLine(x0 + 2*HorizontalDisp, y0 + detach, x0 + 2*HorizontalDisp, y0 + height -detach, color, thickness); /*      2 Vertical Line       */

  GLCD_DisplayLine(x0 + detach, y0 + VerticalDisp, x0 + width - detach, y0 + VerticalDisp, color, thickness); /*      1 Horizontal Line       */
  GLCD_DisplayLine(x0 + detach, y0 + 2*VerticalDisp, x0 + width - detach, y0 + 2*VerticalDisp, color, thickness); /*      2 Horizontal Line       */

  GLCD_DisplayLine(x0 , y0 , x0, y0 + height, color, thickness + 2); /*      1 Lattice Vertical Line       */
  GLCD_DisplayLine(x0 + width , y0 , x0 + width, y0 + height, color, thickness + 2); /*      2 Lattice Vertical Line       */

  GLCD_DisplayLine(x0 , y0, x0 + width, y0, color, thickness + 2); /*      1 Lattice Horizontal Line       */
  GLCD_DisplayLine(x0 , y0 + height, x0 + width, y0 + height, color, thickness + 2); /*      2 Lattice Horizontal Line       */
}


void TTT_init(unsigned int x0, unsigned int y0, unsigned int width, unsigned int height, unsigned int color, unsigned int thickness, unsigned int horizontalDisp, unsigned int verticalDisp, unsigned int detach){
  lattice0.start.x=x0;	
  lattice0.start.y=y0;
  lattice0.width=width;
  lattice0.height=height;
  lattice0.color=color;
  lattice0.thickness=thickness;
  lattice0.horizontalDisp=horizontalDisp;
  lattice0.verticalDisp=verticalDisp;
  lattice0.detach=detach;																																															
  
  lattice0.pos[0][0].p=computePos(1,1);
  lattice0.pos[0][0].value=' ';

  lattice0.pos[0][1].p=computePos(1,2);
  lattice0.pos[0][1].value=' ';

  lattice0.pos[0][2].p=computePos(1,3);
  lattice0.pos[0][2].value=' ';

  lattice0.pos[1][0].p=computePos(2,1);
  lattice0.pos[1][0].value= ' ';

  lattice0.pos[1][1].p=computePos(2,2);
  lattice0.pos[1][1].value= ' ';

  lattice0.pos[1][2].p=computePos(2,3);
  lattice0.pos[1][2].value= ' ';

  lattice0.pos[2][0].p=computePos(3,1);
  lattice0.pos[2][0].value= ' ';

  lattice0.pos[2][1].p=computePos(3,2);
  lattice0.pos[2][1].value= ' ';

  lattice0.pos[2][2].p=computePos(3,3);
  lattice0.pos[2][2].value= ' ';  
}



/*		Coordinates where you insert TTT move (for each cell)		*/

point computePos(unsigned int i, unsigned int j){
  point p;
  p.x = lattice0.start.x + j * lattice0.horizontalDisp - lattice0.horizontalDisp/2 - CHAR_W/2;
  p.y = lattice0.start.y + i * lattice0.verticalDisp - lattice0.verticalDisp/2 - CHAR_H/2;
  return p;
}


void TTT_DisplayMove(unsigned int ln, unsigned int col, unsigned int color, unsigned int move){
	if(!TTT_CheckMove(ln,col,' '))  // if 1 full, move not authorized
		return;
	GLCD_SetTextColor(color);
	lattice0.pos[ln][col].value=Convert(move);
	GLCD_DisplayChar(lattice0.pos[ln][col].p.x,   lattice0.pos[ln][col].p.y,    lattice0.pos[ln][col].value);
	TTT_CheckVictory(ln,col,move);
}

void TTT_CheckVictory(unsigned int ln,unsigned int col,unsigned int move){
	
	// Horizontal Check 
	if(TTT_CheckMove(ln,0,Convert(move))&&TTT_CheckMove(ln,1,Convert(move))&&TTT_CheckMove(ln,2,Convert(move))){
		// to do GLCD display line
		GLCD_DisplayString(0,0,"Horizontal!");
		GLCD_DisplayLine(lattice0.pos[ln][0].p.x-CHAR_W/2   ,lattice0.pos[ln][col].p.y+CHAR_H/2, lattice0.pos[ln][2].p.x+ CHAR_W/2 +CHAR_W , lattice0.pos[ln][col].p.y+CHAR_H/2,Yellow,5);
		//_wait_delay(100000000);
	}
	// Vertical Check
	if(TTT_CheckMove(0,col,Convert(move))&&TTT_CheckMove(1,col,Convert(move))&&TTT_CheckMove(2,col,Convert(move))){
		//to do GLCD display vertical line
		GLCD_DisplayVerticalString(0,0,"Vertical!");
		GLCD_DisplayLine(lattice0.pos[0][col].p.x+CHAR_W/2   ,lattice0.pos[0][col].p.y-CHAR_H/2, lattice0.pos[0][col].p.x + CHAR_W/2 , lattice0.pos[2][col].p.y+CHAR_H/2+ CHAR_H,Yellow,5);
		//_wait_delay(100000000);
	}
	// Diagonal Check
	if(TTT_CheckMove(0,0,Convert(move))&&TTT_CheckMove(1,1,Convert(move))&&TTT_CheckMove(2,2,Convert(move))){
		//to do diagonal check
		GLCD_DisplayVerticalString(0,0,"Diagonal!");
		GLCD_DisplayDiagonalLine(72,35,165,Yellow, 5);
		//_wait_delay(100000000);
	}
	// Antidiagonal check
	if(TTT_CheckMove(2,0,Convert(move))&&TTT_CheckMove(1,1,Convert(move))&&TTT_CheckMove(0,2,Convert(move))){
		//to do antidiagonal check
		GLCD_DisplayVerticalString(0,0,"Antidiagonal!");
		GLCD_DisplayAntidiagonalLine(238,35,165,Yellow, 5);
		//_wait_delay(100000000);
	}
}

