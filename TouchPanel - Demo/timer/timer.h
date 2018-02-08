/*--------------------------------------------------------------------------------------------
Name: timer.h
Purpose: Prototypes of functions included in the lib_timer, funct_timer, IRQ_timer .c files
Note(s): This work is licensed under the Creative Commons Attribution 3.0 Italy License. 
				 To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/it/ 
				 or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
--------------------------------------------------------------------------------------------*/

#ifndef __TIMER_H
#define __TIMER_H

#define TIME_INTERVAL	(9000000/100 - 1)

/* funct_timer.c */
extern void delayMs(uint8_t timer_num, uint32_t delayInMs);
/* init_timer.c */
extern uint32_t init_timer( uint8_t timer_num, uint32_t timerInterval );
extern void enable_timer( uint8_t timer_num );
extern void disable_timer( uint8_t timer_num );
extern void reset_timer( uint8_t timer_num );
/* IRQ_timer.c */
extern void TIMER0_IRQHandler (void);
extern void TIMER1_IRQHandler (void);

#endif /* end __TIMER_H */

