/*
 * servo.c
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */
#include "servo.h"
#include "cli.h"

#ifdef _USE_HW_SERVO

#include <math.h>

#define ANGLE_0		750
#define ANGLE_M90	250
#define ANGLE_90	1250


typedef struct
{
	uint32_t servo_duty;
	int32_t now_servo_angle;
	int16_t step_angle;
	int16_t step_val;
	int16_t target_val;
	int16_t move_count;
	
	bool action_command;
	bool start_move_flag;
} servo_info_t;

//typedef struct
//{
//	uint16_t max_value;
//	uint16_t duty;
//	uint32_t channel;
//	uint32_t step;
//} servo_pwm_tbl_t;

#if CLI_USE(HW_SERVO)
void cliServo(cli_args_t *args);
#endif

servo_info_t servo_info[HW_SERVO_MAX_CH];

//static servo_pwm_tbl_t servo_pwm_tbl[2];

bool servo_init = false;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim17;

bool servoInit(void)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
	servo_init = true;


#if CLI_USE(HW_SERVO)
	cliAdd("servo", cliServo);
#endif
	return true;
}

void servoBegin(void)
{
	servoSetPos(0, 0);
	servoSetPos(1, 0);
}

int16_t servoGetPos(uint8_t ch)
{
	if(ch >= HW_SERVO_MAX_CH)
	{
		return 0;
	}
	return servo_info[ch].now_servo_angle;
}

void servoSetPos(uint8_t ch, int16_t angle)
{
	if(ch >= HW_SERVO_MAX_CH)
	{
		return;
	}
	if(angle > 90 || angle < -90)
	{
		cliPrintf("Out of Range [%d]\n", angle);
		return;
	}
	servo_info[ch].now_servo_angle = angle;
	//float temp_servo_duty_per = (5.0 + ((float)angle * 0.0278));	//	1ms~2ms version
	float temp_servo_duty_per = (2.5 + ((float)(angle + 90)* 0.0555556));	// 0.5ms~2.5ms version
	float temp_servo_duty = ( temp_servo_duty_per  / 100.0 ) * 10000.0;
	servo_info[ch].servo_duty = (uint32_t)temp_servo_duty;

	//cliPrintf("target angle = %d\n", (int)angle);
	//cliPrintf("temp_servo_duty_per = %d\n", (int)temp_servo_duty_per);
	//cliPrintf("temp_servo_duty     = %lf\n", temp_servo_duty);

	switch(ch)
	{
	case _DEF_PWM1:
		htim17.Instance->CCR1 = servo_info[ch].servo_duty;
		break;
	case _DEF_PWM2:
		htim1.Instance->CCR3 = servo_info[ch].servo_duty;
		break;
	}
}

uint32_t calAngleToRegval(int16_t angle)
{
	float temp_servo_duty_per = (2.5 + ((float)(angle + 90) * 0.0555556)); // 0.5ms~2.5ms version
	float temp_servo_duty = ( temp_servo_duty_per  / 100.0 ) * 10000.0;
	return (uint32_t)temp_servo_duty;
}

int16_t calRegvalToAngle(uint32_t register_data)
{
    float temp_servo_duty_per = ((float)register_data / 10000.0) * 100.0;
    int16_t angle = (int16_t)((temp_servo_duty_per - 2.5) / 0.0555556) - 90;
    return angle;
}

//	Main Thread Operation task
void servoSetContinue(void)
{
  // not use.
}

#if CLI_USE(HW_SERVO)
void cliServo(cli_args_t *args)
{
	bool ret = false;
	uint8_t	ch;
	uint8_t speed;
	int32_t angle;
	

	if (args->argc == 1 && args->isStr(0, "info") == true)
	{		
		for(uint8_t i=0;i<HW_SERVO_MAX_CH;i++)
		{
			cliPrintf("%d - %d'(duty: %d)\n", i, servo_info[i].now_servo_angle, servo_info[i].servo_duty);
		}
		ret = true;
	}

	if (args->argc == 4 && args->isStr(0, "set") == true)
	{
		if(args->isStr(1, "pos") == true)
		{
			ch  = (uint8_t)args->getData(2);
			angle = (int32_t)args->getData(3);
		
			cliPrintf("set pos ch=%d, angle=%d\n", ch, angle);
			servoSetPos(ch, angle);
			ret = true;
		}
	}

	if (args->argc == 3 && args->isStr(0, "get") == true)
	{
		if(args->isStr(1, "pos") == true)
		{
			ch  = (uint8_t)args->getData(2);
			if(ch < HW_SERVO_MAX_CH)
			{				
				cliPrintf("pos ch=%d, angle=%d\n", ch, servo_info[ch].now_servo_angle);						
				ret = true;
			}
		}
	}
	//	servo continue <ch> <speed> <angle>
	if (args->argc == 4 && args->isStr(0, "continue"))
	{
		ch  = (uint8_t)args->getData(1);
		speed = (uint8_t)args->getData(2);
		angle = (int32_t)args->getData(3);

		if(ch < HW_SERVO_MAX_CH)
		{
			servo_info[ch].step_val = speed;
			servo_info[ch].target_val = angle;
			servo_info[ch].action_command = true;
			servo_info[ch].start_move_flag = true;
			servo_info[ch].step_angle = calAngleToRegval(speed);
			cliPrintf("set pos ch=%d, angle=%d, speed=%d\n", ch, angle, speed);		
		}
		ret = true;
	}

	if (ret == false)
	{
    cliPrintf("servo info\n");
		cliPrintf("servo set pos [ch] [angle]\n");
		cliPrintf("servo get pos [ch]\n");
		cliPrintf("servo continue [ch] [speed] [angle]\n");
	}
}
#endif

#endif
