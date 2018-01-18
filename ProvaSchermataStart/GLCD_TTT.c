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
static int player = 0;
static int nmoves = 0;
// static positions coordinates;



/*	PROTOTYPES	*/

void TTT_init(unsigned int x0, unsigned int y0, unsigned int width, unsigned int height, unsigned int color, unsigned int thickness, unsigned int horizontalDisp, unsigned int verticalDisp, unsigned int detach);
void GLCD_DisplayTTTLattice(unsigned int x0, unsigned int y0, unsigned int width, unsigned int height, unsigned int color, unsigned int thickness);
point computePos(unsigned int i, unsigned int j);
void TTT_DisplayMove(unsigned int ln, unsigned int col, unsigned int color, unsigned int move);
void TTT_CheckVictory(unsigned int ln,unsigned int col,unsigned int move);
void Map_Into_Lattice(int *x, int *y);

/*	END OF PROTOTYPES	*/

#include "stdlib.h"
#include <string.h>
#include <stdio.h>
#include "lpc17xx.h"
//#include <core_cm3.h>

#define TTT_CheckMove(ln,col,move) (lattice0.pos[ln][col].value == move? 1:0)
#define Convert(i) (i?'O':'X')
#define NCELLS 9

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

/************************************************************************************
* Initialization for TTT structure                                                       *
*   Parameter:      x0:             horizontal starting position                         *
*                   y0:             vertical starting position                           *
*                   width:                                                               *
*                   height:                                                              *
*                   color:          color of line                                        *
*                   thickness:      thickness of line                                   *
*                    horizontalDisp: displacement between two adjacent vertical lines    *
*                    verticalDisp:   displacement between two adjacent horizontal lines  *
*                    detach;         number of empty pixel between border and lattice    *
*   Return:                                                                              *
************************************************************************************/
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



/*    Coordinates where tp print TTT move (for each cell)                                           */
/*    - i and j start from 1 instead of 0 to enumerate the number of rows/colums of the lattice    */
point computePos(unsigned int i, unsigned int j){
  point p;
  p.x = lattice0.start.x + j * lattice0.horizontalDisp - lattice0.horizontalDisp/2 - CHAR_W/2;
  p.y = lattice0.start.y + i * lattice0.verticalDisp - lattice0.verticalDisp/2 - CHAR_H/2;
  return p;
}

/*    Command used to represent one move into the lattice in the correct position     */
void TTT_DisplayMove(unsigned int ln, unsigned int col, unsigned int color, unsigned int move){
	if(!TTT_CheckMove(ln,col,' '))  // if 1 full, move not authorized
		return;
	GLCD_SetTextColor(color);
	lattice0.pos[ln][col].value=Convert(move);
	player = (player + 1) % 2; 
	GLCD_DisplayChar(lattice0.pos[ln][col].p.x,   lattice0.pos[ln][col].p.y,    lattice0.pos[ln][col].value);
	TTT_CheckVictory(ln,col,move);
}

