/*
 * motor.h
 *
 *  Created on: Nov 20, 2024
 *      Author: user
 */

#ifndef HW_INCLUDE_MOTOR_H_
#define HW_INCLUDE_MOTOR_H_


#include "hw_def.h"

#ifdef _USE_HW_MOTOR

#define MOTOR_MAX_CH    HW_MOTOR_MAX_CH


bool motorInit(void);
void motorWrite(int8_t ch, int16_t pwm_data, int8_t dir);

#endif


#endif /* HW_INCLUDE_MOTOR_H_ */
