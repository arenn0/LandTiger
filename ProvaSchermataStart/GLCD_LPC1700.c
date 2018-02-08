#include "lpc17xx.h"
#include "GLCD.h"
#include "Font_24x16.h"

/*********************** Hardware specific configuration **********************/

/* 8bit to 16bit LCD Interface 
   
   PINS: 
   - EN      = P0.19
   - LE      = P0.20
   - DIR     = P0.21
   - CS      = P0.22
   - RS      = P0.23 
   - WR      = P0.24 
   - RD      = P0.25
   - DB[0.7] = P2.0...P2.7
   - DB[8.15]= P2.0...P2.7                                                     */

#define PIN_EN		(1 << 19)
#define PIN_LE		(1 << 20)
#define PIN_DIR		(1 << 21)
#define PIN_CS      (1 << 22)
#define PIN_RS		(1 << 23)
#define PIN_WR		(1 << 24)
#define PIN_RD		(1 << 25)   

/*------------------------- Speed dependant settings -------------------------*/

/* If processor works on high frequency delay has to be increased, it can be 
   increased by factor 2^N by this constant                                   */
#define DELAY_2N    18

/*---------------------- Graphic LCD size definitions ------------------------*/

#define WIDTH       320                 /* Screen Width (in pixels)           */
#define HEIGHT      240                 /* Screen Hight (in pixels)           */
#define BPP         16                  /* Bits per pixel                     */
#define BYPP        ((BPP+7)/8)         /* Bytes per pixel                    */

/*--------------- Graphic LCD interface hardware definitions -----------------*/

/* Pin EN setting to 0 or 1                                                   */
#define LCD_EN(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_EN) : (LPC_GPIO0->FIOCLR = PIN_EN));
/* Pin LE setting to 0 or 1                                                   */
#define LCD_LE(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_LE) : (LPC_GPIO0->FIOCLR = PIN_LE));
/* Pin DIR setting to 0 or 1                                                   */
#define LCD_DIR(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_DIR) : (LPC_GPIO0->FIOCLR = PIN_DIR));
/* Pin CS setting to 0 or 1                                                   */
#define LCD_CS(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_CS) : (LPC_GPIO0->FIOCLR = PIN_CS));
/* Pin RS setting to 0 or 1                                                   */
#define LCD_RS(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_RS) : (LPC_GPIO0->FIOCLR = PIN_RS));
/* Pin WR setting to 0 or 1                                                   */
#define LCD_WR(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_WR) : (LPC_GPIO0->FIOCLR = PIN_WR));
/* Pin RD setting to 0 or 1                                                   */
#define LCD_RD(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_RD) : (LPC_GPIO0->FIOCLR = PIN_RD));
 
/*---------------------------- Global variables ------------------------------*/

/******************************************************************************/
static volatile unsigned short TextColor = Black, BackColor = White;


/************************ Local auxiliary functions ***************************/

/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/
void delay (int cnt) {

  cnt <<= DELAY_2N;
  while (cnt--);
}

__asm void wait()
{
    nop
    BX lr
}

void wait_delay(int count)
{
  while(count--);
}

/*******************************************************************************
* Send 1 byte over serial communication                                        *
*   Parameter:    byte:   byte to be sent                                      *
*   Return:                                                                    *
*******************************************************************************/

static __inline unsigned char lcd_send (unsigned short byte) {

  LPC_GPIO2->FIODIR |= 0x000000ff;  //P2.0...P2.7 Output
  LCD_DIR(1)		   				//Interface A->B
  LCD_EN(0)	                        //Enable 2A->2B
  LPC_GPIO2->FIOPIN =  byte;        //Write D0..D7
  LCD_LE(1)                         
  LCD_LE(0)							//latch D0..D7
  LPC_GPIO2->FIOPIN =  byte >> 8;   //Write D8..D15 
  return(1);
}


/*******************************************************************************
* Write command to LCD controller                                              *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_cmd (unsigned char c) {

  LCD_RS(0)
  LCD_RD(1)
  lcd_send(c);
  LCD_WR(0)
  wait();
  LCD_WR(1)
}


/*******************************************************************************
* Write data to LCD controller                                                 *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat (unsigned short c) {

  LCD_RS(1)
  LCD_RD(1)
  lcd_send(c);
  LCD_WR(0)
  wait();
  LCD_WR(1)
}

/*******************************************************************************
* Start of data writing to LCD controller                                      *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_start (void) {

  LCD_RS(1)
}


/*******************************************************************************
* Stop of data writing to LCD controller                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_stop (void) {

  LCD_CS(1)
}


/*******************************************************************************
* Data writing to LCD controller                                               *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_only (unsigned short c) {

  lcd_send(c);
  LCD_WR(0)
  wait();
  LCD_WR(1)
}


/*******************************************************************************
* Write to LCD register                                                        *
*   Parameter:    reg:    register to be read                                  *
*                 val:    value to write to register                           *
*******************************************************************************/

