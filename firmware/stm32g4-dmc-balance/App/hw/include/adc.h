/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hw_def.h"

#define ADC_MAX_CH    HW_ADC_MAX_CH

void adcInit(void);
int32_t adcRead(uint8_t ch);
float adcConvVoltage(uint8_t ch, uint32_t adc_value);
float adcReadVoltage(uint8_t ch);



#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

