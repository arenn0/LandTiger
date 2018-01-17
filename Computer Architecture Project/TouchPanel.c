/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               TouchPanel.c
** Descriptions:            The TouchPanel application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#include "TouchPanel.h"
#include "GLCD.h"
#include "GLCD_TTT.h"


#define CONVX 10.69411765
#define CONVY 14.91596639

/* Private variables ---------------------------------------------------------*/
Matrix matrix ;
Coordinate  display ;


Coordinate DisplayMargins[4] =   { {3750,250}, { 250, 250}, {3750,3800}, {250,3800}} ;

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2   /* 差值门限 */


/*******************************************************************************
* Function Name  : LPC17xx_SPI_SetSpeed
* Description    : Set clock speed to desired value
* Input          : - speed: speed
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LPC17xx_SPI_SetSpeed (uint8_t speed)
{
	speed &= 0xFE;
	if ( speed < 2  ) {
		speed = 2 ;
	}
	LPC_SSP1->CPSR = speed;
}

/*******************************************************************************
* Function Name  : ADS7843_SPI_Init
* Description    : ADS7843 SPI 初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
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

	LPC17xx_SPI_SetSpeed ( SPI_SPEED_400kHz );

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
* Description    : ADS7843端口初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/

// P0.6 --> TP_CS
// P0.7 --> SCK1
// P0.8/P0.9 --> MISO1/MOSI1

void TP_Init(void) 
{ 
  LPC_GPIO0->FIODIR |=  (1<<6);   /* P0.6 CS is output */
  LPC_GPIO2->FIODIR |=  (0<<13);  /* P2.13 TP_INT is input (touch screen) */
  TP_CS(1); 
  ADS7843_SPI_Init(); 
} 

/*******************************************************************************
* Function Name  : DelayUS
* Description    : 延时1us
* Input          : - cnt: 延时值
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void DelayUS(uint16_t cnt)
{
  uint16_t i;
  for(i = 0;i<cnt;i++)
  {
     uint16_t us = 100; /* 设置值为12，大约延1微秒 */    
     while (us--)     /* 延1微秒	*/
     {
       ;   
     }
  }
}


/*******************************************************************************
* Function Name  : WR_CMD
* Description    : 向 ADS7843写数据
* Input          : - cmd: 传输的数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static uint8_t WR_CMD (uint8_t cmd)  
{ 
  uint8_t byte_r;

  while (LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	     /* Wait for transfer to finish */
  LPC_SSP1->DR = cmd;
  while (LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	     /* Wait for transfer to finish */
  while( !( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) ) );	 /* Wait untill the Rx FIFO is not empty */
  byte_r = LPC_SSP1->DR;

  return byte_r;                                     /* Return received value */
} 



/*******************************************************************************
* Function Name  : RD_AD
* Description    : 读取ADC值
* Input          : None
* Output         : None
* Return         : ADS7843返回二字节数据
* Attention		 : None
*******************************************************************************/
static int RD_AD(void)  
{ 
  unsigned short buf,temp; 

  temp = WR_CMD(0x00);
  buf = temp<<8; 
  DelayUS(5); 
  temp = WR_CMD(0x00);;
  buf |= temp; 
  buf>>=3; 
  buf&=0xfff; 
  return buf; 
} 


/*******************************************************************************
* Function Name  : Read_X
* Description    : 读取ADS7843通道X+的ADC值 
* Input          : None
* Output         : None
* Return         : ADS7843返回通道X+的ADC值
* Attention		 : None
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
* Description    : 读取ADS7843通道Y+的ADC值
* Input          : None
* Output         : None
* Return         : ADS7843返回通道Y+的ADC值
* Attention		 : None
*******************************************************************************/
int Read_Y(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(CHY); 
  DelayUS(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;     
} 


/*******************************************************************************
* Function Name  : TP_GetAdXY
* Description    : 读取ADS7843 通道X+ 通道Y+的ADC值
* Input          : None
* Output         : None
* Return         : ADS7843返回 通道X+ 通道Y+的ADC值 
* Attention		 : None
*******************************************************************************/
void TP_GetAdXY(int *x,int *y)  
{ 
  int adx,ady; 
  adx=Read_X(); 
  DelayUS(1); 
  ady=Read_Y(); 
	//DelayUS(5);
  *x=adx; 
  *y=ady; 
} 

/*******************************************************************************
* Function Name  : Read_Ads7846
* Description    : 得到滤波之后的X Y
* Input          : None
* Output         : None
* Return         : Coordinate结构体地址
* Attention		 : None
*******************************************************************************/
Coordinate *Read_Ads7846(void)
{
  static Coordinate  screen;
  int i,sumx=0,sumy=0,counterx=0,countery=0;
  int TP_X[1],TP_Y[1];

  uint8_t count=0;
  int buffer[2][20]={{0},{0}};  
  do					       
  {		   
    TP_GetAdXY(TP_X,TP_Y);  
	  buffer[0][count]=TP_X[0];  
	  buffer[1][count]=TP_Y[0];
	  count++; 
		delay(1);
  }
  while(!TP_INT_IN && count<20);  
  screen.x=0;
	screen.y=0;
	
	for (i=0;i< count;i++){
		if(!(buffer[0][i]>3750 || buffer[0][i]<250)){
				sumx+=buffer[0][i];
			  counterx++;
		}
		if(!(buffer[1][i]>3750 || buffer[1][i]<250)){
				sumy+=buffer[1][i];
				countery++;
		}	
	}
	if(counterx!=0 && countery!=0){
		screen.x=sumx/counterx;
		screen.y=sumy/countery;
	}
	
	Map_Pixel(&screen);
  if(screen.x > 320 ||screen.y > 240){
		screen.x=0;
	  screen.y=0;
	}
	return &screen;

}

void Map_Pixel(Coordinate *touch){
	
	touch->x = 320 - (touch->x - DisplayMargins[1].x )/CONVX;
	touch->y = (touch->y - DisplayMargins[0].y)/CONVY + 1;
	return;
}
