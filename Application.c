
#include "Application.h"

//----------- Section of Private Constant Macros ---------------//

#define RISING_EDGE  1
#define FALLING_EDGE 0

#define VELOCITY	34

#define ACTIVE_TRIGGER	 1
#define DEACTIVE_TRIGGER 0

#define REQUEST_FINISHED 0
#define NO_REQUEST		 0
#define REQUEST			 1

#define UNAVALIABLE	0
#define AVALIABLE	1

#define UPDATE_DISPLAY_TIME 3

#define DISABLE_INTERRUPT()  SREG &= ~(1u<< 7 )  // Disable GLOBAL INTERRUPT
#define ENABLE_INTERRUPT()   SREG |= (1u<< 7 )   // Enable GLOBAL INTERRUPT

#define EXECUTION_TIME_OF_TASK_PORT PORTC				//test
#define TASK1 1											//test
#define TASK2 2											//test
#define TASK3 3											//test
#define TASK4 4											//test
#define TASK_INIT	  1									//test
#define TASK_TRIGGER  2									//test
#define TASK_DISTANCE 3									//test
#define TASK_DISPLAY  4									//test
#define EXTERNAL_INTERRUPT_HANDLER 5					//test




/************************* ----------- Section of Static Private Global Variables ------------ *******************/


//------------------------------ Section of creating variables from structure  ---------------------//


//--------------------------------------------------------------------------------------------------//


//------------------------------ Section of creating variables from enum ---------------------------//



//--------------------------------------------------------------------------------------------------//


//------------------------------ Section of Static Global Pointers ---------------------------------//


//--------------------------------------------------------------------------------------------------//


//------------------------------ Section of Static Global Counters ---------------------------------//
static volatile uint32 g_u32_Timer0_Overflow_Count = 0;
static volatile uint8 g_u8_Task_Trigger_Counter = 0;
static volatile uint8 g_u8_Duty_Cycle_Display =0;	
//--------------------------------------------------------------------------------------------------//


//------------------------------ Section of Static Global Flags ------------------------------------//
static volatile uint8 g_u8_Initialization_Flag=0;
static volatile uint8 g_u8_Trigger_Active_Flag=ACTIVE_TRIGGER;
static volatile uint8 g_u8_Manipulation_on_Distance_Flag=0;
static volatile uint8 g_u8_Display_Flag=0;


static uint8 g_u8_LCD_Request_Flag = NO_REQUEST;
static uint8 g_u8_LCD_Available_Flag = AVALIABLE;

//--------------------------------------------------------------------------------------------------//


//------------------------------ Section of Static Global variables --------------------------------//

static float32 g_f32_Distance = 0;
static uint8 *Distance_String ="Distance=   cm";
static uint8 *Echo_String = "Echo=   ms";
static uint32 g_u32_Echo_Time_On = 0;
static uint32 g_u32_Echo_Timer_Count=0;

static uint8 g_u8_LCD_Row=255;
static uint8 g_u8_LCD_Column=255;

static uint8 g_u8_LCD_Print_Numbers = 0;
static uint8 * g_u8_LCD_Print_String = NULL_POINTER;
//--------------------------------------------------------------------------------------------------//



/****************************************************************************************************************/


//----------- Section of Private Prototypes of Functions   -------------//

static void Ultrasonic_Echo_Measure_Time_ON_ISR_Handler (void);
static void Count_Overflow_Timer0_ISR_Handler (void);
static void Approximate_Result (volatile float32 * local_Ptr_f32_Echo_Time_On);
//----------- Section of implementation of Function (Function Definition)  -------------//

void Count_Overflow_Timer0_ISR_Handler (void)
{
	g_u32_Timer0_Overflow_Count++;
}

void Approximate_Result (volatile float32 * local_Ptr_f32_Data)
{
	uint32 Integer_Number;
	uint32 Float_Number;

	Integer_Number = (uint32)(*local_Ptr_f32_Data);
	Float_Number = (uint32)( ( (*local_Ptr_f32_Data)  - (Integer_Number) ) *10 ) ;

	if(Float_Number >= 5)
	{
		Integer_Number++;
	}
	else if (Float_Number < 5)
	{
		//Do Nothing
	}
	else
	{
		//Shouldn't be here
	}
	
	*local_Ptr_f32_Data = Integer_Number;
}


