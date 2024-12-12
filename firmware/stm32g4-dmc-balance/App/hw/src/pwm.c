#include "pwm.h"


#ifdef _USE_HW_PWM
#include "cli.h"

#define NAME_DEF(x)  x, #x


typedef struct
{
  TIM_HandleTypeDef *p_htim;
  uint16_t           max_value;
  PwmPinName_t       pin_name;
  const char        *p_name;
} pwm_tbl_t;


#if CLI_USE(HW_PWM)
static void cliPwm(cli_args_t *args);
#endif

static bool is_init = false;


extern TIM_HandleTypeDef htim3;

/*
  TIM3_CH3  MT_L_IN_PWM  PB0
  TIM3_CH4  MT_R_IN_PWM  PB1
*/

static uint16_t pwm_duty[PWM_MAX_CH] = {0, };

static const pwm_tbl_t pwm_tbl[PWM_MAX_CH] =
{
    {&htim3, 255, NAME_DEF(mPWM_MT_L_IN)},
    {&htim3, 255, NAME_DEF(mPWM_MT_R_IN)}
};


static bool pwmInitTimer(void);

bool pwmInit(void)
{
  bool ret;

  for (int i=0; i<PWM_MAX_CH; i++)
  {
    assert(i == (int)pwm_tbl[i].pin_name);
  }

  ret = pwmInitTimer();
  is_init = ret;


#if CLI_USE(HW_PWM)
  cliAdd("pwm", cliPwm);
#endif
  return ret;
}

bool pwmInitTimer(void)
{
  bool ret = true;

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

  return ret;
}

bool pwmIsInit(void)
{
  return is_init;
}

void pwmWrite(uint8_t ch, uint16_t pwm_data)
{
  assert(ch < PWM_MAX_CH);
  assert(pwm_data <= pwm_tbl[ch].max_value);

  pwm_duty[ch] = pwm_data;

  switch(ch)
  {
    case _DEF_PWM1:
      htim3.Instance->CCR3 = pwm_duty[ch];
      break;

    case _DEF_PWM2:
      htim3.Instance->CCR4 = pwm_duty[ch];
      break;
  }
}

uint16_t pwmRead(uint8_t ch)
{
  if (ch >= HW_PWM_MAX_CH) return 0;

  return pwm_duty[ch];
}

uint16_t pwmGetMax(uint8_t ch)
{
  if (ch >= HW_PWM_MAX_CH) return 255;

  return pwm_tbl[ch].max_value;
}


#if CLI_USE(HW_PWM)
void cliPwm(cli_args_t *args)
{
  bool ret = false;
  uint8_t  ch;
  uint32_t pwm;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
    for (int i = 0; i < PWM_MAX_CH; i++)
    {
      cliPrintf("%02d. %-32s : %03d/%03d %-2d%%\n",
                i,
                pwm_tbl[i].p_name,
                pwm_duty[i],
                pwm_tbl[i].max_value,
                pwm_duty[i] * 100 / pwm_tbl[i].max_value);
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "set"))
  {
    ch  = (uint8_t)args->getData(1);
    pwm = (uint8_t)args->getData(2);

    ch = constrain(ch, 0, PWM_MAX_CH);

    pwmWrite(ch, pwm);
    cliPrintf("pwm ch%d %d\n", ch, pwm);
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "get"))
  {
    ch = (uint8_t)args->getData(1);

    cliPrintf("pwm ch%d %d\n", ch, pwmRead(ch));
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("pwm info\n");
    cliPrintf("pwm set ch [0-%d] [0~100] \n", PWM_MAX_CH-1);
    cliPrintf("pwm get ch [0-%d] \n", PWM_MAX_CH-1);
  }

}
#endif

#endif
