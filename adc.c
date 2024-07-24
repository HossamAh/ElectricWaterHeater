 /******************************************************************************
 * Module: ADC driver
 *
 * File Name: adc.c
 *
 * Description: source file for ADC driver.
 *
 * Created on: Jan 21, 2022
 *
 * Author: Hossam
 *
 ******************************************************************************/


/*----------------------------------------INCLUDES-------------------------------------------*/
#include "adc.h"

/*----------------------------------------Global Variables------------------------------------*/
volatile uint16 g_adcResult = 0;/*Variable to store the digital value of ADC reading*/

///*------------------------------------------ISR's Definitions-----------------------------------*/
//
//ISR(ADC_vect)
//{
//	DDRB = 0xff;
//	PORTB = ADC;
//	/* Read ADC register value after conversion complete*/
//	g_adcResult = ADC;
//	SET_BIT(ADCSRA,ADIF); /* clear ADIF by write '1' to it :) */
//}
/*----------------------------------------FUNCTION Definitions-------------------------------*/
void adc_init(void)
{
	/* ADMUX Register Bits Description:
	 * REFS1:0 = 00 to choose to connect external reference voltage by input this voltage through AREF pin
	 * ADLAR   = 0 right adjusted
	 * MUX4:0  = 00000 to choose channel 0 as initialization
	 */
	ADMUX = 0;

	/* ADCSRA Register Bits Description:
	 * ADEN    = 1 Enable ADC
	 * ADIE    = 1 Enable ADC Interrupt
	 * ADPS2:0 = 011 to choose ADC_Clock=F_CPU/8=1Mhz/8=125Khz --> ADC must operate in range 50-200Khz
	 */
//	ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS1) | (1<<ADPS0);
	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);
}

void adc_readValue(uint8 a_channelNum)
{

	/* ADMUX Register Bits Description:
	 * MUX3:0 = a_channelNum to select the ADC channel
	 */
	ADMUX  = (ADMUX & 0xF8) | (a_channelNum&0x07);

	/*ADCSRA Register Bits Description:
	 *ADSC =1  Start Conversion
	 */
	SET_BIT(ADCSRA,ADSC); /* start conversion write '1' to ADSC */
	while(BIT_IS_CLEAR(ADCSRA,ADIF)); /* wait for conversion to complete ADIF becomes '1' */
	SET_BIT(ADCSRA,ADIF); /* clear ADIF by write '1' to it :) */
	g_adcResult = ADC;
}
