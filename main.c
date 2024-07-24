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

SemaphoreHandle_t xSemaphoreState = NULL;
SemaphoreHandle_t xSemaphoreON_OFF = NULL;
SemaphoreHandle_t xSemaphoreUP_DOWN = NULL;
SemaphoreHandle_t xSemaphoreCurrentTemp = NULL;
SemaphoreHandle_t xSemaphoreSetTemp = NULL;
SemaphoreHandle_t xSemaphoreHeaterCooler = NULL;

uint8 g_currentTemp;
uint8 g_setTemp;
uint8 g_heaterState = 0;
uint8 g_CoolerState = 0;

TaskHandle_t g_offTaskHandle;
TaskHandle_t g_ONTaskHandle;
TaskHandle_t g_SETTEMPTaskHandle;

void OFF_StateTask(void *pvParameters);
void OperatingStateTask(void *pvParameters);
void TempSettingState(void *pvParameters);
void SSDTASK(void *pvParameters);
void TempSensorTask(void *pvParameters);
void HeaterCoolerTask(void *pvParameters);
// #define mainLED_TASK_PRIORITY			( tskIDLE_PRIORITY )
#define OFF_TASK_PRIORITY (3) // 4
uint8 g_SystemState = 0;	  /*
							   *0->off state
							   *1->operating state
							   *2->temp setting state */