static __inline void wr_reg (unsigned char reg, unsigned short val) {

  LCD_CS(0)
  wr_cmd(reg);
  wr_dat(val);
  LCD_CS(1)
}


/*******************************************************************************
* Initialize the Graphic LCD controller                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Init (void) { 

  /* Configure the LCD Control pins                                           */
  LPC_GPIO0->FIODIR   |= 0x03f80000;
  LPC_GPIO0->FIOSET    = 0x03f80000;	

  delay(5);                             /* Delay 50 ms                        */
	//driverCode = 0x9325
  wr_reg(0x00e7,0x0010);      
  wr_reg(0x0000,0x0001);  			//start internal osc
  wr_reg(0x0001,0x0100);     
  wr_reg(0x0002,0x0700); 				//power on sequence                     
  wr_reg(0x0003,(1<<12)|(1<<5)|(1<<4) ); 	//65K 
  wr_reg(0x0004,0x0000);                                   
  wr_reg(0x0008,0x0207);	           
  wr_reg(0x0009,0x0000);         
  wr_reg(0x000a,0x0000); 				//display setting         
  wr_reg(0x000c,0x0001);				//display setting          
  wr_reg(0x000d,0x0000); 				//0f3c          
  wr_reg(0x000f,0x0000);
	//Power On sequence //
  wr_reg(0x0010,0x0000);   
  wr_reg(0x0011,0x0007);
  wr_reg(0x0012,0x0000);                                                                 
  wr_reg(0x0013,0x0000);                 
  delay(15);
  wr_reg(0x0010,0x1590);   
  wr_reg(0x0011,0x0227);
  delay(15);
  wr_reg(0x0012,0x009c);                  
  delay(15);
  wr_reg(0x0013,0x1900);   
  wr_reg(0x0029,0x0023);
  wr_reg(0x002b,0x000e);
  delay(15);
  wr_reg(0x0020,0x0000);                                                            
  wr_reg(0x0021,0x0000);           


  delay(15);
  wr_reg(0x0030,0x0007); 
  wr_reg(0x0031,0x0707);   
  wr_reg(0x0032,0x0006);
  wr_reg(0x0035,0x0704);
  wr_reg(0x0036,0x1f04); 
  wr_reg(0x0037,0x0004);
  wr_reg(0x0038,0x0000);        
  wr_reg(0x0039,0x0706);     
  wr_reg(0x003c,0x0701);
  wr_reg(0x003d,0x000f);
  delay(15);
  wr_reg(0x0050,0x0000);        
  wr_reg(0x0051,0x00ef);   
  wr_reg(0x0052,0x0000);     
  wr_reg(0x0053,0x013f);
  wr_reg(0x0060,0xa700);        
  wr_reg(0x0061,0x0001); 
  wr_reg(0x006a,0x0000);
  wr_reg(0x0080,0x0000);
  wr_reg(0x0081,0x0000);
  wr_reg(0x0082,0x0000);
  wr_reg(0x0083,0x0000);
  wr_reg(0x0084,0x0000);
  wr_reg(0x0085,0x0000);
     
  wr_reg(0x0090,0x0010);     
  wr_reg(0x0092,0x0000);  
  wr_reg(0x0093,0x0003);
  wr_reg(0x0095,0x0110);
  wr_reg(0x0097,0x0000);        
  wr_reg(0x0098,0x0000);  
  //display on sequence     
  wr_reg(0x0007,0x0133);
    
  wr_reg(0x0020,0x0000);                                                            
  wr_reg(0x0021,0x0000);
  /* Set GRAM write direction and BGR = 1
     I/D=10 (Horizontal : increment, Vertical : increment)
     AM=1 (address is updated in vertical writing direction)                  */
  wr_reg(0x03, 0x1038);

  wr_reg(0x07, 0x0173);                 /* 262K color and display ON          */ 
}


/*******************************************************************************
* Set draw window region to whole screen                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_WindowMax (void) {
	
  	wr_reg(0x50, 0);                      /* Horizontal GRAM Start Address      */
  	wr_reg(0x51, HEIGHT-1);               /* Horizontal GRAM End   Address (-1) */
  	wr_reg(0x52, 0);                      /* Vertical   GRAM Start Address      */
  	wr_reg(0x53, WIDTH-1);                /* Vertical   GRAM End   Address (-1) */

}


