/*
 * balance.c
 *
 *  Created on: Dec 10, 2024
 *      Author: user
 */
#include "ap_def.h"
#include "encoder.h"

#define IMU_STABILIZATION_TIME 	100 // 0.1s
#define IMU_PITCH_OFFSET				2.55f

balance_data_t balance_data[IMU_DATA_DIMENSIONS_COUNT];

pid_data_t speed_balance;
pid_data_t angle_balance;
pid_data_t motor_balance;

static imu_info_t imu_data_info;

bool imu_init_ok_flag = false;

uint32_t imu_timer = 0;
uint32_t log_timer = 0;

float imu_target_pitch = 0.0f;
float pitch_offset = IMU_PITCH_OFFSET;

#ifdef _USE_CLI_SW_PID
static void cliBalance(cli_args_t *args);
#endif

void balanceInit(void)
{
#ifdef _USE_CLI_SW_PID
  cliAdd("balance", cliBalance);
#endif

	for(int i = 0; i < IMU_DATA_DIMENSIONS_COUNT; i++)
	{
		balance_data[i].avg_count = 0;
		balance_data[i].avg_index = 0;
		balance_data[i].avg_is_ready = false;
	}

  swtimer_handle_t timer_ch;
  timer_ch = swtimerGetHandle();
  if (timer_ch >= 0)
  {
    logPrintf("[OK] aTiltInit()\n     swtimerGetHandle()->ch[%d]\n",timer_ch);
  	swtimerSet(timer_ch, PID_T_SAMPLING, LOOP_TIME, balanceController, NULL);	// 10ms
  	swtimerStart(timer_ch);
  }
  else
  {
    logPrintf("[NG] aTiltInit()\n     swtimerGetHandle()\n");
  }

	pidSetting(&angle_balance, PID_P_GAIN, PID_I_GAIN, PID_D_GAIN, PID_ISUM_LIMIT, PID_OUTPUT_LIMIT, P_ON_E, DIRECT);
  pidSetMode(&angle_balance, AUTOMATIC);

	pidSetting(&speed_balance, PID_POS_P_GAIN, PID_POS_I_GAIN, PID_POS_D_GAIN, PID_POS_ISUM_LIMIT, PID_POS_OUTPUT_LIMIT, P_ON_E, DIRECT);
  pidSetMode(&speed_balance, AUTOMATIC);

	pidSetting(&motor_balance, PID_MTR_P_GAIN, PID_MTR_I_GAIN, PID_MTR_D_GAIN, PID_MTR_ISUM_LIMIT, PID_MTR_OUTPUT_LIMIT, P_ON_E, DIRECT);
  pidSetMode(&motor_balance, AUTOMATIC);

  log_timer = millis ();
  imu_timer = millis ();
}

void balanceUpdate(void)
{
	if(imu_init_ok_flag == false)
	{
		if (millis () - imu_timer >= IMU_STABILIZATION_TIME)
		{
			imu_init_ok_flag = true;
		}
	}
}

void balanceController(void *arg)
{
	float motor_control_data[2] = {0.0f,0.0f};
	static int32_t pre_encoder_data[2] = {0,0};
	int32_t cur_encoder_data[2] = {0,0};
	int32_t encoder_speed[2] = {0,0};

	if(imu_init_ok_flag)
	{
		int8_t dir = 0;

		cur_encoder_data[HW_MOTOR_R] = encoderGetCount(HW_MOTOR_R);
		encoder_speed[HW_MOTOR_R] = cur_encoder_data[HW_MOTOR_R] - pre_encoder_data[HW_MOTOR_R];
		pre_encoder_data[HW_MOTOR_R] = cur_encoder_data[HW_MOTOR_R];

		cur_encoder_data[HW_MOTOR_L] = encoderGetCount(HW_MOTOR_L);
		encoder_speed[HW_MOTOR_L] = cur_encoder_data[HW_MOTOR_L] - pre_encoder_data[HW_MOTOR_L];
		pre_encoder_data[HW_MOTOR_L] = cur_encoder_data[HW_MOTOR_L];

		pidUpdate(&speed_balance, (float)encoder_speed[HW_MOTOR_R], 0.0f, &imu_target_pitch);

		if(imuGetInfo(&imu_data_info))
		{
			imu_data_info.pitch = -(imu_target_pitch - imu_data_info.pitch);

			pidUpdate(&angle_balance, imu_data_info.pitch, pitch_offset, &balance_data[DATA_TYPE_PITCH].pid_out);

			motor_control_data[HW_MOTOR_R] = balance_data[DATA_TYPE_PITCH].pid_out;

			if(motor_control_data[HW_MOTOR_R] > 0.0f )
			{
				dir = 1;
			}
			else if(motor_control_data[HW_MOTOR_R] < 0.0f )
			{
				dir = -1;
			}

			motorWrite(HW_MOTOR_R, abs((int16_t)motor_control_data[HW_MOTOR_R]), dir);
			motorWrite(HW_MOTOR_L, abs((int16_t)motor_control_data[HW_MOTOR_R]), dir);

			//cliPrintf("[balance]output_sum:%3.2f\t last_input:%3.2f\t pid_out:%3.2f error:%3.2f\t d_input:%3.2f\t pitch:%3.2f\n",
			//					angle_balance.output_sum, angle_balance.last_input, balance_data[DATA_TYPE_PITCH].pid_out
			//					, angle_balance.error, angle_balance.d_input, imu_data_info.pitch
			//					);
		}
	}
}

