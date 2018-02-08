/*--------------------------------------------------------------------------------------------
Name: IRQ_timer.c
Purpose: Functions to enable timer interrupt handler
Note(s): This work is licensed under the Creative Commons Attribution 3.0 Italy License. 
				 To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/it/ 
				 or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
--------------------------------------------------------------------------------------------*/
#include "../lpc17xx.h"
#include "timer.h"

/*******************************************************************************
* Function Name  : TIMER0_IRQHandler
* Description    : timer 0 interrupt handler
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void TIMER0_IRQHandler (void)
{
	static int flag=1;   //buzzer "disable"
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
	
	if(flag==0){
		LPC_GPIO0->FIOCLR |= (1<<26);
		//flag=1;
	}
	else{
		LPC_GPIO0->FIOSET |= (1<<26);
		//flag=0;
	}
	
  return;
}

/*******************************************************************************
* Function Name  : TIMER1_IRQHandler
* Description    : timer 1 interrupt handler
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

