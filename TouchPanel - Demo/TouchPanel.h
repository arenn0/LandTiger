/*--------------------------------------------------------------------------------------------
Name: TouchPanel.h
Purpose: Prototypes declaration for touch panel functions
Note(s): This work is licensed under the Creative Commons Attribution 3.0 Italy License. 
				 To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/it/ 
				 or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
--------------------------------------------------------------------------------------------*/

#ifndef _TOUCHPANEL_H_
#define _TOUCHPANEL_H_

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"

/* Private typedef -----------------------------------------------------------*/
typedef	struct POINT 
{
   uint16_t x;
   uint16_t y;
}Coordinate;

#define	CHX 	        0x90 	
#define	CHY 	        0xd0	

#define SSPSR_RNE       2
#define SSPSR_BSY       4

/* bit-frequency = PCLK / CPSR */
#define SPI_SPEED_4MHz    18 	  /* 4MHz */
#define SPI_SPEED_2MHz    36 	  /* 2MHz */
#define SPI_SPEED_400kHz  180	  /* 400kHz */


#define TP_CS(a)	if (a)	\
					LPC_GPIO0->FIOSET = (1<<6);\
					else		\
					LPC_GPIO0->FIOCLR = (1<<6)

#define TP_INT_IN   ( LPC_GPIO2->FIOPIN & (1<<13) ) 

/* Private function prototypes -----------------------------------------------*/				
void TP_Init(void);	
Coordinate *Read_Ads7846(void);
void Map_Pixel(Coordinate *touch);

#endif



