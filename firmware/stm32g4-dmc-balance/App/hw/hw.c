/*
 * hw.c
 *
 *  Created on: Dec 4, 2024
 *      Author: user
 */

#include "hw.h"

bool hwInit(void)
{
  // 하드웨어 초기화 함수
  cliInit();
  logInit();
  ledInit();
  i2cInit();
  uartInit();
  gpioInit();
  buttonInit();

  canInit();
  pwmInit();
  motorInit();
  encoderInit();

  //for (int i=0; i<HW_UART_MAX_CH; i++)
  //{
  //  uartOpen(i, 115200);
  //}
  uartOpen(HW_UART_CH_DEBUG, 115200);
  uartOpen(HW_UART_CH_2, 38400);

  logOpen(HW_UART_CH_DEBUG, 115200);
  logPrintf("\r\n[ Firmware Begin... ]\r\n");
  logPrintf("Booting..Name \t\t: %s\r\n", _DEF_BOARD_NAME);
  logPrintf("Booting..Ver  \t\t: %s\r\n", _DEF_FIRMWATRE_VERSION);
  logPrintf("Booting..Clock\t\t: %d Mhz\r\n",
            (int)HAL_RCC_GetSysClockFreq()/1000000);
  logPrintf("\n");

  adcInit();
  imuInit();
  servoInit();
  ps2Init();

  return true;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void delay(uint32_t ms)
{
  HAL_Delay(ms);
}

void delayUs(uint32_t us) 
{
    uint32_t startTick = micros();
    while ((micros() - startTick) < us);
}

uint32_t millis(void)
{
  return HAL_GetTick();
}

uint32_t micros(void)
{
  uint32_t       m0  = millis();
  __IO uint32_t  u0  = SysTick->VAL;
  uint32_t       m1  = millis();
  __IO uint32_t  u1  = SysTick->VAL;
  const uint32_t tms = SysTick->LOAD + 1;

  if (m1 != m0)
  {
    return (m1 * 1000 + ((tms - u1) * 1000) / tms);
  }
  else
  {
    return (m0 * 1000 + ((tms - u0) * 1000) / tms);
  }
}
