/*
 * hw.h
 *
 *  Created on: Dec 4, 2024
 *      Author: user
 */

#ifndef HW_HW_H_
#define HW_HW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#include "led.h"
#include "uart.h"
#include "cli.h"
#include "log.h"
#include "include/i2c.h"
#include "include/gpio.h"
#include "swtimer.h"
#include "button.h"
#include "can.h"
#include "pwm.h"
#include "motor.h"
#include "encoder.h"
#include "servo.h"
#include "imu.h"

#include "ps2.h"

bool hwInit(void);


#ifdef __cplusplus
}
#endif

#endif /* HW_HW_H_ */
