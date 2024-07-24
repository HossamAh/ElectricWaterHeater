/*
 * main.c
 *
 *  Created on: Mar 16, 2022
 *      Author: Hossam
 */

#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "mytasks.h"
#include "LCD.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "Inputs.h"
#include "event_groups.h"
#include "adc.h"

SemaphoreHandle_t  xSemaphoreState = NULL;
SemaphoreHandle_t  xSemaphoreON_OFF = NULL;
SemaphoreHandle_t  xSemaphoreUP_DOWN = NULL;
SemaphoreHandle_t  xSemaphoreCurrentTemp = NULL;
SemaphoreHandle_t  xSemaphoreSetTemp = NULL;

uint8 g_currentTemp;
uint8 g_setTemp;
uint8 g_heaterState=0;
uint8 g_CoolerState=0;


TaskHandle_t g_offTaskHandle;
TaskHandle_t g_ONTaskHandle;
TaskHandle_t g_SETTEMPTaskHandle;

void OFF_StateTask(void *pvParameters);
void OperatingStateTask(void *pvParameters);
void TempSettingState(void *pvParameters);
void SSDTASK(void *pvParameters);
void TempSensorTask(void *pvParameters);
void T6(void *pvParameters);
//#define mainLED_TASK_PRIORITY			( tskIDLE_PRIORITY )
#define OFF_TASK_PRIORITY ( 3 )//4
uint8 g_SystemState=0;/*
*0->off state
*1->operating state
*2->temp setting state */

