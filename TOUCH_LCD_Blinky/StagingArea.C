
/*******************************************************************************
* Display Antidiagonal Line. It is supposed to draw an antidiagonal line.      *
* Could degenerate into a point. For simmetry reasons, it is supposed that a   *
* odd number is used as thickness.                                             *
* (This function is optimized for 16 bits per pixel format, it has to be       *
*  adapted for any other bits per pixel format)                                *
*   Parameter:      x0:         horizontal starting position                   *
*                   y0:         vertical starting position                     *
*                   x1:         horizontal ending position                     *
*                   y1:         vertical ending position                       *
*                   color:      color of line                                  *
*                   thickness:  thickness of line                              *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_DisplayAntidiagonalLine(unsigned int x, unsigned int y, unsigned int height, unsigned int color, unsigned int thickness){
  unsigned int i,j;
  GLCD_SetTextColor(color);
  
 
  for(j=y; j<=y+height; j++)
    for(i=x-j+y-((thickness-1)/2); i<=x-j+y+((thickness-1)/2); i++)
      GLCD_PutPixel(i,j);
 
}