void Ultrasonic_Echo_Measure_Time_ON_ISR_Handler (void)
 {
	 EXECUTION_TIME_OF_TASK_PORT ^= (1<<EXTERNAL_INTERRUPT_HANDLER); //test
	 
	 static volatile uint8 local_u8_Edge_Flag = RISING_EDGE;
	 
	 float32 local_f32_Distance = 0xFF;
	 volatile uint8 local_u8_Echo_Timer_Count = 0;
	 volatile float32 local_f32_Echo_Time_On = 0;
	 volatile uint32 Local_u32_Total_Number_Echo_Timer_Count=0;
	 
	  if (local_u8_Edge_Flag == FALLING_EDGE)
	 {
		  local_u8_Echo_Timer_Count = TCNT0 ;
		  
		  TIMER_Stop(timer0);
		  INTP2_VidSelectEvent(INT2_RisingEdge);
		  
		  Local_u32_Total_Number_Echo_Timer_Count = (uint32) ( local_u8_Echo_Timer_Count +  g_u32_Timer0_Overflow_Count* 255);
		  local_f32_Echo_Time_On = (float32) (Local_u32_Total_Number_Echo_Timer_Count) * ( ( (  ( (float32) (Prescalar_Factor[PRESCALER64]) /F_CPU ) )*1000 ) ) ;	//we multiple by 1000 to get the time units in Milliseconds
		  
		  Approximate_Result(&local_f32_Echo_Time_On);
		  
		  local_u8_Edge_Flag = RISING_EDGE;
		  g_u8_Trigger_Active_Flag = ACTIVE_TRIGGER;
		  g_u8_Manipulation_on_Distance_Flag=1;
		  g_u8_Display_Flag = 1;
		  
		  
		  g_u32_Echo_Timer_Count = Local_u32_Total_Number_Echo_Timer_Count;	    //test
		  g_u32_Echo_Time_On = local_f32_Echo_Time_On;							//test 
		  
		  g_u32_Timer0_Overflow_Count=0; 
		  
		  if (local_f32_Echo_Time_On < 24)
		  {  
			  local_f32_Distance = (local_f32_Echo_Time_On * VELOCITY) / 2.0;	//Distance unit in Centimeter
			  
			  if(local_f32_Distance != 0)
			  {
				 // g_f32_Distance = local_f32_Distance/17;	//test
				  g_f32_Distance = local_f32_Distance;
			  }
			  
		  }
		  
		  else if (local_f32_Echo_Time_On > 24)
		  {
			  g_f32_Distance = 400;
		  }
		  
		  else
		  {
			  //Do Nothing
		  }
		  
	 }
	 
	else if(local_u8_Edge_Flag == RISING_EDGE)
	{
		
		TIMER_Start(timer0);
		local_u8_Edge_Flag = FALLING_EDGE;
		INTP2_VidSelectEvent(INT2_FallingEdge);
		/*
		if(local_f32_Distance == 0)
		{
			 g_u8_Trigger_Active_Flag = ACTIVE_TRIGGER;
		}
		*/
	}
	 
	 else
	 {
		//Shouldn't be here
		//Do Nothing
	 }
	
	 EXECUTION_TIME_OF_TASK_PORT ^= (1<<EXTERNAL_INTERRUPT_HANDLER); //test
 }


