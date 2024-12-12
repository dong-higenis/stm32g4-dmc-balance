/*
 * motor.c
 *
 *  Created on: Nov 21, 2024
 *      Author: user
 */

#include "motor.h"

#ifdef _USE_HW_MOTOR
#include "pwm.h"
#include "qbuffer.h"
#include "cli.h"
#include "include/gpio.h"

#define NAME_DEF(x)  x, #x


typedef enum
{
  NONE  =  0,
  FRONT =  1,
  REAR  = -1
} MotorDirect_t;

typedef struct
{
  uint16_t pwm_offset;
  uint16_t pwm_max;
  uint16_t pwm_out;
  uint8_t pin_name[3];
} motor_tbl_t;

// const cfg table

motor_tbl_t motor_tbl[MOTOR_MAX_CH] = {
    {
        .pwm_offset = 0,
        .pwm_max    = 255,
        .pwm_out    = 0,
        .pin_name   = {MT_L_IN_1, MT_L_IN_2, mPWM_MT_L_IN},
    },
    {
        .pwm_offset = 0,
        .pwm_max    = 255,
        .pwm_out    = 0,
        .pin_name   = {MT_R_IN_1, MT_R_IN_2, mPWM_MT_R_IN},
    },
};


#if CLI_USE(HW_MOTOR)
static void cliMotor(cli_args_t *args);
#endif


bool motorInit(void)
{
#if CLI_USE(HW_MOTOR)
  cliAdd("motor", cliMotor);
#endif
  return true;
}


void motorWrite(int8_t ch, int16_t pwm_data, int8_t dir)
{
//  assert((100 > pwm_data) || (-100 < pwm_data));
  pwm_data = constrain(pwm_data, 0, 100);
  int16_t pwm_duty = cmap(pwm_data, 0, 100, motor_tbl[ch].pwm_offset, motor_tbl[ch].pwm_max);

  if (pwm_data > 0)
  {
    if (dir == FRONT)
    {
      pwmWrite(motor_tbl[ch].pin_name[2], pwm_duty);
      gpioPinWrite(motor_tbl[ch].pin_name[0], 0);
      gpioPinWrite(motor_tbl[ch].pin_name[1], 1);

    }
    else if (dir == REAR)
    {
      pwmWrite(motor_tbl[ch].pin_name[2], pwm_duty);
      gpioPinWrite(motor_tbl[ch].pin_name[0], 1);
      gpioPinWrite(motor_tbl[ch].pin_name[1], 0);
//      pwmWrite(motor_tbl[ch].pin_name[2], motor_tbl[ch].pwm_out);
    }
    else
    {
      pwmWrite(motor_tbl[ch].pin_name[2], 0);
      gpioPinWrite(motor_tbl[ch].pin_name[0], 0);
      gpioPinWrite(motor_tbl[ch].pin_name[1], 0);
    }

  }
  else
  {
    pwmWrite(motor_tbl[ch].pin_name[2], 0);
    gpioPinWrite(motor_tbl[ch].pin_name[0], 0);
    gpioPinWrite(motor_tbl[ch].pin_name[1], 0);
  }
}

#if CLI_USE(HW_MOTOR)
void cliMotor(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
    ret = true;
  }

  if (args->argc == 4 && args->isStr(0, "move"))
  {
    int8_t ch;
    int8_t speed;
    int8_t dir;

    ch    = (int8_t)args->getData(1);
    speed = (int8_t)args->getData(2);
    dir   = (int8_t)args->getData(3);

    motorWrite(ch, speed, dir);

    while(cliKeepLoop())
    {
      cliPrintf("motor move ch[%d] speed[%d] dir[%d]\n", ch, speed, dir);

      cliMoveUp(1);
      delay(10);
    }
    cliMoveDown(1);

//    motorWrite(ch, 0, 1); // stop

    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("motor info\n");
    cliPrintf("motor move ch[0~3] speed[0~100] dir[-1,1]\n");
  }

}

#endif

#endif
