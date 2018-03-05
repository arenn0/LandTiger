Name: TouchPanel.c
Purpose: Functions to init and read values from touch panel 
Note(s): This work is licensed under the Creative Commons Attribution 3.0 Italy License. 
				 To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/it/ 
				 or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
--------------------------------------------------------------------------------------------*/
#include "TouchPanel.h"
#include "GLCD.h"
#include "GLCD_TTT.h"

#define NRead 20 // Number of inputs read for every EINT3 interrupt (triggered by each touch)

#define CONVX 10.69411765 // Rescaling factor to map the coordinate x 
#define CONVY 14.91596639 // Rescaling factor to map the coordinate y

//Screen border points: {{top left corner}, {top right corner}, {bottom left corner}, {bottom right corner}}
//These values refer to margin coordinates read before rescaling.

Coordinate DisplayMargins[4] =   { {3750,250}, { 250, 250}, {3750,3800}, {250,3800}} ;

/*******************************************************************************
* Function Name  : LPC17xx_SPI_SetSpeed
* Description    : Set clock speed to desired value.
*		   The used pins refer to SSP controller to communicate with SPI 
* 		   for backward compatibility.
* Input          : - speed: speed
* Output         : None
* Return         : None
*******************************************************************************/
void LPC17xx_SPI_SetSpeed (uint8_t speed)
{
	speed &= 0xFE;
	if ( speed < 2  ) {
		speed = 2 ;   //min possible value
	}
	LPC_SSP1->CPSR = speed;
}

/*******************************************************************************
* Function Name  : ADS7843_SPI_Init
* Description    : ADS7843 SPI initialization.
* 		   The used pins refer to SSP controller to communicate with SPI 
* 		   for backward compatibility.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void ADS7843_SPI_Init(void) 
{ 
	volatile uint32_t dummy;

	/* Initialize and enable the SSP1 Interface module. */
	LPC_SC->PCONP |= (1 << 10);          /* Enable power to SSPI1 block  */

	/* P0.7 SCK, P0.8 MISO, P0.9 MOSI are SSP pins. */
	LPC_PINCON->PINSEL0 &= ~((3UL<<14) | (3UL<<16) | (3UL<<18)) ; /* P0.7,P0.8,P0.9 cleared */
	LPC_PINCON->PINSEL0 |=  (2UL<<14) | (2UL<<16) | (2UL<<18);    /* P0.7 SCK1,P0.8 MISO1,P0.9 MOSI1 */

	/* PCLK_SSP1=CCLK */
	LPC_SC->PCLKSEL0 &= ~(3<<20);               /* PCLKSP0 = CCLK/4 (18MHz) */
	LPC_SC->PCLKSEL0 |=  (1<<20);               /* PCLKSP0 = CCLK   (72MHz) */

	LPC_SSP1->CR0  = 0x0007;                    /* 8Bit, CPOL=0, CPHA=0         */
	LPC_SSP1->CR1  = 0x0002;                    /* SSP1 enable, master          */

	LPC17xx_SPI_SetSpeed ( SPI_SPEED_400kHz );   //Default speed setting 

	/* wait for busy gone */
	while( LPC_SSP1->SR & ( 1 << SSPSR_BSY ) );

	/* drain SPI RX FIFO */
	while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )
	{
		dummy = LPC_SSP1->DR;
	}
} 

/*******************************************************************************
* Function Name  : TP_Init
* Description    : ADS7843 TP initialization
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

// P0.6 --> TP_CS
// P0.7 --> SCK1
// P0.8/P0.9 --> MISO1/MOSI1

void TP_Init(void) 
{ 
	LPC_PINCON->PINSEL4    |= (1 << 26); // P2.13 as EINT3 (touch panel interrupt) 
	
  LPC_GPIO0->FIODIR |=  (1<<6);   /* P0.6 CS is output */
  LPC_GPIO2->FIODIR |=  (0<<13);  /* P2.13 TP_INT is input (touch screen) */
  TP_CS(1); 
  ADS7843_SPI_Init(); 
	
  LPC_SC->EXTMODE += 0x8;			// Set EINT3 in the external interrupt mode register
	
	NVIC_EnableIRQ(EINT3_IRQn); // Enable EINT3 in the Nested Vector Interrupt Controller

} 

/*******************************************************************************
* Function Name  : DelayUS
* Description    : Delay, roughly cnt * 1 millisecond
* Input          : - cnt : delay
* Output         : None
* Return         : None
*******************************************************************************/
static void DelayUS(uint16_t cnt)
{
  uint16_t i;
  for(i = 0;i<cnt;i++)
  {
     uint16_t us = 100; 
     while (us--)     
     {
       ;   
     }
  }
}


