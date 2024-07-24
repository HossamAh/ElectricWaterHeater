/*
 * outputs.h
 *
 *  Created on: Mar 16, 2022
 *      Author: Hossam
 */

#ifndef OUTPUTS_H_
#define OUTPUTS_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"
/* Outputs of the systems are
 * LED to indicate the state of the heater/Cooling elements
 * 2 7-seg for displaying the temperature with 5 pins -> 4 pins for data of both 7-segments and 1 for enables
 * 2- motors for Cooling and heater with 2 Enables and two pins for positive inputs of motor */
#define SEVEN_SEG_DIR DDRD
#define SEVEN_SEG_PORT PORTD
#define SEVEN_SEG_DATA_PINS (0x0F)
#define SEVEN_SEG1_DIR DDRE
#define SEVEN_SEG1_PORT PORTE
#define SEVEN_SEG1_DATA_PINS (0x0F)

#define SEVEN_SEG_ENABLE_PIN (4)
#define SEVEN_SEG_ENABLE1_PIN (5)


#define LED_DIR DDRD
#define LED_PORT PORTD
#define LED_PIN (6)

#define MOTOR_DIR DDRB
#define MOTOR_PORT PORTB
#define MOTOR_HEATER_IN_PIN 0
#define MOTOR_HEATER_EN_PIN 1
#define MOTOR_Cooling_IN_PIN 2
#define MOTOR_Cooling_EN_PIN 3

void OUTPUTS_init(void);

/*7-segment displaying functions*/
void SEG_Display_Num( unsigned char temp);
void SEG_OFF(void);

/*LED functions*/
void LED_ON(void);
void LED_OFF(void);
void LED_Toggle(void);

/*MOTORS functions*/
void Heater_ON(void);
void Heater_OFF(void);

void Cooling_ON(void);
void Cooling_OFF(void);

#endif /* OUTPUTS_H_ */