uint32 temp[10] = {0};
portSHORT main(void)
{
	//	LCD_init();
	OUTPUTS_init();
	Inputs_init();
	adc_init();

	g_currentTemp = 15;
	g_setTemp = 60;

	xTaskCreate(OFF_StateTask, "ON/OFF", configMINIMAL_STACK_SIZE, NULL, 1, &g_offTaskHandle);
	xTaskCreate(OperatingStateTask, "OP_State", configMINIMAL_STACK_SIZE, NULL, 1, &g_ONTaskHandle);
	xTaskCreate(TempSettingState, "Temp_setting", configMINIMAL_STACK_SIZE, NULL, 1, &g_SETTEMPTaskHandle);
	xTaskCreate(SSDTASK, "SSDTASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(TempSensorTask, "TempSensorTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(HeaterCoolerTask, "HeaterCoolerTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	xSemaphoreON_OFF = xSemaphoreCreateMutex();
	xSemaphoreUP_DOWN = xSemaphoreCreateMutex();
	xSemaphoreCurrentTemp = xSemaphoreCreateMutex();
	xSemaphoreSetTemp = xSemaphoreCreateMutex();
	xSemaphoreState = xSemaphoreCreateMutex();
	xSemaphoreHeaterCooler = xSemaphoreCreateMutex();
	// start scheduler
	vTaskStartScheduler();

	while (1)
	{
	}
	return 0;
}
void OFF_StateTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 30;
	SET_BIT(DDRC, 0);
	for (;;)
	{
		// if (xSemaphoreTake(xSemaphoreState, 1) == pdTRUE)
		// {
        taskENTER_CRITICAL();

			if (g_SystemState == 0)
			{
				TOGGLE_BIT(PORTC, 0);
				SEG_OFF();
				LED_OFF();
				Heater_OFF();
				Cooling_OFF();
				if (Read_ON_OFF())
				{
					/*critical section */
					g_SystemState = 1; // operating state
				}
				else
				{
				}
			}
			else if (g_SystemState == 1 || g_SystemState == 2)
			{
				if (Read_ON_OFF())
				{
					/*critical section */
					g_SystemState = 0; // off state
				}
			}
		// 	xSemaphoreGive(xSemaphoreState);
		// }
taskEXIT_CRITICAL();
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}
void OperatingStateTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 30;
	SET_BIT(DDRC, 1);
	for (;;)
	{
		// if (xSemaphoreTake(xSemaphoreState, 1) == pdTRUE)
		// 	if (xSemaphoreTake(xSemaphoreCurrentTemp, 1) == pdTRUE)
		// 		if (xSemaphoreTake(xSemaphoreHeaterCooler, 1) == pdTRUE)
		// 		{
		taskENTER_CRITICAL();

					if (g_SystemState == 1) // operating state
					{
						TOGGLE_BIT(PORTC, 1);
						if (Read_Up() || Read_Down())
						{
							/*critical section */
							g_SystemState = 2; // Temp Setting state
											   /*critical section of current temp*/

							g_setTemp = g_currentTemp;
						}
						else
						{

							/*compare the current temp and set temp*/
							if (g_currentTemp <= g_setTemp - 5)
							{

								g_heaterState = 1;
								g_CoolerState = 0;
							}
							else if (g_currentTemp >= g_setTemp + 5)
							{

								g_heaterState = 0;
								g_CoolerState = 1;
							}
						}
					}
					else
					{
					}
				// 	xSemaphoreGive(xSemaphoreState);
				// 	xSemaphoreGive(xSemaphoreCurrentTemp);
				// 	xSemaphoreGive(xSemaphoreHeaterCooler);
				// }
		taskEXIT_CRITICAL();
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}
void TempSettingState(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 15;
	static count_5sec = 0; /*to count time to 5 sec 100 cycle to exit*/
	SET_BIT(DDRC, 2);
	for (;;)
	{
		// if (xSemaphoreTake(xSemaphoreState, 1) == pdTRUE)
		// 	if (xSemaphoreTake(xSemaphoreSetTemp, 1) == pdTRUE)
		// 		if (xSemaphoreTake(xSemaphoreCurrentTemp, 1) == pdTRUE)	
		// 		{
		taskENTER_CRITICAL();

					if (g_SystemState == 2) // Temp setting
					{
						TOGGLE_BIT(PORTC, 2);
						uint8 up, down;
						up = Read_Up();
						down = Read_Down();
						if (up || down)
						{
							count_5sec = 0; // reset exit timeout
							/*critical section of current temp*/

							if (up)
							{
								if (g_setTemp + 5 <= 75)
									g_setTemp += 5;
								else
									g_setTemp = 75;
							}
							else
							{
							}
							if (down)
							{
								if (g_setTemp - 5 >= 35)
									g_setTemp -= 5;
								else
									g_setTemp = 35;
							}
							else
							{
							}
						}
						else
						{
							// increase exit timeout
							count_5sec++;
							if (count_5sec == 500)
							{
								count_5sec = 0;
								/*critical section of current temp*/
								g_currentTemp = g_setTemp;

								g_SystemState = 1; // operating state
							}
						}
					}
					else
					{
					}
				// 	xSemaphoreGive(xSemaphoreState);
				// 	xSemaphoreGive(xSemaphoreSetTemp);
				// 	xSemaphoreGive(xSemaphoreCurrentTemp);
				// }
		taskEXIT_CRITICAL();

		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void SSDTASK(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 50;
	SET_BIT(DDRC, 6);
	SET_BIT(DDRC, 7);
	static uint8 count = 0;
	static uint8 seg_state = 0;
	for (;;)
	{
		// if (xSemaphoreTake(xSemaphoreState, 1) == pdTRUE)
		// 	if (xSemaphoreTake(xSemaphoreCurrentTemp, 1) == pdTRUE)
		// 		if (xSemaphoreTake(xSemaphoreSetTemp, 1) == pdTRUE)
		// 		{
		taskENTER_CRITICAL();

					if (g_SystemState == 1) // operating state
					{
						TOGGLE_BIT(PORTC, 6);
						/*critical section of current temp*/
						SEG_Display_Num(g_currentTemp);
					}
					else if (g_SystemState == 2) // temp setting
					{
						TOGGLE_BIT(PORTC, 7);
						count++;
						if (count % 20 == 0)
						{
							seg_state = 0;
							count = 0;
						}
						else
							seg_state = 1;
						
						if (seg_state == 1)
						{
							/*critical section of current temp*/
							SEG_Display_Num(g_setTemp);
						}
						else
						{
							SEG_OFF();
						}
					}
					else
					{
					}
					// xSemaphoreGive(xSemaphoreState);
					// xSemaphoreGive(xSemaphoreCurrentTemp);
					// xSemaphoreGive(xSemaphoreSetTemp);
				// }
		taskEXIT_CRITICAL();
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void TempSensorTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 100;
	SET_BIT(DDRC, 4);
	static uint32 temp_sum = 0;
	static uint8 itr = 1;
	adc_readValue(2);										  /* read channel two where the temp sensor is connect */
	temp[0] = ((uint32)g_adcResult * 150 * 5) / (1023 * 1.5); /* calculate the temp from the ADC value*/
	temp_sum = temp_sum + temp[0];
	/*critical section of current temp*/
	// if (xSemaphoreTake(xSemaphoreCurrentTemp, 1) == pdTRUE)
	// {
		taskENTER_CRITICAL();

		g_currentTemp = (uint32)(temp_sum);
		// xSemaphoreGive(xSemaphoreCurrentTemp);
		taskEXIT_CRITICAL();	
	// }
	for (;;)
	{
		TOGGLE_BIT(PORTC, 4);
		if (itr % 10 != 0)
		{
			adc_readValue(2); /* read channel two where the temp sensor is connect */
			// sub the previous reading in this position and add the new one
			temp_sum = temp_sum - temp[itr % 10];
			temp[itr % 10] = ((uint32)g_adcResult * 150 * 5) / (1023 * 1.5); /* calculate the temp from the ADC value*/
			temp_sum = temp_sum + temp[itr % 10];
		}
		else if (itr % 10 == 0)
		{
			/*critical section of current temp*/
			// if (xSemaphoreTake(xSemaphoreCurrentTemp, 1) == pdTRUE)
			// {
			taskENTER_CRITICAL();

				g_currentTemp = (uint32)(temp_sum / 10);
			
			taskEXIT_CRITICAL();
			// 	xSemaphoreGive(xSemaphoreCurrentTemp);
			// }
			// else
			// {
			// }
		}
		itr++;
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}
void HeaterCoolerTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 1000;
	SET_BIT(DDRC, 5);
	static uint8 count = 0;
	for (;;)
	{
		TOGGLE_BIT(PORTC, 5);
		taskENTER_CRITICAL();
		if(g_heaterState==1 && g_CoolerState==0)
		{
			if(count++%2==0)
			{
				g_currentTemp+=1;
				LED_ON();
			}
			else
			{
				LED_OFF();
			}
		}
		else if(g_heaterState==0 && g_CoolerState==1)
		{
			g_currentTemp-=1;
			LED_ON();
		}
		taskEXIT_CRITICAL();
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}
