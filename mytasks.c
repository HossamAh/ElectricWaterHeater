/*
 * mytasks.c
 *
 * Created: 7/5/2011 23:45:23
 *  Author: MMM
 */ 
#include "FreeRTOS.h"
#include "task.h"
#include "mytasks.h"
#include "LCD.h"
#include "Inputs.h"
#include "adc.h"

/* Priority definitions for most of the tasks in the demo application.  Some
tasks just use the idle priority. */
#define ON_TASK_PRIORITY ( 3 )//5
#define SEG_TASK_PRIORITY (3)//7
#define BLINK_TASK_PRIORITY ( 3 )
#define OFF_TASK_PRIORITY ( 3 )//4
#define SAVE_TASK_PRIORITY ( 1 )
#define SETTEMP_TASK_PRIORITY ( 3 )
#define SENSE_TASK_PRIORITY ( 3 )//6
#define LED_TASK_PRIORITY ( 3 )


xSemaphoreHandle CURRENT_TEMP_SEMPH;

unsigned char g_blinkFlagTemp;
unsigned char g_currentTemp;
unsigned char g_acumlatedTemp;
unsigned char g_setTemp;
unsigned char g_tempSettingModeFlag;
unsigned char g_Heater_state;
unsigned char g_Cooler_state;



xTaskHandle g_offTaskHandle;
xTaskHandle g_ONTaskHandle;
xTaskHandle g_SETTEMPTaskHandle;
xTaskHandle g_DisplayTempTaskHandle;
xTaskHandle g_senseTempTaskHandle;
xTaskHandle g_LEDTaskHandle;
xTaskHandle g_SettingModeTaskHandle;
xTaskHandle g_SenseTempTaskHandle;
xTaskHandle g_LEDTaskHandle;
xTaskHandle g_BlinkTaskHandle;


unsigned char g_offTaskEnable;
unsigned char g_ONTaskEnable;
unsigned char g_SETTEMPTaskEnable;
unsigned char g_DisplayTempTaskEnable;
unsigned char g_senseTempTaskEnable;
unsigned char g_LEDTaskEnable;
unsigned char g_SettingModeTaskEnable;
unsigned char g_SenseTempTaskEnable;
unsigned char g_LEDTaskEnable;
unsigned char g_BlinkTaskEnable;

void vOFFTask( void *pvParameters )
{
	SET_BIT(DDRA,4);
	for(;;)
	{
		TOGGLE_BIT(PORTA,4);
		SEG_OFF();
		LED_OFF();
		Heater_OFF();
		Cooling_OFF();
		if(Read_ON_OFF())
		{
			g_ONTaskEnable=1;
			xTaskCreate( vOnTask, ( signed char * ) "ONSTATE", configMINIMAL_STACK_SIZE, NULL, ON_TASK_PRIORITY, g_ONTaskHandle);
//				xTaskCreate( vLedTask, ( signed char * ) "LED", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIORITY, g_LEDTaskHandle );
//				CLEAR_BIT(DDRA,4);
			vTaskDelete(g_offTaskHandle);
			g_offTaskEnable=0;
		}
		else{}
		taskYIELD();

	}
}



void vOnTask(void *pvParameters)
{

	static bool save_task_creation_flag =0;
	static bool set_temp_task_creation_flag =0;
	static bool display_task_creation_flag =0;
	static bool senseTemp_task_creation_flag =0;
	static bool led_task_creation_flag =0;
//	portTickType xLastWakeTime;
//	const portTickType xFrequency = 1;
//
////	 Initialise the xLastWakeTime variable with the current time.
//	xLastWakeTime = xTaskGetTickCount();

	SET_BIT(DDRA,5);

	for(;;)
	{
		TOGGLE_BIT(PORTA,5);
		/*check on/off button
		 * pressed -> save current set temperature and go to off state
		 * otherwise do nothing*/
		if(Read_ON_OFF())
		{
	//		xTaskCreate( vsaveTempTask, ( signed char * ) "SAVETEMP", configMINIMAL_STACK_SIZE, NULL, SAVE_TASK_PRIORITY, g_SAVETaskHandle);

			vTaskDelete(g_ONTaskHandle);
		}
		else{}
		/*check up and down buttons
		 * up button pressed -> activate temperature setting mode*/
		if(set_temp_task_creation_flag==0)
		{
			if(Read_Up())
			{
//				TOGGLE_BIT(PORTA,7);
//				if(led_task_creation_flag==0)
//				{
//					/*Start the LED tasks to monitor the heater and cooler states*/
//					xTaskCreate( vLedTask, ( signed char * ) "LED", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIORITY, g_LEDTaskHandle );
//					led_task_creation_flag =1;
//				}
				g_tempSettingModeFlag = 1;
				g_blinkFlagTemp = 0;

				set_temp_task_creation_flag=1;
				xTaskCreate( vSettingTempTask, ( signed char * ) "SETTEMP", configMINIMAL_STACK_SIZE, NULL, SETTEMP_TASK_PRIORITY, g_SETTEMPTaskHandle);

//				vTaskDelete(g_ONTaskHandle);
//				vTaskDelete(g_LEDTaskHandle);
//				vTaskDelete(g_SenseTempTaskHandle);

			}
		}
		else{}
		/*Sense the temperature
		 * enable the ADC each 100 ms and update the current temperature after 10 reading.*/
		if(senseTemp_task_creation_flag==0)
		{
			g_acumlatedTemp = 0;
			xTaskCreate( vSenseTempTask, ( signed char * ) "SENSE", configMINIMAL_STACK_SIZE, NULL, SENSE_TASK_PRIORITY, g_SenseTempTaskHandle );
			senseTemp_task_creation_flag=1;
		}
		/*Display the current temperature on seven segments*/
		if(display_task_creation_flag==0)
		{
			g_tempSettingModeFlag = 0;
			g_blinkFlagTemp = 0;
			xTaskCreate( vDisplayTempTask, ( signed char * ) "SEG", configMINIMAL_STACK_SIZE, NULL, SEG_TASK_PRIORITY, g_DisplayTempTaskHandle );
			display_task_creation_flag =1;
		}

		if(led_task_creation_flag==0)
		{
			/*Start the LED tasks to monitor the heater and cooler states*/
			xTaskCreate( vLedTask, ( signed char * ) "LED", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIORITY, g_LEDTaskHandle );
			led_task_creation_flag =1;
		}
		/*check on the difference between the current water temperature and the set temperature
		 * to determine the state of the heater and the cooler
		 * set - current >= 5 then turn the heater on  and turn the cooler off
		 * set - current <= -5 then turn the heater off and turn the cooler on*/
		if(g_setTemp - g_currentTemp >=5)
		{
			Heater_ON();
			Cooling_OFF();
			g_Cooler_state =0;
			g_Heater_state=1;
		}
		else if(g_setTemp - g_currentTemp <= -5)
		{
			Heater_OFF();
			Cooling_ON();
			g_Cooler_state =1;
			g_Heater_state=0;
		}
		else
		{
			Heater_OFF();
			Cooling_OFF();
			g_Cooler_state =0;
			g_Heater_state=0;
		}
//		vTaskDelayUntil( &xLastWakeTime, xFrequency );
//		vTaskDelay(50);
		taskYIELD();
	}

}

void vLedTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 1000;

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		if(g_Cooler_state)
		{
			LED_ON();
//			vTaskDelayUntil( &xLastWakeTime, 20 );
		}
		else if(g_Heater_state)
		{
			LED_Toggle();
			vTaskDelayUntil( &xLastWakeTime, xFrequency );
		}
		taskYIELD();
	}
}

void vSenseTempTask(void *pvParameters)
{
	static unsigned char iterations =0;
	portTickType xLastWakeTime;
	const portTickType xFrequency = 100;

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
	uint32 temp;
	SET_BIT(DDRA,6);
	for(;;)
	{
		TOGGLE_BIT(PORTA,6);
		adc_readValue(2); /* read channel two where the temp sensor is connect */
		temp= ((uint32)g_adcResult*150*5)/(1023*1.5); /* calculate the temp from the ADC value*/
//		g_acumlatedTemp +=(uint8)temp;
		g_currentTemp = temp;
//		if(iterations==9)
//		{
//			g_currentTemp = g_acumlatedTemp/10;
//			g_acumlatedTemp = 0;
//		}
//		iterations = (iterations+1)%10;
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
//		taskYIELD();
	}
}

void vSettingTempTask( void *pvParameters )
{

//	vTaskSuspend(g_ONTaskHandle);
//	vTaskSuspend(g_LEDTaskHandle);
//	vTaskSuspend(g_SenseTempTaskHandle);

	xTaskCreate( vBlinkSEGTask, ( signed char * ) "BLINK", configMINIMAL_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, g_BlinkTaskHandle );

//	portTickType xLastWakeTime;
//	const portTickType xFrequency = 20;
//
////	 Initialise the xLastWakeTime variable with the current time.
//	xLastWakeTime = xTaskGetTickCount();
	SET_BIT(DDRA,7);
	for(;;)
	{
		TOGGLE_BIT(PORTA,7);
		g_tempSettingModeFlag = 1;
//		g_blinkFlagTemp = 0;
		if(Read_Up())
		{
			if((g_setTemp+5)<75)
			{
				g_setTemp+=5;
			}
			else
			{
				g_setTemp=75;
			}
		}
		else if(Read_Down())
		{
			if((g_setTemp-5)>35)
			{
				g_setTemp-=5;
			}
			else
			{
				g_setTemp=35;
			}
		}
		else{}
//		vTaskDelayUntil( &xLastWakeTime, xFrequency );
//		vTaskDelay(20);
		taskYIELD();
	}
}

void vDisplayTempTask( void *pvParameters )
{
//	portTickType xLastWakeTime;
//	const portTickType xFrequency = 50;
//
//	// Initialise the xLastWakeTime variable with the current time.
//	xLastWakeTime = xTaskGetTickCount();
//
	for( ;; )
	{
		if(g_tempSettingModeFlag==1 && g_blinkFlagTemp==1)
		{
			SEG_OFF();
		}
		else if(g_tempSettingModeFlag==1 && g_blinkFlagTemp==0)
		{
			SEG_Display_Num(g_setTemp);
		}
		else
		{
			SEG_Display_Num(g_currentTemp);
		}
//		vTaskDelay(1);
//		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		taskYIELD();
	}
	
}

void vBlinkSEGTask( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 100;

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		if(g_tempSettingModeFlag)
		{
			g_blinkFlagTemp = (g_blinkFlagTemp +1)%2;
//			vTaskDelay(100);
			vTaskDelayUntil( &xLastWakeTime, xFrequency );
		}
	}
}