portSHORT main(void)
{
//	LCD_init();
	OUTPUTS_init();
	Inputs_init();
	adc_init();



	g_currentTemp = 15;
	g_setTemp = 25;


	xTaskCreate( OFF_StateTask,"ON/OFF", configMINIMAL_STACK_SIZE, NULL, 1, &g_offTaskHandle );
	xTaskCreate( OperatingStateTask,"OP_State", configMINIMAL_STACK_SIZE, NULL, 1, &g_ONTaskHandle );
	xTaskCreate( TempSettingState,"Temp_setting", configMINIMAL_STACK_SIZE, NULL, 1, &g_SETTEMPTaskHandle );
	xTaskCreate( SSDTASK,"SSDTASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
	xTaskCreate( TempSensorTask,"TempSensorTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
	xTaskCreate( T6,"T6", configMINIMAL_STACK_SIZE, NULL, 1, NULL );




	xSemaphoreON_OFF = xSemaphoreCreateMutex();
	xSemaphoreUP_DOWN = xSemaphoreCreateMutex();
	xSemaphoreCurrentTemp = xSemaphoreCreateMutex();
	xSemaphoreSetTemp = xSemaphoreCreateMutex();
	xSemaphoreState = xSemaphoreCreateMutex();
//start scheduler
	vTaskStartScheduler();

	while(1)
    {

    }
		return 0;
}
void OFF_StateTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 30;
	SET_BIT(DDRC,0);
	for(;;)
	{
		if(xSemaphoreTake(xSemaphoreState,1)==pdTRUE)
		{
			if(g_SystemState==0)
			{
				TOGGLE_BIT(PORTC,0);
				SEG_OFF();
				LED_OFF();
				Heater_OFF();
				Cooling_OFF();
				if(Read_ON_OFF())
				{
						/*critical section */
						g_SystemState = 1;//operating state
				}
				else{}
			}
			else if(g_SystemState==1 || g_SystemState==2)
			{
				if(Read_ON_OFF())
				{
						/*critical section */
						g_SystemState = 0;//off state
				}
			}
			xSemaphoreGive(xSemaphoreState);
		}

		vTaskDelayUntil( &xLastWakeTime, xFrequency );

	}
}
void OperatingStateTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 30;
	SET_BIT(DDRC,1);
	for(;;)
	{

		if(xSemaphoreTake(xSemaphoreState,1)==pdTRUE)
		{
			if(g_SystemState==1)//operating state
			{
				TOGGLE_BIT(PORTC,1);
				if(Read_Up()||Read_Down())
				{
					/*critical section */
					g_SystemState = 2;//Temp Setting state

					/*critical section of current temp*/
					if(xSemaphoreTake(xSemaphoreCurrentTemp,1)==pdTRUE)
					{
						g_setTemp = g_currentTemp;
						xSemaphoreGive(xSemaphoreCurrentTemp);
					}

				}
				else{}
			}
			else{}
			xSemaphoreGive(xSemaphoreState);
		}

		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
void TempSettingState(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 15;
	static count_5sec = 0;/*to count time to 5 sec 100 cycle to exit*/
	SET_BIT(DDRC,2);
	for(;;)
	{
		if(xSemaphoreTake(xSemaphoreState,1)==pdTRUE)
		{
			if(g_SystemState==2)//Temp setting
			{
				TOGGLE_BIT(PORTC,2);
				uint8 up,down;
				up = Read_Up();
				down = Read_Down();
				if(up || down)
				{
					count_5sec=0;//reset exit timeout
					/*critical section of current temp*/
					if(xSemaphoreTake(xSemaphoreSetTemp,1)==pdTRUE)
					{
						if(up)
						{
							if(g_setTemp+5<=75)
								g_setTemp+=5;
							else
								g_setTemp=75;
						}
						else{}
						if(down)
						{
							if(g_setTemp-5>=35)
								g_setTemp-=5;
							else
								g_setTemp=35;
						}
						else{}
						xSemaphoreGive(xSemaphoreSetTemp);
					}
				}
				else
				{
					//increase exit timeout
					count_5sec++;
					if(count_5sec==500)
						{
							count_5sec=0;
							/*critical section of current temp*/
							if(xSemaphoreTake(xSemaphoreCurrentTemp,1)==pdTRUE)
							{
								g_currentTemp = g_setTemp;
								xSemaphoreGive(xSemaphoreCurrentTemp);
							}

							g_SystemState = 1;//operating state
						}
				}
			}
			else{}
			xSemaphoreGive(xSemaphoreState);
		}

		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		}
}

void SSDTASK(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 50;
	SET_BIT(DDRC,6);
	SET_BIT(DDRC,7);
	static uint8 count=0;
	static uint8 seg_state=0;

	for(;;)
	{
		if(xSemaphoreTake(xSemaphoreState,1)==pdTRUE)
		{
			if(g_SystemState==1)//operating state
			{
				TOGGLE_BIT(PORTC,6);
				/*critical section of current temp*/
				if(xSemaphoreTake(xSemaphoreCurrentTemp,1)==pdTRUE)
				{
					SEG_Display_Num(g_currentTemp);
					xSemaphoreGive(xSemaphoreCurrentTemp);
				}
				else{}
			}
			else if(g_SystemState==2)//temp setting
			{
				TOGGLE_BIT(PORTC,7);
				count++;
				if(count%20==0)
				{
					seg_state =0;
					count=0;
				}
				else
					seg_state=1;
				if(seg_state==1)
				{
					/*critical section of current temp*/
					if(xSemaphoreTake(xSemaphoreSetTemp,1)==pdTRUE)
					{
						SEG_Display_Num(g_setTemp);
						xSemaphoreGive(xSemaphoreSetTemp);
					}
					else{}
				}
				else
				{
					SEG_OFF();
				}
			}
			else{}
			xSemaphoreGive(xSemaphoreState);
		}

		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

void TempSensorTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 100;
	SET_BIT(DDRC,4);
	static uint32 temp_sum=0;
	static uint8 itr=0;
	for(;;)
	{
		TOGGLE_BIT(PORTC,4);
//		if(itr<=9)
//		{
//			adc_readValue(2); /* read channel two where the temp sensor is connect */
//			temp_sum += ((uint32)g_adcResult*150*5)/(1023*1.5); /* calculate the temp from the ADC value*/
//			itr++;
//		}
//		if(itr==10)
//		{
//			/*critical section of current temp*/
//			if(xSemaphoreTake(xSemaphoreCurrentTemp,1)==pdTRUE)
//			{
//				g_currentTemp = (uint32)(temp_sum/10);
//				itr=0;
//				xSemaphoreGive(xSemaphoreCurrentTemp);
//			}
//			else{}
//		}

		adc_readValue(2); /* read channel two where the temp sensor is connect */
		/*critical section of current temp*/
		if(xSemaphoreTake(xSemaphoreCurrentTemp,10)==pdTRUE)
		{
			g_currentTemp = ((uint32)g_adcResult*150*5)/(1023*1.5); /* calculate the temp from the ADC value*/;
			xSemaphoreGive(xSemaphoreCurrentTemp);
		}
		else{}

		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
void T6(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 3000;
	SET_BIT(DDRC,5);
	for(;;)
	{
		TOGGLE_BIT(PORTC,5);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