/*    Check for victory configuration     */
void TTT_CheckVictory(unsigned int ln,unsigned int col,unsigned int move){
	point c;
	int victory = 0;
	c=computePos(2,1);
	nmoves++;
	// Horizontal Check 
	if(TTT_CheckMove(ln,0,Convert(move))&&TTT_CheckMove(ln,1,Convert(move))&&TTT_CheckMove(ln,2,Convert(move))){
		// to do GLCD display line
		//GLCD_DisplayString(0,0,"Horizontal!");
		GLCD_DisplayLine(lattice0.pos[ln][0].p.x-CHAR_W/2   ,lattice0.pos[ln][col].p.y+CHAR_H/2, lattice0.pos[ln][2].p.x+ CHAR_W/2 +CHAR_W , lattice0.pos[ln][col].p.y+CHAR_H/2,Yellow,5);
		
		GLCD_SetTextColor(Yellow);
		GLCD_DisplayString(c.x + CHAR_W,c.y,"YOU WIN!");
		NVIC_DisableIRQ(EINT3_IRQn);
		_wait_delay(72000000);
		GLCD_DisplayString(lattice0.start.x+5,c.y + lattice0.verticalDisp,"Press RESET!");
		victory++;
	}
	// Vertical Check
	if(TTT_CheckMove(0,col,Convert(move))&&TTT_CheckMove(1,col,Convert(move))&&TTT_CheckMove(2,col,Convert(move))){
		//to do GLCD display vertical line
		//GLCD_DisplayVerticalString(0,0,"Vertical!");
		GLCD_DisplayLine(lattice0.pos[0][col].p.x+CHAR_W/2   ,lattice0.pos[0][col].p.y-CHAR_H/2, lattice0.pos[0][col].p.x + CHAR_W/2 , lattice0.pos[2][col].p.y+CHAR_H/2+ CHAR_H,Yellow,5);
		//_wait_delay(100000000);
		GLCD_SetTextColor(Yellow);
		GLCD_DisplayString(c.x + CHAR_W,c.y,"YOU WIN!");
		NVIC_DisableIRQ(EINT3_IRQn);
		_wait_delay(72000000);
		GLCD_DisplayString(lattice0.start.x+5,c.y + lattice0.verticalDisp,"Press RESET!");
		victory++;
	}
	// Diagonal Check
	if(TTT_CheckMove(0,0,Convert(move))&&TTT_CheckMove(1,1,Convert(move))&&TTT_CheckMove(2,2,Convert(move))){
		//to do diagonal check
		//GLCD_DisplayVerticalString(0,0,"Diagonal!");
		GLCD_DisplayDiagonalLine(lattice0.pos[0][0].p.x- CHAR_W, lattice0.pos[0][0].p.y - CHAR_H/2, lattice0.verticalDisp *2 + CHAR_H/2 + CHAR_H ,Yellow, 5);
		//GLCD_DisplayDiagonalLine(72,35,165,Yellow, 5);
		//_wait_delay(100000000);
		GLCD_SetTextColor(Yellow);
		GLCD_DisplayString(c.x + CHAR_W,c.y,"YOU WIN!");
		NVIC_DisableIRQ(EINT3_IRQn);
		_wait_delay(72000000);
		GLCD_DisplayString(lattice0.start.x+5,c.y + lattice0.verticalDisp,"Press RESET!");
		victory++;
	}
	// Antidiagonal check
	if(TTT_CheckMove(2,0,Convert(move))&&TTT_CheckMove(1,1,Convert(move))&&TTT_CheckMove(0,2,Convert(move))){
		//to do antidiagonal check
		//GLCD_DisplayVerticalString(0,0,"Antidiagonal!");
		GLCD_DisplayAntidiagonalLine(lattice0.pos[0][2].p.x + CHAR_W/2 + CHAR_W, lattice0.pos[0][2].p.y - CHAR_H/2, lattice0.verticalDisp *2 + CHAR_H/2 + CHAR_H ,Yellow, 5);
		
		GLCD_SetTextColor(Yellow);
		GLCD_DisplayString(c.x + CHAR_W,c.y,"YOU WIN!");
		NVIC_DisableIRQ(EINT3_IRQn);
		_wait_delay(72000000);
		GLCD_DisplayString(lattice0.start.x+5,c.y + lattice0.verticalDisp,"Press RESET!");
		victory++;
	}
	
	if(nmoves == NCELLS && victory == 0){
		GLCD_SetTextColor(Yellow);
		GLCD_DisplayString(lattice0.start.x+5,c.y,"IT'S A DRAW!");
		NVIC_DisableIRQ(EINT3_IRQn);
		_wait_delay(72000000);
		GLCD_DisplayString(lattice0.start.x+5,c.y + lattice0.verticalDisp,"Press RESET!");
	}
		
}
/* Make your move and insert into lattice */
void TTT_YourMove(int x, int y){
	 Map_Into_Lattice(&x,&y);
	 TTT_DisplayMove(x, y, Red,  player);
}
/* Assign the correct cell of the TTT lattice and check for offset validity */
void Map_Into_Lattice(int *x, int *y){
		point posizioni[NCELLS];
		int distanze[NCELLS],i,min;
	  
		if((*x) < lattice0.start.x || ((*x)> lattice0.start.x +lattice0.width) || ((*y) < lattice0.start.y) || ((*y) > (lattice0.start.y+lattice0.height))){
			return;
		}
	
	
	
		posizioni[0]=computePos(1,1);
		posizioni[0].x+=CHAR_W/2;
		posizioni[0].y+=CHAR_H/2;
	
		posizioni[1]=computePos(1,2);
		posizioni[1].x+=CHAR_W/2;
		posizioni[1].y+=CHAR_H/2;
	
		posizioni[2]=computePos(1,3);
		posizioni[2].x+=CHAR_W/2;
		posizioni[2].y+=CHAR_H/2;	
	
		posizioni[3]=computePos(2,1);
		posizioni[3].x+=CHAR_W/2;
		posizioni[3].y+=CHAR_H/2;	
	
		posizioni[4]=computePos(2,2);
		posizioni[4].x+=CHAR_W/2;
		posizioni[4].y+=CHAR_H/2;
		
		posizioni[5]=computePos(2,3);
		posizioni[5].x+=CHAR_W/2;
		posizioni[5].y+=CHAR_H/2;
		
		posizioni[6]=computePos(3,1);
		posizioni[6].x+=CHAR_W/2;
		posizioni[6].y+=CHAR_H/2;
		
		posizioni[7]=computePos(3,2);
		posizioni[7].x+=CHAR_W/2;
		posizioni[7].y+=CHAR_H/2;
		
		posizioni[8]=computePos(3,3);
		posizioni[8].x+=CHAR_W/2;
		posizioni[8].y+=CHAR_H/2;
		
		for(i=0;i<NCELLS;i++)
			distanze[i] = (*x - posizioni[i].x) * (*x - posizioni[i].x) + (*y - posizioni[i].y) * (*y - posizioni[i].y);  
		
		min=distanze[0];
		
		*x=0;
		*y=0;
		
		for(i=1;i<NCELLS;i++){
			if(distanze[i]<min){
				min=distanze[i];
				*x = i / 3;
				*y = i % 3;
			}
		}
}