balance_data_t balanceGetPidGain(DataType_t data_type)
{
	return balance_data[data_type];
}

pid_data_t getPIDdata(uint8_t ch)
{
	pid_data_t ret = {0x00,};
	switch(ch)
	{
		case PID_TYPE_ANGLE:
			memcpy(&ret, &angle_balance, sizeof( pid_data_t ));
			break;
		case PID_TYPE_SPEED:
			memcpy(&ret, &speed_balance, sizeof( pid_data_t ));
			break;
	}
	return ret;
}

void setPIDdata(pid_data_t *target, float gain_p, float gain_i, float gain_d)
{
	target->disp_kp = gain_p;
	target->disp_ki = gain_i;
	target->disp_kd = gain_d;
}

void setPdata(uint8_t ch, float gain)
{
	switch(ch)
	{
		case PID_TYPE_ANGLE:
			angle_balance.kp = gain;
			angle_balance.output_sum = 0.0f;
			break;
		case PID_TYPE_SPEED:
			speed_balance.kp = gain;
			speed_balance.output_sum = 0.0f;
			break;
	}
}

void setIdata(uint8_t ch, float gain)
{
	switch(ch)
	{
		case PID_TYPE_ANGLE:
			angle_balance.ki = gain;
			angle_balance.output_sum = 0.0f;
			break;
		case PID_TYPE_SPEED:
			speed_balance.ki = gain;
			speed_balance.output_sum = 0.0f;
			break;
	}
}

void setDdata(uint8_t ch, float gain)
{
	switch(ch)
	{
		case PID_TYPE_ANGLE:
			angle_balance.kd = gain;
			angle_balance.output_sum = 0.0f;
			break;
		case PID_TYPE_SPEED:
			speed_balance.kd = gain;
			speed_balance.output_sum = 0.0f;
			break;
	}
}

void setIsumData(uint8_t ch, float gain)
{
	switch(ch)
	{
		case PID_TYPE_ANGLE:
			angle_balance.out_int_min = -gain;
			angle_balance.out_int_max = gain;
			angle_balance.output_sum = 0.0f;
			break;
		case PID_TYPE_SPEED:
			speed_balance.out_int_min = -gain;
			speed_balance.out_int_max = gain;
			speed_balance.output_sum = 0.0f;
			break;
	}
}

void setOutputLimit(uint8_t ch, float gain)
{
	switch(ch)
	{
		case PID_TYPE_ANGLE:
			angle_balance.out_min = -gain;
			angle_balance.out_max = gain;
			angle_balance.output_sum = 0.0f;
			break;
		case PID_TYPE_SPEED:
			speed_balance.out_min = -gain;
			speed_balance.out_max = gain;
			speed_balance.output_sum = 0.0f;
			break;
	}
}

void setPitchAngleOffset(float gain)
{
	pitch_offset = gain;
}

//
//
//

void cliBalance(cli_args_t *args)
{

  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
  	//while(1)
  		cliPrintf("[balance]angle:%3.2f,\t speed:%3.2f\n",angle_balance.output_sum,speed_balance.output_sum);
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "offset"))
  {
  		if(args->isStr(1, "pitch"))
  		{
  				pitch_offset = args->getFloat(2);
  		}
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "set_pitch"))
  {
  	if( args->isStr(1, "p_gain"))
  	{
  		speed_balance.kp = args->getFloat(2);
  	}
  	if( args->isStr(1, "i_gain"))
  	{
  		speed_balance.ki = args->getFloat(2);
  	}
  	if( args->isStr(1, "d_gain"))
  	{
  		speed_balance.kd = args->getFloat(2);
  	}

  	ret = true;
  }

  if (ret == false)
  {
      cliPrintf("balance info\n");
      cliPrintf("balance set_pitch p_gain <p gain>\n");
      cliPrintf("balance set_pitch i_gain <i gain>\n");
      cliPrintf("balance set_pitch d_gain <d gain>\n");
      cliPrintf("balance offset pitch <val>\n");
  }
}