/*******************************************************************************
* Function Name  : WR_CMD
* Description    : ADS7843D write command.
* 		   The used pins refer to SSP controller to communicate with SPI 
* 		   for backward compatibility. 
* Input          : - cmd: command
* Output         : None
* Return         : Received value
*******************************************************************************/
static uint8_t WR_CMD (uint8_t cmd)  
{ 
  uint8_t byte_r;

  while (LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	     /* Wait for transfer to finish */
  LPC_SSP1->DR = cmd;
  while (LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	     /* Wait for transfer to finish */
  while( !( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) ) );	 /* Wait until the Rx FIFO is not empty */
  byte_r = LPC_SSP1->DR;

  return byte_r;                                     /* Return received value */
} 



/*******************************************************************************
* Function Name  : RD_AD
* Description    : Read from SPI 
* Input          : None
* Output         : None
* Return         : ADS7843 value
*******************************************************************************/
static int RD_AD(void)  
{ 
  unsigned short buf,temp; 

  temp = WR_CMD(0x00);   //NOP
  buf = temp<<8; 
  DelayUS(5); 
  temp = WR_CMD(0x00);   //NOP 
  buf |= temp; 
  buf>>=3; 
  buf&=0xfff; 
  return buf; 
} 


/*******************************************************************************
* Function Name  : Read_X
* Description    : Read x coordinate 
* Input          : None
* Output         : None
* Return         : ADS7843 x
*******************************************************************************/
int Read_X(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(5); 
  WR_CMD(CHX); 
  DelayUS(5); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;    
} 

/*******************************************************************************
* Function Name  : Read_Y
* Description    : Read y coordinate
* Input          : None
* Output         : None
* Return         : ADS7843 y
*******************************************************************************/
int Read_Y(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(5); 
  WR_CMD(CHY); 
  DelayUS(5); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;     
} 

/*******************************************************************************
* Function Name  : TP_GetAdXY
* Description    : Higher Level function that reads x and y
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TP_GetAdXY(int *x,int *y)  
{ 
  int adx,ady; 
  adx=Read_X(); 
  DelayUS(1); 
  ady=Read_Y(); 
  *x=adx; 
  *y=ady; 
} 

/*******************************************************************************
* Function Name  : Read_Ads7846
* Description    : Clean and Average of the read values
* Input          : None
* Output         : None
* Return         : Coordinate
*******************************************************************************/

Coordinate * Read_Ads7846(void)
{
  static Coordinate  screen;
  int i,sumx=0,sumy=0,counterx=0,countery=0;
  int TP_X[1],TP_Y[1];    // X and Y coordinates

  uint8_t count=0;
  int buffer[2][NRead]={{0},{0}};  
  do					       
  {		   
    TP_GetAdXY(TP_X,TP_Y);  
	  buffer[0][count]=TP_X[0];  
	  buffer[1][count]=TP_Y[0];
	  count++; 
		delay(1);
  }
  while(!TP_INT_IN && count<NRead);  
  screen.x=0;  // flag wrong data read
  screen.y=0;  // flag wrong data read


	for (i=0;i< count;i++){
		// Sum over x
		if(!(buffer[0][i]>3750 || buffer[0][i]<250)){ 
			  // X coordinate in display margins
			  sumx+=buffer[0][i];  
			  counterx++;
		}
		// Sum over y
		if(!(buffer[1][i]>3750 || buffer[1][i]<250)){
		          // Y coordinate in display margins
			  sumy+=buffer[1][i];
			  countery++;
		}	
	}
	if(counterx!=0 && countery!=0){
		// Average
		screen.x=sumx/counterx;
		screen.y=sumy/countery;
	}
	
	Map_Pixel(&screen);   
  if(screen.x > 320 ||screen.y > 240){
	  // flag wrong data 
 	  screen.x=0;
	  screen.y=0;
	}
	return &screen;

}

/*******************************************************************************
* Function Name  : Map_Pixel
* Description    : Map and rescale values into 320x240 values
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Map_Pixel(Coordinate *touch){
	
	touch->x = 320 - (touch->x - DisplayMargins[1].x )/CONVX;   // Read value of X decreases from left to right
	touch->y = (touch->y - DisplayMargins[0].y)/CONVY + 1;   // Read value of Y increases from top to bottom, however the screen works in the same way
	return;
}
