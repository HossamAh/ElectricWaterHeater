/*
 * Inputs.c
 *
 *  Created on: Mar 18, 2022
 *      Author: Hossam
 */


#include "Inputs.h"

void Inputs_init(void)
{
	INPUTS_PORT_DIR &= ~(0x07);
}
bool Read_ON_OFF(void)
{
	bool check = 0;
	if(BIT_IS_SET(INPUTS_PORT_PIN,ON_OFFButton_Pin))
	{
		_delay_us(50);
		if(BIT_IS_SET(INPUTS_PORT_PIN,ON_OFFButton_Pin))
		{
			check =1;
		}
		else{}
		while(BIT_IS_SET(INPUTS_PORT_PIN,ON_OFFButton_Pin));
	}
	return check;
}
bool Read_Up(void)
{
	bool check = 0;
	if(BIT_IS_SET(INPUTS_PORT_PIN,UPButton_Pin))
	{
		_delay_us(50);
		if(BIT_IS_SET(INPUTS_PORT_PIN,UPButton_Pin))
		{
			check =1;
		}
		else{}
		while(BIT_IS_SET(INPUTS_PORT_PIN,UPButton_Pin));
	}
	return check;
}
bool Read_Down(void)
{
	bool check = 0;
	if(BIT_IS_SET(INPUTS_PORT_PIN,DownButton_Pin))
	{
		_delay_us(50);
		if(BIT_IS_SET(INPUTS_PORT_PIN,DownButton_Pin))
		{
			check =1;
		}
		else{}
		while(BIT_IS_SET(INPUTS_PORT_PIN,DownButton_Pin));
	}
	return check;
}
