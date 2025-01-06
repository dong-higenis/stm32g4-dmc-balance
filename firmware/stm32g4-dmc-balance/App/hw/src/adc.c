/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "log.h"
#include "cli.h"

static uint16_t adc_data[HW_ADC_MAX_CH+1];

static bool is_init = false;

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

#ifdef _USE_HW_CLI
static void cliAdc(cli_args_t *args);
#endif

/* ADC1 init function */
void adcInit(void)
{
  bool ret = true;

  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_data[0], HW_ADC_MAX_CH) != HAL_OK)
  {
    ret = false;
  }

  logPrintf("[%s] adcInit()\n", ret ? "OK" : "NG");

#ifdef _USE_HW_CLI
  cliAdd("adc", cliAdc);
#endif
}

int32_t adcRead(uint8_t ch)
{
  assert_param(ch < HW_ADC_MAX_CH);
  return adc_data[ch];
}

float adcConvVoltage(uint8_t ch, uint32_t adc_value)
{
  float ret = 0;
  switch (ch)
  {
    default:
      ret = ((float)adc_value * 3.3f) / (4095.f);
      break;
  }
  return ret;
}

float adcReadVoltage(uint8_t ch)
{
  return adcConvVoltage(ch, adcRead(ch));
}

void cliAdc(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    cliPrintf("adc init : %d\n", is_init);
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "show"))
  {
  	cliPrintf("[0] : %d\n", (int)adcRead(0));
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "show") && args->isStr(1, "vol"))
  {
  	cliPrintf("[0] : %2.2fV \n", adcReadVoltage(0));
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "reset") == true)
  {
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("adc info\n");
    cliPrintf("adc show\n");
    cliPrintf("adc show vol\n");
  }
}



