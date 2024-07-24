/*
 * Inputs.h
 *
 *  Created on: Mar 18, 2022
 *      Author: Hossam
 */

#ifndef INPUTS_H_
#define INPUTS_H_

#include "common_macros.h"
#include "std_types.h"
#include "micro_config.h"

#define INPUTS_PORT_DIR DDRA
#define INPUTS_PORT_PIN PINA
#define ON_OFFButton_Pin (0)
#define UPButton_Pin (1)
#define DownButton_Pin (2)


void Inputs_init(void);

bool Read_ON_OFF(void);
bool Read_Up(void);
bool Read_Down(void);



#endif /* INPUTS_H_ */
