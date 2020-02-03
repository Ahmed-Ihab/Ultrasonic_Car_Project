
#include "Application.h"


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ----------- Task_Name ------- Run_Time (ms) ------- Task_Run_Mode ------ Task_Status ------ Task_Priority (0 is Highest priority) -------- Task_Shift ------------//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------//

ST_Task_Info Task_1_Info={TASK_Init,1,PERIODIC,Ready,1,0};
ST_Task_Info Task_2_Info={Task_Ultrasonic_Trigger,1,PERIODIC,Ready,1,0};
ST_Task_Info Task_3_Info={Task_Distance,1,PERIODIC,Ready,3,0};
ST_Task_Info Task_4_Info={Task_LCD_Print,1,PERIODIC,Ready,2,0};
ST_Task_Info Task_5_Info={Task_Display,1,PERIODIC,Ready,4,0};
	
int main(void) 
{
	DDRB |= (1<<PB1) | (1<<PB0);
	DDRC = 0xFF;
	
	OS_Init(&OS_cnfg);

	// Creation of Task
	OS_Create_Task(&Task_1_Info);
	OS_Create_Task(&Task_2_Info);
	OS_Create_Task(&Task_3_Info);
	OS_Create_Task(&Task_4_Info);
	OS_Create_Task(&Task_5_Info);
	// Start Scheduler 
	OS_Run();

	while(1)
	{

	}
}
