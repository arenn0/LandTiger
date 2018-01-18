																			 /*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           timer.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the lib_timer, funct_timer .c files
** Correlated files:    lib_timer.c, funct_timer.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

/* lib_led */
void LED_init(void);
void LED_deinit(void);

/* funct_led */
void LED_On (unsigned int num);
void LED_Off (unsigned int num);
void LED_Out(unsigned int value);

