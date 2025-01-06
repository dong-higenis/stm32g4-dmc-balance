/*
 * balance.c
 *
 *  Created on: Dec 10, 2024
 *      Author: user
 */
#include "ap_def.h"
#include "encoder.h"

#define IMU_STABILIZATION_TIME (100) // 15s

static imu_info_t imu_data_info;

uint32_t imu_timer = 0;
uint32_t log_timer = 0;

bool imu_init_ok_flag = false;

balance_data_t balance_data[IMU_DATA_DIMENSIONS_COUNT];

pid_data_t position_balance;

float pitch_offset = -0.25f;

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

	float setpoint = 0.0f;
	pidSetting( &imu_data_info.pitch, &balance_data[DATA_TYPE_PITCH].pid_out, &setpoint, PID_P_GAIN, PID_I_GAIN, PID_D_GAIN, P_ON_E, DIRECT);

	pidSettingNew(&position_balance, PID_POS_P_GAIN, PID_POS_I_GAIN, PID_POS_D_GAIN, P_ON_E, DIRECT);
  pidSetModeNew(&position_balance, AUTOMATIC);

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
float imu_target_pitch = 0.0f;
void balanceController(void *arg)
{
	int8_t ret = 0;
	static int32_t pre_encoder_data = 0;
	int32_t cur_encoder_data = 0;
	int32_t encoder_speed = 0;
	if(imu_init_ok_flag)
	{
		float motor_control_data = 0.0f;

		static uint8_t cnt = 0;

		int32_t dir = 0;

		if(cnt %1 == 0)
		{
			cur_encoder_data = encoderGetCount(HW_MOTOR_R);
			encoder_speed = cur_encoder_data - pre_encoder_data;
			pre_encoder_data = cur_encoder_data;
		}
		cnt++;
		ret = pidUpdateNew(&position_balance, -(float)encoder_speed, 0.0f, &imu_target_pitch);

		if(imuGetInfo(&imu_data_info))
		{
			imu_data_info.pitch = -(imu_target_pitch - imu_data_info.pitch);

			pidUpdate();

			motor_control_data = balance_data[DATA_TYPE_PITCH].pid_out + ( imu_data_info.gy * 0.3f);

			dir = (int32_t)motor_control_data;
			if(dir > 0 )
			{
				dir = 1;
			}
			else if(dir < 0 )
			{
				dir = -1;
			}
			motorWrite(HW_MOTOR_R, abs((int16_t)motor_control_data), (int8_t)dir);
			motorWrite(HW_MOTOR_L, abs((int16_t)motor_control_data), (int8_t)dir);
		}
	}
}

balance_data_t balanceGetPidGain(DataType_t data_type)
{
	return balance_data[data_type];
}

void cliBalance(cli_args_t *args)
{

  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
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
  		position_balance.kp = args->getFloat(2);
  	}
  	if( args->isStr(1, "i_gain"))
  	{
  		position_balance.ki = args->getFloat(2);
  	}
  	if( args->isStr(1, "d_gain"))
  	{
  		position_balance.kd = args->getFloat(2);
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

