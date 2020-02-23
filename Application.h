

#ifndef APP_H_
#define APP_H_

#include <std_types.h>



#include "..\MCAL\DIO\DIO.h"
#include "..\MCAL\Timer\Timer.h"
#include "..\MCAL\External_Interrupt\ExtInter.h"
#include "..\MCAL\PWM\PWM.h"


#include "..\HAL\LCD\LCD.h"
#include "..\HAL\DC_Motor\DC_Motor.h"

#include "../FreeRTOS/FreeRTOS.h"
#include "../FreeRTOS/task.h"
#include "../FreeRTOS/queue.h"
#include "../FreeRTOS/event_groups.h"
#include "../FreeRTOS/semphr.h"

/*
#include "std_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "DIO.h"
#include "PWM.h"
#include "ExtInter.h"
#include "LCD.h"
#include "DC_Motor.h"
*/


//----------- Section of Public Constant Macros --------------------------------//



//------------ Section of Structure  -------------------------------------------//



//------------ Section of enum  -------------------------------------------//
typedef enum {PRINT_DISTANCE , PRINT_DUTY_CYCLE}Print_Order_t;



//----------- Section of GLobal Variables ---------------------------------------//



//----------- Section of Public Prototypes of Functions and Tasks ---------------//


void TASK_Init (void * POINTERVOID );
void Task_Ultrasonic_Trigger (void * POINTERVOID);
void Task_Distance(void * POINTERVOID);
void Task_Display (void * POINTERVOID);
void Task_LCD_Print (void * POINTERVOID);


#endif /* APP_H_ */