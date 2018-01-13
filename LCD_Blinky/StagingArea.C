
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

  GLCD_DisplayLine(x0 + HorizontalDisp, y0 + 3, x0 + HorizontalDisp, y0 + height -3, color, thickness); /*      1 Vertical Line       */
  GLCD_DisplayLine(x0 + 2*HorizontalDisp, y0 + 3, x0 + 2*HorizontalDisp, y0 + height -3, color, thickness); /*      2 Vertical Line       */

  GLCD_DisplayLine(x0 + 3, y0 + VerticalDisp, x0 + width - 3, y0 + VerticalDisp, color, thickness); /*      1 Horizontal Line       */
  GLCD_DisplayLine(x0 + 3, y0 + 2*VerticalDisp, x0 + width - 3, y0 + 2*VerticalDisp, color, thickness); /*      2 Horizontal Line       */

  GLCD_DisplayLine(x0 , y0 , x0, y0 + height, color, thickness); /*      1 Lattice Vertical Line       */
  GLCD_DisplayLine(x0 + width , y0 , x0 + width, y0 + height, color, thickness); /*      2 Vertical Line       */

  GLCD_DisplayLine(x0 , y0, x0 + width, y0, color, thickness); /*      1 Horizontal Line       */
  GLCD_DisplayLine(x0 , y0 + height, x0 + width, y0 + height, color, thickness); /*      2 Horizontal Line       */

 
 
}
