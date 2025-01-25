/*
 * pid.h
 *
 *  Created on: Dec 26, 2024
 *      Author: user
 */

#ifndef AP_APPLICATION_PID_H_
#define AP_APPLICATION_PID_H_

#include "ap_def.h"

#define PID_T_SAMPLING				10	//ms

#define PID_P_GAIN						10.5f
#define PID_I_GAIN 						0.6f
#define PID_D_GAIN 						10.0f
#define PID_ISUM_LIMIT				100.0f
#define PID_OUTPUT_LIMIT			200.0f

#define PID_POS_P_GAIN				-0.0015f
#define PID_POS_I_GAIN 				0.0001f
#define PID_POS_D_GAIN 				-0.0004f
#define PID_POS_ISUM_LIMIT		0.1f
#define PID_POS_OUTPUT_LIMIT	10.0f

#define PID_MTR_P_GAIN				1.0f
#define PID_MTR_I_GAIN 				1.0f
#define PID_MTR_D_GAIN 				1.0f
#define PID_MTR_ISUM_LIMIT		50.0f
#define PID_MTR_OUTPUT_LIMIT	255.0f

#define AUTOMATIC	1
#define MANUAL	0
#define DIRECT  0
#define REVERSE  1
#define P_ON_M 0
#define P_ON_E 1

typedef struct
{
	bool p_one;
	bool in_auto;
	float kp;
	float ki;
	float kd;
	float error;
	float d_input;
	float disp_kp;
	float disp_ki;
	float disp_kd;
	float out_min;
	float out_max;
	float output_sum;
	float last_input;
	float out_int_min;
	float out_int_max;
	int32_t p_on;
	int32_t controller_direction;
	uint64_t last_time;
	uint64_t sample_time;
} pid_data_t;

void pidSetMode(pid_data_t *in_pid_data, int32_t Mode);
bool pidUpdate(pid_data_t *in_pid_data, float in_data, float setpoint_data, float *out_data);
void pidSetting(pid_data_t *in_pid_data, float in_kp, float in_ki, float in_kd, float isum_limit, float out_limit, int32_t in_p_on, int32_t in_controller_direction);
void pidInitialize(pid_data_t *in_pid_data);
void pidSetcontrollerDirection(pid_data_t *in_pid_data,int32_t in_direction);
void pidSetTunings(pid_data_t *in_pid_data,float in_kp, float in_ki, float in_kd, int32_t in_p_on);
void pidSetOutputLimits(pid_data_t *in_pid_data, float in_min, float in_max);

#endif /* AP_APPLICATION_PID_H_ */
