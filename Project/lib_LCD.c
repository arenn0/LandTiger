/* Includes ------------------------------------------------------------------*/
#include "LCD.h" 
#include "AsciiLib.h"


#define  SSD1289    5  /* 0x8989 */


/*******************************************************************************
* Function Name  : Lcd_Configuration
* Description    : Configures LCD Control lines
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Configuration(void)
{
	/* Configure the LCD Control pins */
	
	/* EN = P0.19 , LE = P0.20 , DIR = P0.21 , CS = P0.22 , RS = P0.23 , RS = P0.23 */
	/* RS = P0.23 , WR = P0.24 , RD = P0.25 , DB[0.7] = P2.0...P2.7 , DB[8.15]= P2.0...P2.7 */  
	LPC_GPIO0->FIODIR   |= 0x03f80000;
	LPC_GPIO0->FIOSET    = 0x03f80000;
}

/*******************************************************************************
* Function Name  : LCD_Send
* Description    : LCD???
* Input          : - byte: byte to be sent
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_Send (uint16_t byte) 
{
	LPC_GPIO2->FIODIR |= 0xFF;          /* P2.0...P2.7 Output */
	LCD_DIR(1)		   				    /* Interface A->B */
	LCD_EN(0)	                        /* Enable 2A->2B */
	LPC_GPIO2->FIOPIN =  byte;          /* Write D0..D7 */
	LCD_LE(1)                         
	LCD_LE(0)							/* latch D0..D7	*/
	LPC_GPIO2->FIOPIN =  byte >> 8;     /* Write D8..D15 */
}

/*******************************************************************************
* Function Name  : wait_delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void wait_delay(int count)
{
	while(count--);
}

/*******************************************************************************
* Function Name  : LCD_Read
* Description    : LCD???
* Input          : - byte: byte to be read
* Output         : None
* Return         : ????????
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_Read (void) 
{
	uint16_t value;
	
	LPC_GPIO2->FIODIR &= ~(0xFF);              /* P2.0...P2.7 Input */
	LCD_DIR(0);		   				           /* Interface B->A */
	LCD_EN(0);	                               /* Enable 2B->2A */
	wait_delay(30);							   /* delay some times */
	value = LPC_GPIO2->FIOPIN0;                /* Read D8..D15 */
	LCD_EN(1);	                               /* Enable 1B->1A */
	wait_delay(30);							   /* delay some times */
	value = (value << 8) | LPC_GPIO2->FIOPIN0; /* Read D0..D7 */
	LCD_DIR(1);
	return  value;
}

/*******************************************************************************
* Function Name  : LCD_WriteIndex
* Description    : LCD??????
* Input          : - index: ?????
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteIndex(uint16_t index)
{
	LCD_CS(0);
	LCD_RS(0);
	LCD_RD(1);
	LCD_Send( index ); 
	wait_delay(25);	
	LCD_WR(0);  
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_WriteData
* Description    : LCD??????
* Input          : - index: ?????
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteData(uint16_t data)
{				
	LCD_CS(0);
	LCD_RS(1);   
	LCD_Send( data );
	LCD_WR(0);     
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    : ???????
* Input          : None
* Output         : None
* Return         : ????????
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadData(void)
{ 
	uint16_t value;
	
	LCD_CS(0);
	LCD_RS(1);
	LCD_WR(1);
	LCD_RD(0);
	value = LCD_Read();
	
	LCD_RD(1);
	LCD_CS(1);
	
	return value;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{ 
	/* Write 16-bit Index, then Write Reg */  
	LCD_WriteIndex(LCD_Reg);         
	/* Write 16-bit Reg */
	LCD_WriteData(LCD_RegValue);  
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	uint16_t LCD_RAM;
	
	/* Write 16-bit Index (then Read Reg) */
	LCD_WriteIndex(LCD_Reg);
	/* Read 16-bit Reg */
	LCD_RAM = LCD_ReadData();      	
	return LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
	
 	uint16_t temp = Xpos;

			 Xpos = Ypos;
			 Ypos = ( MAX_X - 1 ) - temp;  

	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
		
	#endif


	LCD_WriteReg(0x004e, Xpos );      
    LCD_WriteReg(0x004f, Ypos );    
	      
}

