/*
 * rc_car.c
 *
 *  Created on: Jan 14, 2025
 *      Author: user
 */
#include "ap_def.h"

#define CONTROL_DELAY			10 // ms
#define SPEED_STEP				0.9f

#define MINIMUM_PWM				25.0f	// %
#define MAXYMUM_PWM				90.0f	// %

#define MINIMUM_PS2_ADC		5
#define MAXYMUM_PS2_ADC		250
#define CENTER_PS2_ADC		128

#define SERVO_ANGLE				90.0f		// -90'~90'
#define MOTOR_MAX_PWM			100.0f 	// %

#define LIMIT_INCREASE		4.0f

typedef enum
{
	RC_MOTOR_STOP = 0,
	RC_MOTOR_FRONT = 1,
	RC_MOTOR_BACK = -1,
}RcCarMotorDir_t;

static ps2_data_t rc_controller;

uint32_t ps2_time;

void rcCarInit(void)
{
	ps2_time = millis();
}

void rcCarUpdate(void)
{
	uint8_t dir_val = 0;
	//
	float angle = SERVO_ANGLE / 127.0f;
	float speed = MOTOR_MAX_PWM / (127.0f * LIMIT_INCREASE);

	static float now_speed = 0.0;

	if (millis() - ps2_time >= CONTROL_DELAY)
	{
		ps2_time = millis();
		rc_controller = ps2ReadGamePadData(false, 0);
		if( (rc_controller.raw[1] & 0xF0) == 0x70 )
		{
			if(rc_controller.r_axis_x <= MINIMUM_PS2_ADC)
			{
				rc_controller.r_axis_x = MINIMUM_PS2_ADC;
			}
			if(rc_controller.r_axis_x >= MAXYMUM_PS2_ADC)
			{
				rc_controller.r_axis_x = MAXYMUM_PS2_ADC;
			}
			angle = angle * (float)((int32_t)rc_controller.r_axis_x - CENTER_PS2_ADC);

			if(rc_controller.l_axis_y <= MINIMUM_PS2_ADC)
			{
				rc_controller.l_axis_y = MINIMUM_PS2_ADC;
			}
			if(rc_controller.l_axis_y >= MAXYMUM_PS2_ADC)
			{
				rc_controller.l_axis_y = MAXYMUM_PS2_ADC;
			}
			speed = speed * (float)((int32_t)rc_controller.l_axis_y - CENTER_PS2_ADC);

			if(speed > 0)
			{
				dir_val = RC_MOTOR_FRONT;
			}
			else if(speed < 0)
			{
				dir_val = RC_MOTOR_BACK;
			}
			else
			{
				dir_val = RC_MOTOR_STOP;
			}

			speed = (float)(abs((int16_t)speed));
			if(speed > MAXYMUM_PWM)
			{
				speed = MAXYMUM_PWM;
			}

			if(now_speed < speed)
			{
				if(now_speed <MINIMUM_PWM)
				{
					now_speed = MINIMUM_PWM;
				}
				now_speed = now_speed + SPEED_STEP;
			}
			else if(now_speed >= speed)
			{
				now_speed = speed;
			}

			motorWrite(HW_MOTOR_R, abs((int16_t)now_speed), (int8_t)dir_val);
			motorWrite(HW_MOTOR_L, abs((int16_t)now_speed), (int8_t)dir_val);

			servoSetPos(0,-(int16_t)angle);
			servoSetPos(1,-(int16_t)angle);
		}
	}
}
