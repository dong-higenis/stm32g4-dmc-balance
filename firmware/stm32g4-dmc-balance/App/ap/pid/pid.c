/*
 * pid.c
 *
 *  Created on: Dec 26, 2024
 *      Author: user
 */
#include "ap_def.h"


pid_data_t pid_data;

float *my_input;
float *my_output;
float *my_setpoint;

void pidSetMode(int32_t Mode);

void pidInitialize(void);
void pidSetSampleTime(int32_t NewSampleTime);
void pidSetOutputLimits(float Min, float Max);
void pidSetcontrollerDirection(int32_t Direction);
void pidSetTunings(float Kp, float Ki, float Kd, int32_t POn);

//	추후 다른 제어기도 추가 하기 위한 함수
void pidInitializeNew(pid_data_t *in_pid_data);
void pidSetcontrollerDirectionNew(pid_data_t *in_pid_data,int32_t in_direction);
void pidSetTuningsNew(pid_data_t *in_pid_data,float in_kp, float in_ki, float in_kd, int32_t in_p_on);
void pidSetOutputLimitsNew(pid_data_t *in_pid_data, float in_min, float in_max);

#ifdef _USE_CLI_SW_PID
static void cliPid(cli_args_t *args);
#endif

void pidInit(void)
{
#ifdef _USE_CLI_SW_PID
  cliAdd("pid", cliPid);
#endif
  pidSetMode(AUTOMATIC);
}

bool pidUpdate(void)
{
	if(!pid_data.in_auto)
		return false;

	uint64_t now = millis();
	uint64_t time_change = (now - pid_data.last_time);
	if(time_change >= pid_data.sample_time)
	{
		float output;
		float input = *my_input;
		float error = *my_setpoint - input;
		float d_input = (input - pid_data.last_input);
		pid_data.output_sum += (pid_data.ki * error);

		if(!pid_data.p_one)
			pid_data.output_sum-= pid_data.kp * d_input;

		if(pid_data.output_sum > pid_data.out_int_max)
			pid_data.output_sum= pid_data.out_int_max;
		else if(pid_data.output_sum < pid_data.out_int_min)
			pid_data.output_sum= pid_data.out_int_min;

		if(pid_data.p_one)
			output = pid_data.kp * error;
		else
			output = 0;

		output += pid_data.output_sum - pid_data.kd * d_input;

		if(output > pid_data.out_max)
			output = pid_data.out_max;
		else if(output < pid_data.out_min)
			output = pid_data.out_min;

		*my_output = -output;

		pid_data.last_input = input;
		pid_data.last_time = now;
		return true;
	}
	else
	{
		return false;
	}
}

void pidSetTunings(float in_kp, float in_ki, float in_kd, int32_t in_p_on)
{
	if (in_kp<0 || in_ki<0 || in_kd<0)
	{
		return;
	}

	pid_data.p_on = in_p_on;
	pid_data.p_one = in_p_on == P_ON_E;

	pid_data.disp_kp = in_kp;
	pid_data.disp_ki = in_ki;
	pid_data.disp_kd = in_kd;

	pid_data.kp = in_kp;
	pid_data.ki = in_ki;
	pid_data.kd = in_kd;

	if(pid_data.controller_direction == REVERSE)
	{
		pid_data.kp = (0 - pid_data.kp);
		pid_data.ki = (0 - pid_data.ki);
		pid_data.kd = (0 - pid_data.kd);
	}
}

void pidSetSampleTime(int32_t new_sample_time)
{
	if (new_sample_time > 0)
	{
		float ratio  = (float)new_sample_time / (float)pid_data.sample_time;
		pid_data.ki *= ratio;
		pid_data.kd /= ratio;
		pid_data.sample_time = (uint64_t)new_sample_time;
	}
}

void pidSetOutputLimits(float in_min, float in_max)
{
	if(in_min >= in_max)
	{
		return;
	}

	pid_data.out_min = in_min;
	pid_data.out_max = in_max;

	if(pid_data.in_auto)
	{
		if(*my_output > pid_data.out_max)
		{
			*my_output = pid_data.out_max;
		}
		else if(*my_output < pid_data.out_min)
		{
			*my_output = pid_data.out_min;
		}

		if(pid_data.output_sum > pid_data.out_max)
		{
			pid_data.output_sum = pid_data.out_max;
		}
		else if(pid_data.output_sum < pid_data.out_min)
		{
			pid_data.output_sum = pid_data.out_min;
		}
	}
}

void pidSetMode(int32_t Mode)
{
	bool new_auto = (Mode == AUTOMATIC);
	if(new_auto && !pid_data.in_auto)
	{
		pidInitialize();
	}
	pid_data.in_auto = new_auto;
}

void pidInitialize(void)
{
	pid_data.output_sum = *my_output;
	pid_data.last_input = *my_input;
	if(pid_data.output_sum > pid_data.out_max)
	{
		pid_data.output_sum = pid_data.out_max;
	}
	else if(pid_data.output_sum < pid_data.out_min)
	{
		pid_data.output_sum = pid_data.out_min;
	}
}

void pidSetcontrollerDirection(int32_t in_direction)
{
	if(pid_data.in_auto && in_direction != pid_data.controller_direction)
	{
		pid_data.kp = (0 - pid_data.kp);
		pid_data.ki = (0 - pid_data.ki);
		pid_data.kd = (0 - pid_data.kd);
	}
	pid_data.controller_direction = in_direction;
}

