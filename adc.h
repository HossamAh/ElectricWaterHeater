 /******************************************************************************
 *
 * Module: ADC driver
 *
 * File Name: adc.h
 *
 * Description: ADC driver header file
 *
 * Created on: Jan 21, 2022
 *
 * Author: Hossam
 *
 ******************************************************************************/

#ifndef ADC_H_
#define ADC_H_

/*----------------------------------------INCLUDES-------------------------------------------*/
#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"


/*------------------------------Extern module shared global variables------------------------------------*/
extern volatile uint16 g_adcResult ;/*Variable to store the digital value of ADC reading*/

/*----------------------------------------FUNCTION DECLERATIONS-------------------------------------------*/

/*----------------------------------------------------------------------------------
 * Function Name: adc_init
 * Description: initialize ADC driver
 -------------------------------------------------------------------------------------*/
void adc_init(void);

/*----------------------------------------------------------------------------------
 * Function Name: adc_readValue
 * Description: function to assign the global variable g_adcResult to the
 * digital value of analog voltage on ADC pin.
 * Args:
 * in: uint8 channelNum:
 * 		number of the channel to convert the voltage on.
------------------------------------------------------------------------------------*/
void adc_readValue(uint8 a_channelNum);
#endif /* ADC_H_ */
