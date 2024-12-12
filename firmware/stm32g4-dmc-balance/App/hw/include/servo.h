/*
 * servo.h
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */

#ifndef COMMON_HW_INCLUDE_SERVO_H_
#define COMMON_HW_INCLUDE_SERVO_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_SERVO

bool servoInit(void);
void servoBegin(void);
void servoSetPos(uint8_t ch, int16_t angle);
int16_t servoGetPos(uint8_t ch);
void servoSetContinue(void);
uint32_t calAngleToRegval(int16_t angle);
int16_t calRegvalToAngle(uint32_t register_data);


#endif

#ifdef __cplusplus
}
#endif

#endif /* COMMON_HW_INCLUDE_SERVO_H_ */