void TASK_Init (void * POINTERVOID )
{
	if(g_u8_Initialization_Flag == 0)
	{
		EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_INIT); //test
		
		static Function_Process_t LCD_State = NOT_FINISHED;
		static uint8 local_u8_init_Flag = 0;
		if(local_u8_init_Flag == 0)
		{
			DIO_init();
			INTP_vidInit();
			TIMER_ID_init(timer1);
			TIMER_ID_init(timer0);
			PWM_Init();
			MOTORS_Initialization();
			INTP2_vidSetCallBack(Ultrasonic_Echo_Measure_Time_ON_ISR_Handler);
			Timer0_OVF_Set_Callback(Count_Overflow_Timer0_ISR_Handler);
			local_u8_init_Flag = 1;
		}
		
		LCD_State = LCD_4Bits_Initialization_OS();
		
		if(LCD_State == FINISHED)
		{
			//LCD_4Bits_Print_Number(1,1,OCR2); //test
			g_u8_Initialization_Flag = 1;
			LCD_4Bits_Print_String(1,1,Distance_String);
			LCD_4Bits_Print_String(2,1,Echo_String);
			LCD_4Bits_Print_Character(2,15,'%');
			//OS_Delete_Task(&Task_1_Info);
		}
		EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_INIT); //test	
	}
}

void Task_Ultrasonic_Trigger(void * POINTERVOID)
{

	if(g_u8_Initialization_Flag == 1)
	{
		EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_TRIGGER); //test
		/*
		g_u8_Task_Trigger_Counter++;
		
		if (g_u8_Task_Trigger_Counter > 200);
		{
			g_u8_Trigger_Active_Flag= ACTIVE_TRIGGER;
		}
		*/	
		 if(g_u8_Trigger_Active_Flag == ACTIVE_TRIGGER)
		{
			//g_u8_Task_Trigger_Counter=0;	  
			DIO_Write_Pin(TRIGGER_PORT_,TRIGGER_PIN , HIGH);
			//Time_Delay(timer2,500,us);
			_delay_us(50);
			DIO_Write_Pin(TRIGGER_PORT_,TRIGGER_PIN , LOW);
			
			g_u8_Trigger_Active_Flag = DEACTIVE_TRIGGER;
		}
	EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_TRIGGER); //test
	}
	
}

 

void Task_Distance(void * POINTERVOID)
{
	
	static uint16 local_u16_Task_Counter=0;
	
	if(g_u8_Initialization_Flag == 1)
	{
		EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_DISTANCE);
			
		static uint8 local_u8_Duty_Cycle = 0;
		
		if(g_u8_Manipulation_on_Distance_Flag == 1)
		{
			if(g_f32_Distance >= 400)
			{
				local_u8_Duty_Cycle = 60;
				MOTOR_Direction(MOTOR_1,FORWARD);
				MOTOR_Direction(MOTOR_2,FORWARD);
				PWM_Timer1_Generate(local_u8_Duty_Cycle,1000,Channel_A_B,Non_Inverting_PWM);
				g_u8_Duty_Cycle_Display = local_u8_Duty_Cycle;
			}
			
			else if( (g_f32_Distance > 40) && (g_f32_Distance < 400) )
			{
				local_u16_Task_Counter = 0;	//Reset the Counter after the car has been away from object by 40cm
				
				local_u8_Duty_Cycle = 40;
				MOTOR_Direction(MOTOR_1,FORWARD);
				MOTOR_Direction(MOTOR_2,FORWARD);
				PWM_Timer1_Generate(local_u8_Duty_Cycle,1000,Channel_A_B,Non_Inverting_PWM);
				g_u8_Duty_Cycle_Display = local_u8_Duty_Cycle;
				
			}
			
			else if (g_f32_Distance < 20) 
			{
				
				if(local_u16_Task_Counter < 1000)
				{	
					g_u8_Duty_Cycle_Display = 0;
					local_u16_Task_Counter++;
					MOTOR_Stop(MOTOR_1);
					MOTOR_Stop(MOTOR_2);	
				}
				else
				{	
					PORTC ^= (1<<PC4);
					local_u8_Duty_Cycle = 20;
					MOTOR_Direction(MOTOR_1,BACKWARD);
					MOTOR_Direction(MOTOR_2,BACKWARD);
					PWM_Timer1_Generate(local_u8_Duty_Cycle,1000,Channel_A_B,Non_Inverting_PWM);
					g_u8_Duty_Cycle_Display = local_u8_Duty_Cycle;
				}

			}
			g_u8_Manipulation_on_Distance_Flag=0;
		}
	EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_DISTANCE);	//test
	}
}