/*******************************************************************************
* Function Name  : LCD_Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
} 


/*******************************************************************************
* Function Name  : LCD_Initializtion
* Description    : Initialize TFT Controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Initialization(void)
{
	
	LCD_Configuration();
	delay_ms(100);

    LCD_WriteReg(0x0000,0x0001);    delay_ms(50);   /* ???? */
    LCD_WriteReg(0x0003,0xA8A4);    delay_ms(50);   
    LCD_WriteReg(0x000C,0x0000);    delay_ms(50);   
    LCD_WriteReg(0x000D,0x080C);    delay_ms(50);   
    LCD_WriteReg(0x000E,0x2B00);    delay_ms(50);   
    LCD_WriteReg(0x001E,0x00B0);    delay_ms(50);   
    LCD_WriteReg(0x0001,0x2B3F);    delay_ms(50);   /* ??????320*240 0x2B3F */
    LCD_WriteReg(0x0002,0x0600);    delay_ms(50);
    LCD_WriteReg(0x0010,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0011,0x6070);    delay_ms(50);   /* ?????? 16?? ?? 0x6070 */
    LCD_WriteReg(0x0005,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0006,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0016,0xEF1C);    delay_ms(50);
    LCD_WriteReg(0x0017,0x0003);    delay_ms(50);
    LCD_WriteReg(0x0007,0x0133);    delay_ms(50);         
    LCD_WriteReg(0x000B,0x0000);    delay_ms(50);
    LCD_WriteReg(0x000F,0x0000);    delay_ms(50);   /* ?????? */
    LCD_WriteReg(0x0041,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0042,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0048,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0049,0x013F);    delay_ms(50);
    LCD_WriteReg(0x004A,0x0000);    delay_ms(50);
    LCD_WriteReg(0x004B,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0044,0xEF00);    delay_ms(50);
    LCD_WriteReg(0x0045,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0046,0x013F);    delay_ms(50);
    LCD_WriteReg(0x0030,0x0707);    delay_ms(50);
    LCD_WriteReg(0x0031,0x0204);    delay_ms(50);
    LCD_WriteReg(0x0032,0x0204);    delay_ms(50);
    LCD_WriteReg(0x0033,0x0502);    delay_ms(50);
    LCD_WriteReg(0x0034,0x0507);    delay_ms(50);
    LCD_WriteReg(0x0035,0x0204);    delay_ms(50);
    LCD_WriteReg(0x0036,0x0204);    delay_ms(50);
    LCD_WriteReg(0x0037,0x0502);    delay_ms(50);
    LCD_WriteReg(0x003A,0x0302);    delay_ms(50);
    LCD_WriteReg(0x003B,0x0302);    delay_ms(50);
    LCD_WriteReg(0x0023,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0024,0x0000);    delay_ms(50);
    LCD_WriteReg(0x0025,0x8000);    delay_ms(50);
    LCD_WriteReg(0x004f,0);        /* ???0 */
	LCD_WriteReg(0x004e,0);        /* ???0 */
						
    delay_ms(50);   /* delay 50 ms */	
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : ???????????,???,??? 0xffff
* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index;	
		
	LCD_SetCursor(0,0); 	

	LCD_WriteIndex(0x0022);
	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
		LCD_WriteData(Color);
	}
}

/******************************************************************************
* Function Name  : LCD_GetPoint
* Description    : ??????????
* Input          : - Xpos: Row Coordinate
*                  - Xpos: Line Coordinate 
* Output         : None
* Return         : Screen Color
* Attention		 : None
*******************************************************************************/
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
	uint16_t dummy;
	
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteIndex(0x0022);  
	
	
    dummy = LCD_ReadData();   /* Empty read */
	dummy = LCD_ReadData(); 
		
 	return  dummy;	      
	    
}

/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : ???????
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	if( Xpos >= MAX_X || Ypos >= MAX_Y )
	{
		return;
	}
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteReg(0x0022,point);
}

/******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Bresenham's line algorithm
* Input          : - x1: A????
*                  - y1: A???? 
*				   - x2: B????
*				   - y2: B???? 
*				   - color: ???
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 
void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
    short dx,dy;      /* ??X Y???????? */
    short temp;       /* ?? ?????? ?????????? */

    if( x0 > x1 )     /* X???????? ???? */
    {
	    temp = x1;
		x1 = x0;
		x0 = temp;   
    }
    if( y0 > y1 )     /* Y???????? ???? */
    {
		temp = y1;
		y1 = y0;
		y0 = temp;   
    }
  
	dx = x1-x0;       /* X??????? */
	dy = y1-y0;       /* Y??????? */

    if( dx == 0 )     /* X?????? ???? */ 
    {
        do
        { 
            LCD_SetPoint(x0, y0, color);   /* ???? ???? */
            y0++;
        }
        while( y1 >= y0 ); 
		return; 
    }
    if( dy == 0 )     /* Y?????? ????? */ 
    {
        do
        {
            LCD_SetPoint(x0, y0, color);   /* ???? ???? */
            x0++;
        }
        while( x1 >= x0 ); 
		return;
    }
	/* ?????(Bresenham)???? */
    if( dx > dy )                         /* ??X? */
    {
	    temp = 2 * dy - dx;               /* ???????? */         
        while( x0 != x1 )
        {
	        LCD_SetPoint(x0,y0,color);    /* ??? */ 
	        x0++;                         /* X???1 */
	        if( temp > 0 )                /* ????????? */
	        {
	            y0++;                     /* ??????,?(x0+1,y0+1) */ 
	            temp += 2 * dy - 2 * dx; 
	 	    }
            else         
            {
			    temp += 2 * dy;           /* ????????? */  
			}       
        }
        LCD_SetPoint(x0,y0,color);
    }  
    else
    {
	    temp = 2 * dx - dy;                      /* ??Y? */       
        while( y0 != y1 )
        {
	 	    LCD_SetPoint(x0,y0,color);     
            y0++;                 
            if( temp > 0 )           
            {
                x0++;               
                temp+=2*dy-2*dx; 
            }
            else
			{
                temp += 2 * dy;
			}
        } 
        LCD_SetPoint(x0,y0,color);
	}
} 

/******************************************************************************
* Function Name  : PutChar
* Description    : ?Lcd????????????
* Input          : - Xpos: ???? 
*                  - Ypos: ????  
*				   - ASCI: ?????
*				   - charColor: ????   
*				   - bkColor: ???? 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor )
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);  /* ????? */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* ???? */
            }
            else
            {
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );  /* ???? */
            }
        }
    }
}

/******************************************************************************
* Function Name  : GUI_Text
* Description    : ??????????
* Input          : - Xpos: ???
*                  - Ypos: ??? 
*				   - str: ???
*				   - charColor: ????   
*				   - bkColor: ???? 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void GUI_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor)
{
    uint8_t TempChar;
    do
    {
        TempChar = *str++;  
        PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        }    
    }
    while ( *str != 0 );
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