void pidSetting(float* input, float* output, float* setpoint, float in_kp, float in_ki, float in_kd, int32_t in_p_on, int32_t in_controller_direction)
{
	pid_data.out_int_min = -PID_ISUM_LIMIT;
	pid_data.out_int_max = PID_ISUM_LIMIT;

	my_output = output;
	my_input = input;
	my_setpoint = setpoint;

	pid_data.in_auto = false;
	pidSetOutputLimits( -PID_OUTPUT_LIMIT, PID_OUTPUT_LIMIT);
	pid_data.sample_time = PID_T_SAMPLING;
	pidSetcontrollerDirection(in_controller_direction);
	pidSetTunings(in_kp, in_ki, in_kd, in_p_on);
	pid_data.last_time = millis() - pid_data.sample_time;
}

//
//
//

bool pidUpdateNew(pid_data_t *in_pid_data, float in_data, float setpoint_data, float *out_data)
{
	float output;

	//if(!in_pid_data->in_auto)
	//	return false;

	uint32_t now = millis();
	uint32_t time_change = (now - in_pid_data->last_time);
	if(time_change >= in_pid_data->sample_time)
	{
		float input = in_data;
		float error = setpoint_data - input;
		float d_input = (input - in_pid_data->last_input);
		in_pid_data->output_sum += (in_pid_data->ki * error);

		if(!in_pid_data->p_one)
			in_pid_data->output_sum-= in_pid_data->kp * d_input;
#if 1
		if(in_pid_data->output_sum > in_pid_data->out_int_max)
			in_pid_data->output_sum= in_pid_data->out_int_max;
		else if(in_pid_data->output_sum < in_pid_data->out_int_min)
			in_pid_data->output_sum= in_pid_data->out_int_min;
#endif
		if(in_pid_data->p_one)
			output = in_pid_data->kp * error;
		else
			output = 0;

		output += in_pid_data->output_sum - in_pid_data->kd * d_input;
#if 1
		if(output > in_pid_data->out_max)
			output = in_pid_data->out_max;
		else if(output < in_pid_data->out_min)
			output = in_pid_data->out_min;
#endif
		*out_data = -output;

		in_pid_data->last_input = input;
		in_pid_data->last_time = now;
		return true;
	}
	else
	{
		return false;
	}
}

void pidSettingNew(pid_data_t *in_pid_data, float in_kp, float in_ki, float in_kd, int32_t in_p_on, int32_t in_controller_direction)
{
	in_pid_data->out_int_min = -PID_POS_ISUM_LIMIT;
	in_pid_data->out_int_max = PID_POS_ISUM_LIMIT;

	in_pid_data->in_auto = false;
	in_pid_data->sample_time = PID_T_SAMPLING*4;
	in_pid_data->last_time = millis() - in_pid_data->sample_time;

	pidSetOutputLimitsNew(in_pid_data, -PID_POS_OUTPUT_LIMIT, PID_POS_OUTPUT_LIMIT);
	pidSetcontrollerDirectionNew(in_pid_data,in_controller_direction);
	pidSetTuningsNew(in_pid_data,in_kp, in_ki, in_kd, in_p_on);
}

void pidSetcontrollerDirectionNew(pid_data_t *in_pid_data,int32_t in_direction)
{
	if(in_pid_data->in_auto && in_direction != in_pid_data->controller_direction)
	{
		in_pid_data->kp = (0 - in_pid_data->kp);
		in_pid_data->ki = (0 - in_pid_data->ki);
		in_pid_data->kd = (0 - in_pid_data->kd);
	}
	in_pid_data->controller_direction = in_direction;
}

void pidSetTuningsNew(pid_data_t *in_pid_data, float in_kp, float in_ki, float in_kd, int32_t in_p_on)
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

void pidSetOutputLimitsNew(pid_data_t *in_pid_data, float in_min, float in_max)
{
	if(in_min >= in_max)
	{
		return;
	}
	in_pid_data->out_min = in_min;
	in_pid_data->out_max = in_max;
}

void pidSetModeNew(pid_data_t *in_pid_data, int32_t Mode)
{
	bool new_auto = (Mode == AUTOMATIC);
	if(new_auto && !in_pid_data->in_auto)
	{
		pidInitializeNew(in_pid_data);
	}
	in_pid_data->in_auto = new_auto;
}

void pidInitializeNew(pid_data_t *in_pid_data)
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
//
//

//	추후 balance에 동작 시키도록
#ifdef _USE_CLI_SW_PID
void cliPid(cli_args_t *args)
{
	bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
  	ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "set"))
  {
  	if( args->isStr(1, "p_gain"))
  	{
  		pid_data.kp = args->getFloat(2);
  	}
  	if( args->isStr(1, "i_gain"))
  	{
  		pid_data.ki = args->getFloat(2);
  	}
  	if( args->isStr(1, "d_gain"))
  	{
  		pid_data.kd = args->getFloat(2);
  	}

  	if( args->isStr(1, "out_limit"))
  	{
  		pid_data.out_max = args->getFloat(2);
  		pid_data.out_min = -(args->getFloat(2));
  	}

  	if( args->isStr(1, "intg_limit"))
  	{
  		pid_data.out_int_max = args->getFloat(2);
  		pid_data.out_int_min = -(args->getFloat(2));
  	}

  	ret = true;
  }

  if (args->argc == 5 && args->isStr(0, "set"))
  {
  	if( args->isStr(1, "all"))
  	{
  		pid_data.kp = args->getFloat(2);
  		pid_data.ki = args->getFloat(3);
  		pid_data.kd = args->getFloat(4);
  	}
  	ret = true;
  }

  if (ret == false)
  {
    cliPrintf("pid info\n");
    cliPrintf("pid set p_gain <gain>\n");
    cliPrintf("pid set i_gain <gain>\n");
    cliPrintf("pid set d_gain <gain>\n");
    cliPrintf("pid set out_limit <output limit>\n");
    cliPrintf("pid set intg_limit <integral limit>\n");
    cliPrintf("pid set all <p gain> <i gain> <d gain>\n");
  }

}

#endif