void Task_Display(void * POINTERVOID)
{
	
	if(g_u8_Initialization_Flag == 1)
	{
		EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_DISPLAY); //test
		if(g_u8_Display_Flag == 1)
		{
			if ( g_f32_Distance < 9 )
			{
				LCD_4Bits_Print_Number(1,10,0);
				LCD_4Bits_Print_Number(1,11,0);
				LCD_4Bits_Print_Number(1,12,g_f32_Distance);
			}
			
			else if ( ( g_f32_Distance > 9 ) && ( g_f32_Distance < 99 ) )
			{
				LCD_4Bits_Print_Number(1,10,0);
				LCD_4Bits_Print_Number(1,11,g_f32_Distance);
			}
			
			else if ( ( g_f32_Distance > 99 ) && ( g_f32_Distance < 999 ) )
			{					
				LCD_4Bits_Print_Number(1,10,g_f32_Distance);
			}
			
			else
			{
				//Do Nothing
			}
			


			if(g_u8_Duty_Cycle_Display < 10)
			{
				LCD_4Bits_Print_Number(2,12,0);
				LCD_4Bits_Print_Number(2,13,g_u8_Duty_Cycle_Display);
			}
			
			else if ( (g_u8_Duty_Cycle_Display > 10) && (g_u8_Duty_Cycle_Display < 100) )
			{
				LCD_4Bits_Print_Number(2,12,g_u8_Duty_Cycle_Display);
			}
			
			else
			{
				LCD_4Bits_Print_Number(2,11,g_u8_Duty_Cycle_Display);
				
			}
//------------------------------------------------------------------------------------------------//
			if ( g_u32_Echo_Time_On < 9 )
			{	
				LCD_4Bits_Print_Number(2,6,0);					//test Echo time
				LCD_4Bits_Print_Number(2,7,0);					//test Echo time
				LCD_4Bits_Print_Number(2,8,g_u32_Echo_Time_On);	//test Echo time
				
			}
			
			else if ( ( g_u32_Echo_Time_On > 9 ) && ( g_u32_Echo_Time_On < 99 ) )
			{
				LCD_4Bits_Print_Number(2,6,0);					//test Echo time
				LCD_4Bits_Print_Number(2,7,g_u32_Echo_Time_On);	//test Echo time
			}
			
			else if ( ( g_u32_Echo_Time_On > 99 ) && ( g_u32_Echo_Time_On < 999 ) )
			{
				LCD_4Bits_Print_Number(2,6,g_u32_Echo_Time_On);	//test Echo time
				
			}
			
			else
			{
				//Do Nothing
			}

//------------------------------------------------------------------------------------------------//
			g_u8_Display_Flag=0;
		}
		
		EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_DISPLAY); //test
	}
}

