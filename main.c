
#include "Application.h"



	
int main(void) 
{
		xTaskCreate(TASK_Init,"TASK_Init",300,NULL,4,NULL);
		
		xTaskCreate(Task_Ultrasonic_Trigger,"Task_Ultrasonic_Trigger",100,NULL,4,NULL);
		
		xTaskCreate(Task_Distance,"Task_Distance",300,NULL,3,NULL);
		
		xTaskCreate(Task_LCD_Print,"Task_LCD_Print",100,NULL,2,NULL);

		xTaskCreate(Task_Display,"Task_Display",100,NULL,1,NULL);
		
		/* Start Scheduler */
		vTaskStartScheduler();

		while(1)
		{

		}
}


