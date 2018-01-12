    #include "LCD.h"   
    #include "LPC17xx.h"  
	
    int main(void)  
    {  
      LCD_Initialization();  
      LCD_Clear(Red);  
      GUI_Text(56,144,"HY-LandTiger V2.0",White,Red);  
      GUI_Text(52,160,"Development Board",White,Red);  
      /* Infinite loop */  
      while (1)  
      {  
      
      }  
    }  
      
    #ifdef  USE_FULL_ASSERT  
        
    void assert_failed(uint8_t* file, uint32_t line)  
    {    
      while (1)  
      {  
      }  
    }  
    #endif   
	 