/*
void Task_Display(void * POINTERVOID)
{
	static Display_mode_t Display_mode = DISTANCE_MODE;
	static Display_Number_t Display_Digit=ZERO_HUNDREDTH;	
	static Data_Digit_t Display_Distance_Digit = 255;
	static Data_Digit_t Display_Echo_Digit = 255;
	static uint8 local_u8_Task_Counter = 0;
	static uint8 local_u32_Distance = 0;
	static uint8 local_u32_Echo_Time = 0;
	
	//static volatile uint8 local_u8_update_Flag = 0;
	if(g_u8_Initialization_Flag == 1)
	{
		EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_DISPLAY); //test

		if(g_u8_Display_Flag == 1)
		{
			if (local_u8_Task_Counter > (UPDATE_DISPLAY_TIME-1) )
			{
				local_u8_Task_Counter=0;
			
			}
			else
			{
				local_u8_Task_Counter++;
				switch(Display_mode)
				{
					case DISTANCE_MODE:
					{
						if ( (local_u8_Task_Counter <= UPDATE_DISPLAY_TIME) )
						{
							if(Display_Distance_Digit == CONSISTING_ONE_DIGIT)
							{
								switch(Display_Digit)
								{
									case ZERO_HUNDREDTH:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=0;
											g_u8_LCD_Row=1;
											g_u8_LCD_Column=10;
											Display_Digit=ZERO_TENTH;
										}
										break;
									}
									
									case ZERO_TENTH:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=0;
											g_u8_LCD_Row=1;
											g_u8_LCD_Column=11;
											Display_Digit=NUMBER;
										}
										break;
									}
									
									case NUMBER:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=local_u32_Distance;
											g_u8_LCD_Row=1;
											g_u8_LCD_Column=12;
											Display_Digit=ZERO_HUNDREDTH;
											if(local_u8_Task_Counter == UPDATE_DISPLAY_TIME)
											{
												Display_mode=ECHO_TIME_MODE;
											}
										}
										break;
									}
									
									default:
									{
										// Shouldn't be here
										break;
									}
								}
							}
							
							else if(Display_Distance_Digit == CONSISTING_TWO_DIGIT)
							{
								switch(Display_Digit)
								{
									case ZERO_HUNDREDTH:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=0;
											g_u8_LCD_Row=1;
											g_u8_LCD_Column=10;
											Display_Digit=ZERO_TENTH;
										}
										break;
									}
									
									case ZERO_TENTH:
									{
										Display_Digit=NUMBER;
										break;
									}
									
									case NUMBER:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=local_u32_Distance;
											g_u8_LCD_Row=1;
											g_u8_LCD_Column=11;
											Display_Digit=ZERO_HUNDREDTH;
											if(local_u8_Task_Counter == UPDATE_DISPLAY_TIME)
											{
												Display_mode=ECHO_TIME_MODE;
											}
										}
										break;
									}
									
									default:
									{
										//shouldn't be here
										break;
									}
								}
							}
							
							else if(Display_Distance_Digit == CONSISTING_THREE_DIGIT)
							{
								switch(Display_Digit)
								{
									case ZERO_HUNDREDTH:
									{
										Display_Digit=ZERO_TENTH;
										break;
									}
									
									case ZERO_TENTH:
									{
										Display_Digit=NUMBER;
										break;
									}
									
									case NUMBER:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=local_u32_Distance;
											g_u8_LCD_Row=1;
											g_u8_LCD_Column=10;
											Display_Digit=ZERO_HUNDREDTH;
											if(local_u8_Task_Counter == UPDATE_DISPLAY_TIME)
											{
												Display_mode=ECHO_TIME_MODE;
											}		
										}
										break;
									}
									
									default:
									{
										//shouldn't be here
										break;
									}
								}
							}	
						}
						
						 if(local_u8_Task_Counter == UPDATE_DISPLAY_TIME)
						{
							if ( ( g_f32_Distance < 9 ) )
							{
								Display_Distance_Digit = CONSISTING_ONE_DIGIT;
								local_u32_Distance= g_f32_Distance;
								
							}
							
							else if  (( g_f32_Distance > 9 ) && ( g_f32_Distance < 99 ) )
							{
								Display_Distance_Digit = CONSISTING_TWO_DIGIT;
								local_u32_Distance= g_f32_Distance;
								
							}
							
							else if ( ( g_f32_Distance > 99 ) )
							{
								Display_Distance_Digit = CONSISTING_THREE_DIGIT;
								local_u32_Distance= g_f32_Distance;
								
							}
							
							else
							{
								//DO NOTHING
							}
						}

						break;
					}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

					case ECHO_TIME_MODE:
					{
						if((local_u8_Task_Counter <= UPDATE_DISPLAY_TIME))
						{
							if(Display_Echo_Digit == CONSISTING_ONE_DIGIT)
							{
								switch(Display_Digit)
								{
									case ZERO_HUNDREDTH:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=0;
											g_u8_LCD_Row=2;
											g_u8_LCD_Column=6;
											Display_Digit=ZERO_TENTH;
										}
										break;
									}
									
									case ZERO_TENTH:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=0;
											g_u8_LCD_Row=2;
											g_u8_LCD_Column=7;
											Display_Digit=NUMBER;
										}
										break;
									}
									
									case NUMBER:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=local_u32_Echo_Time;
											g_u8_LCD_Row=2;
											g_u8_LCD_Column=8;
											Display_Digit=ZERO_HUNDREDTH;
											if(local_u8_Task_Counter == UPDATE_DISPLAY_TIME)
											{
												Display_mode=DISTANCE_MODE;
											}
										}
										break;
									}
									
									default:
									{
										// Shouldn't be here
										break;
									}
								}
							}
							
							else if(Display_Echo_Digit == CONSISTING_TWO_DIGIT)
							{
								switch(Display_Digit)
								{
									case ZERO_HUNDREDTH:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=0;
											g_u8_LCD_Row=2;
											g_u8_LCD_Column=6;
											Display_Digit=ZERO_TENTH;
										}
										break;
									}
									
									case ZERO_TENTH:
									{
										Display_Digit=NUMBER;
										break;
									}
									
									case NUMBER:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=local_u32_Echo_Time;
											g_u8_LCD_Row=2;
											g_u8_LCD_Column=7;
											Display_Digit=ZERO_HUNDREDTH;
											if(local_u8_Task_Counter == UPDATE_DISPLAY_TIME)
											{
												Display_mode=DISTANCE_MODE;
											}
										}
										break;
									}
									
									default:
									{
										//shouldn't be here
										break;
									}
								}
							}
							
							else if(Display_Echo_Digit == CONSISTING_THREE_DIGIT)
							{
								switch(Display_Digit)
								{
									case ZERO_HUNDREDTH:
									{
										Display_Digit=ZERO_TENTH;
										break;
									}
									
									case ZERO_TENTH:
									{
										Display_Digit=NUMBER;
										break;
									}
									case NUMBER:
									{
										if(g_u8_LCD_Available_Flag == AVALIABLE)
										{
											g_u8_LCD_Request_Flag = REQUEST;
											g_u8_LCD_Print_Numbers=local_u32_Echo_Time;
											g_u8_LCD_Row=2;
											g_u8_LCD_Column=6;
											Display_Digit=ZERO_HUNDREDTH;
											if(local_u8_Task_Counter == UPDATE_DISPLAY_TIME)
											{
												Display_mode=DISTANCE_MODE;
											}
										}
										break;
									}
									
									default:
									{
										//shouldn't be here
										break;
									}
								}
							}
						}
						
						if(local_u8_Task_Counter == UPDATE_DISPLAY_TIME)
						{
							if ( ( g_u32_Echo_Time_On < 9 ) )
							{
								Display_Echo_Digit = CONSISTING_ONE_DIGIT;
								local_u32_Echo_Time = g_u32_Echo_Time_On;
								
							}
							
							else if  (( g_u32_Echo_Time_On > 9 ) && ( g_u32_Echo_Time_On < 99 ) )
							{
								Display_Echo_Digit = CONSISTING_TWO_DIGIT;
								local_u32_Echo_Time = g_u32_Echo_Time_On;
							}
							
							else if ( ( g_u32_Echo_Time_On > 999 ) )
							{
								Display_Echo_Digit = CONSISTING_THREE_DIGIT;
								local_u32_Echo_Time = g_u32_Echo_Time_On;
								
							}
							
							else
							{
								//Do NOTHING
							}
						}
						
						break;
					}
					
					
					case DUTY_CYCLE_MODE:
					{
						
						break;
					}
					
					default:
					{
						//Shouldn't be here
						//Do Nothing
						break;
					}
				}
				
				
			}	
			
		}
		EXECUTION_TIME_OF_TASK_PORT ^= (1<<TASK_DISPLAY); //test
	}
}
*/

void Task_LCD_Print (void * POINTERVOID)
{
	if(g_u8_Initialization_Flag == 1)
	{
		if(g_u8_LCD_Request_Flag == REQUEST)
		{
			g_u8_LCD_Available_Flag = UNAVALIABLE;
			LCD_4Bits_Print_Number(g_u8_LCD_Row,g_u8_LCD_Column,g_u8_LCD_Print_Numbers);
			g_u8_LCD_Available_Flag = AVALIABLE;
			g_u8_LCD_Request_Flag=REQUEST_FINISHED;
		}
	}
	
}




