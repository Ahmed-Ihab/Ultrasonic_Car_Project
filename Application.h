

#ifndef APP_H_
#define APP_H_

#include <std_types.h>



#include "..\MCAL\DIO\DIO.h"
#include "..\MCAL\Timer\Timer.h"
#include "..\MCAL\External_Interrupt\ExtInter.h"
#include "..\MCAL\PWM\PWM.h"


#include "..\HAL\LCD\LCD.h"
#include "..\HAL\DC_Motor\DC_Motor.h"

#include "../OS/OS.h"


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
typedef enum {DISTANCE_MODE , ECHO_TIME_MODE , DUTY_CYCLE_MODE}Display_mode_t;
typedef enum {ZERO_HUNDREDTH,ZERO_TENTH,NUMBER}Display_Number_t;
//typedef enum {UNITH,TENTH,HUNDREDTH}Data_Digit_t;
typedef enum {CONSISTING_ONE_DIGIT,CONSISTING_TWO_DIGIT,CONSISTING_THREE_DIGIT}Data_Digit_t;


//----------- Section of GLobal Variables ---------------------------------------//



//----------- Section of Public Prototypes of Functions and Tasks ---------------//


void TASK_Init (void * POINTERVOID );
void Task_Ultrasonic_Trigger (void * POINTERVOID);
void Task_Distance(void * POINTERVOID);
void Task_Display (void * POINTERVOID);
void Task_LCD_Print (void * POINTERVOID);


#endif /* APP_H_ */