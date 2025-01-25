/*
 * pid.c
 *
 *  Created on: Dec 26, 2024
 *      Author: user
 */
#include "ap_def.h"

bool pidUpdate(pid_data_t *in_pid_data, float in_data, float setpoint_data, float *out_data)
{
	float output;

	in_pid_data->error = setpoint_data - in_data;
	in_pid_data->d_input = (in_data - in_pid_data->last_input);
	in_pid_data->output_sum += (in_pid_data->ki * in_pid_data->error);

	if(!in_pid_data->p_one)
	{
		in_pid_data->output_sum-= in_pid_data->kp * in_pid_data->d_input;
	}

	if(in_pid_data->output_sum > in_pid_data->out_int_max)
	{
		in_pid_data->output_sum= in_pid_data->out_int_max;
	}
	else if(in_pid_data->output_sum < in_pid_data->out_int_min)
	{
		in_pid_data->output_sum= in_pid_data->out_int_min;
	}

	if(in_pid_data->p_one)
	{
		output = in_pid_data->kp * in_pid_data->error;
	}
	else
	{
		output = 0;
	}

	output += in_pid_data->output_sum - in_pid_data->kd * in_pid_data->d_input;

	if(output > in_pid_data->out_max)
	{
		output = in_pid_data->out_max;
	}
	else if(output < in_pid_data->out_min)
	{
		output = in_pid_data->out_min;
	}

	*out_data = -output;

	in_pid_data->last_input = in_data;

	return true;
}

void pidSetting(pid_data_t *in_pid_data, float in_kp, float in_ki, float in_kd, float isum_limit, float out_limit, int32_t in_p_on, int32_t in_controller_direction)
{
	in_pid_data->out_int_min = -isum_limit;
	in_pid_data->out_int_max = isum_limit;

	in_pid_data->in_auto = false;
	in_pid_data->sample_time = PID_T_SAMPLING;
	in_pid_data->last_time = millis() - in_pid_data->sample_time;

	pidSetOutputLimits(in_pid_data, -out_limit, out_limit);
	pidSetcontrollerDirection(in_pid_data,in_controller_direction);
	pidSetTunings(in_pid_data,in_kp, in_ki, in_kd, in_p_on);
}

void pidSetcontrollerDirection(pid_data_t *in_pid_data,int32_t in_direction)
{
	if(in_pid_data->in_auto && in_direction != in_pid_data->controller_direction)
	{
		in_pid_data->kp = (0 - in_pid_data->kp);
		in_pid_data->ki = (0 - in_pid_data->ki);
		in_pid_data->kd = (0 - in_pid_data->kd);
	}
	in_pid_data->controller_direction = in_direction;
}

void pidSetTunings(pid_data_t *in_pid_data, float in_kp, float in_ki, float in_kd, int32_t in_p_on)
{
	if (in_kp<0 || in_ki<0 || in_kd<0)
	{
		return;
	}

	in_pid_data->p_on = in_p_on;
	in_pid_data->p_one = in_p_on == P_ON_E;

	in_pid_data->disp_kp = in_kp;
	in_pid_data->disp_ki = in_ki;
	in_pid_data->disp_kd = in_kd;

	in_pid_data->kp = in_kp;
	in_pid_data->ki = in_ki;
	in_pid_data->kd = in_kd;

	if(in_pid_data->controller_direction == REVERSE)
	{
		in_pid_data->kp = (0 - in_pid_data->kp);
		in_pid_data->ki = (0 - in_pid_data->ki);
		in_pid_data->kd = (0 - in_pid_data->kd);
	}
}

void pidSetOutputLimits(pid_data_t *in_pid_data, float in_min, float in_max)
{
	if(in_min >= in_max)
	{
		return;
	}
	in_pid_data->out_min = in_min;
	in_pid_data->out_max = in_max;
}

void pidSetMode(pid_data_t *in_pid_data, int32_t Mode)
{
	bool new_auto = (Mode == AUTOMATIC);
	if(new_auto && !in_pid_data->in_auto)
	{
		pidInitialize(in_pid_data);
	}
	in_pid_data->in_auto = new_auto;
}

void pidInitialize(pid_data_t *in_pid_data)
{
	if(in_pid_data->output_sum > in_pid_data->out_max)
	{
		in_pid_data->output_sum = in_pid_data->out_max;
	}
	else if(in_pid_data->output_sum < in_pid_data->out_min)
	{
		in_pid_data->output_sum = in_pid_data->out_min;
	}
}

//
//	File END
//
