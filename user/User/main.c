#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "Serial.h"




 int main(void)
 {
	 
	  OLED_Init();
	  
	  Serial_Init();
	 
	 OLED_ShowString(1,1,"TxPacket");
	 OLED_ShowString(3,1,"RxPacket");
	 
	
	 
	 
	
	 while(1)
	 {
		if(Serial_GetRxFlag () == 1)
		{
			OLED_ShowString (4,1,"                 ");
			OLED_ShowString (4,1,Serial_RxPacket);
	 }
 }
	 

 
 
 
 
 
 
 }