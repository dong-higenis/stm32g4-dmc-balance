/*
 * pid.h
 *
 *  Created on: Dec 26, 2024
 *      Author: user
 */

#ifndef AP_APPLICATION_PID_H_
#define AP_APPLICATION_PID_H_

#include "ap_def.h"

#define PID_P_GAIN				10.0f
#define PID_I_GAIN 				0.0f
#define PID_D_GAIN 				30.0f

#define PID_T_SAMPLING		5	//ms

#define PID_ISUM_LIMIT		128.0f
#define PID_OUTPUT_LIMIT	255.0f


#define PID_POS_P_GAIN				0.3f
#define PID_POS_I_GAIN 				0.15f
#define PID_POS_D_GAIN 				0.2f
#define PID_POS_ISUM_LIMIT		5.0f
#define PID_POS_OUTPUT_LIMIT	10.0f


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
	float disp_kp;
	float disp_ki;
	float disp_kd;
	float out_min;
	float out_max;
	float out_int_min;
	float out_int_max;
	float output_sum;
	float last_input;
	int32_t p_on;
	int32_t controller_direction;
	uint64_t last_time;
	uint64_t sample_time;
} pid_data_t;


void pidInit(void);
bool pidUpdate(void);
void pidSetting(float* input, float* output, float* setpoint, float in_kp, float in_ki, float in_kd, int32_t in_p_on, int32_t in_controller_direction);

void pidSetModeNew(pid_data_t *in_pid_data, int32_t Mode);
bool pidUpdateNew(pid_data_t *in_pid_data, float in_data, float setpoint_data, float *out_data);
void pidSettingNew(pid_data_t *in_pid_data, float in_kp, float in_ki, float in_kd, int32_t in_p_on, int32_t in_controller_direction);

#endif /* AP_APPLICATION_PID_H_ */