/*******************************************************************************
* Draw a pixel in foreground color                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_PutPixel (unsigned int x, unsigned int y) {
  
  GLCD_WindowMax();

  wr_reg(0x20, y);
  wr_reg(0x21, WIDTH-1-x);
  
  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat(TextColor);
  LCD_CS(1)
}


/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:      color:    foreground color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetTextColor (unsigned short color) {

  TextColor = color;
}


/*******************************************************************************
* Set background color                                                         *
*   Parameter:      color:    background color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetBackColor (unsigned short color) {

  BackColor = color;
}


/*******************************************************************************
* Clear display                                                                *
*   Parameter:      color:    display clearing color                           *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Clear (unsigned short color) {
  unsigned int   i;

  GLCD_WindowMax();

  wr_reg(0x20, 0);
  wr_reg(0x21, 0);

  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat_start();
  for(i = 0; i < (WIDTH*HEIGHT); i++)
    wr_dat_only(color);
  wr_dat_stop();
}


/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DrawChar (unsigned int x, unsigned int y, unsigned short *c) {
  int idx = 0, i, j;

  x = WIDTH-x-CHAR_W;

  
  wr_reg(0x50, y);                      /* Horizontal GRAM Start Address      */
  wr_reg(0x51, y+CHAR_H-1);             /* Horizontal GRAM End   Address (-1) */
  wr_reg(0x52, x);                      /* Vertical   GRAM Start Address      */
  wr_reg(0x53, x+CHAR_W-1);             /* Vertical   GRAM End   Address (-1) */

	wr_reg(0x20, y);
  wr_reg(0x21, x);
	
  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat_start();
  for (j = 0; j < CHAR_H; j++) {
    for (i = CHAR_W-1; i >= 0; i--) {
      if((c[idx] & (1 << i)) == 0x00) {
        wr_dat_only(BackColor);
      } else {
        wr_dat_only(TextColor);
      }
    }
    c++;
  }
  wr_dat_stop();
}


/*******************************************************************************
* Disply character on given line                                               *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DisplayChar (unsigned int x, unsigned int y, unsigned char c) {

  c -= 32;
  GLCD_DrawChar(x, y, (unsigned short *)&Font_24x16[c * CHAR_H]);
}


/*******************************************************************************
* Disply string on given column                                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   s:        pointer to string                                *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DisplayVerticalString (unsigned int x, unsigned int y, unsigned char *s) {
  y-=CHAR_H;
  GLCD_WindowMax();
  while (*s) {
    GLCD_DisplayChar(x, y+=CHAR_H,*s++);
  }
}


/*******************************************************************************
* Disply string on given line                                                  *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DisplayString (unsigned int x, unsigned int y, unsigned char *s) {
  x-=CHAR_W;
  GLCD_WindowMax();
  while (*s) {
    GLCD_DisplayChar(x+=CHAR_W,y, *s++);
  }
}


/*******************************************************************************
* Display Line. It is supposed to draw a line. Could degenerate into a point.  *
* For simmetry reasons, it is supposed that a odd number is used as thickness. *
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
void GLCD_DisplayLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color, unsigned int thickness){

  unsigned int i,j;
  GLCD_SetTextColor(color);
  
  if(x0 == x1){ /*   Vertical Line    */
    for(i=x0-((thickness-1)/2); i<x0+((thickness-1)/2); i++)
      for(j=y0; j<=y1; j++)
        GLCD_PutPixel(i,j);
  }
  if(y0 == y1){ /*   Horizontal Line   */
    for(i=x0; i<=x1; i++)
      for(j=y0-((thickness-1)/2); j<y0+((thickness-1)/2); j++)
        GLCD_PutPixel(i,j);
  }

}




/*******************************************************************************
* Display Diagonal Line. It is supposed to draw a diagonal line. Could 		   *
* degenerate into a point. For simmetry reasons, it is supposed that a odd 	   *
* number is used as thickness.                                                 *
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
void GLCD_DisplayDiagonalLine(unsigned int x, unsigned int y, unsigned int height, unsigned int color, unsigned int thickness){
  unsigned int i,j;
  GLCD_SetTextColor(color);
  
 
  for(j=y; j<=y+height; j++)
    for(i=x+j-y-((thickness-1)/2); i<=x+j-y+((thickness-1)/2); i++)
      GLCD_PutPixel(i,j);
 
}


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
