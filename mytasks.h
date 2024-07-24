/*
 * mytasks.h
 *
 * Created: 7/5/2011 23:45:37
 *  Author: MMM
 */ 


#ifndef MYTASKS_H_
#define MYTASKS_H_
#include "semphr.h"
#include "outputs.h"
#include "adc.h"
#include "Inputs.h"

extern xSemaphoreHandle CURRENT_TEMP_SEMPH;
extern unsigned char g_blinkFlagTemp;
extern unsigned char g_currentTemp;
extern unsigned char g_acumlatedTemp;
extern unsigned char g_setTemp;
extern unsigned char g_tempSettingModeFlag;
extern unsigned char g_Heater_state;
extern unsigned char g_Cooler_state;

extern xTaskHandle g_offTaskHandle;
extern xTaskHandle g_ONTaskHandle;
extern xTaskHandle g_SETTEMPTaskHandle;
extern xTaskHandle g_DisplayTempTaskHandle;
extern xTaskHandle g_senseTempTaskHandle;
extern xTaskHandle g_LEDTaskHandle;
extern xTaskHandle g_SettingModeTaskHandle;
extern xTaskHandle g_SenseTempTaskHandle;
extern xTaskHandle g_LEDTaskHandle;
extern xTaskHandle g_BlinkTaskHandle;

extern unsigned char g_offTaskEnable;
extern unsigned char g_ONTaskEnable;
extern unsigned char g_SETTEMPTaskEnable;
extern unsigned char g_DisplayTempTaskEnable;
extern unsigned char g_senseTempTaskEnable;
extern unsigned char g_LEDTaskEnable;
extern unsigned char g_SettingModeTaskEnable;
extern unsigned char g_SenseTempTaskEnable;
extern unsigned char g_LEDTaskEnable;
extern unsigned char g_BlinkTaskEnable;


void vSenseTempTask(void *pvParameters);
void vOnTask(void *pvParameters);
void vOFFTask( void *pvParameters );
void vSettingTempTask( void *pvParameters );
void vDisplayTempTask( void *pvParameters );
void vLedTask(void *pvParameters);
void vINCDECTempTask( void *pvParameters );
void vBlinkSEGTask( void *pvParameters );


void T1(void *pvParameters);
void T2(void *pvParameters);
#endif /* MYTASKS_H_ */
