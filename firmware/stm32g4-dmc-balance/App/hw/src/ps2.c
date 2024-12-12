/*
 * ps2.c
 *
 *  Created on: Dec 10, 2024
 *      Author: user
 */
#include "ps2.h"

#ifdef _USE_HW_PS2
#include "include/gpio.h"
#include "cli.h"

extern SPI_HandleTypeDef hspi3;


typedef struct {
  uint8_t header[2];
  uint8_t raw[6];
} ps2_rx_t;

ps2_rx_t * ps2_rx;

#if CLI_USE(HW_PS2)
static void cliPS2(cli_args_t *args);
#endif


void ps2Init(void)
{
  ps2_rx = (ps2_rx_t*)malloc(sizeof(ps2_rx_t));

#if CLI_USE(HW_MOTOR)
  cliAdd("ps2", cliPS2);
#endif

  return;
}


ps2_rx_t * ps2ReadRaw_struct()
{
  uint8_t psx_rx[8] = {0, };
  uint8_t psx_tx[8] = {0x01, 0x42, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00};

  HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET); // CS 활성화
  HAL_SPI_TransmitReceive(&hspi3, psx_tx, psx_rx, 8, 10);
  HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);   // CS 비활성화


  for (int i=0; i<8; i++)
  {
    if (i<2)
    {
      ps2_rx->header[i] = psx_rx[i];
    }
    else
    {
      ps2_rx->raw[i-2] = psx_rx[i];
    }
  }

  return ps2_rx;
}


void ps2ReadState(ps2_state_t * ps2_state)
{
  static uint8_t ps2_cnt = 0;

  ps2_rx_t * ps2_rx = ps2ReadRaw_struct();

  uint8_t left_up    = (ps2_rx->raw[0] == 0x3E) && (ps2_rx->raw[1] == 0xC0) && (ps2_rx->raw[2] == 0xFF);
  uint8_t left_down  = (ps2_rx->raw[0] == 0xFE) && (ps2_rx->raw[1] == 0xFF) && (ps2_rx->raw[2] == 0xFF);
  uint8_t right_up   = (ps2_rx->raw[3] == 0xBF) && (ps2_rx->raw[4] == 0x3F) && (ps2_rx->raw[5] == 0xE0);
  uint8_t right_down = (ps2_rx->raw[3] == 0xFF) && (ps2_rx->raw[4] == 0x3E) && (ps2_rx->raw[5] == 0xE0);

  if (left_up)
  {
    ps2_state->left = 1;
  }
  else if (left_down)
  {
    ps2_state->left = -1;
  }
  else if (right_up)
  {
    ps2_state->right = 1;
  }
  else if (right_down)
  {
    ps2_state->right = -1;
  }
  else
  {
    if (ps2_cnt > 3)
    {
      ps2_state->left = 0;
      ps2_state->right = 0;
      ps2_cnt = 0;
    }
  }
  ps2_cnt += 1;

  return ;
}


#if CLI_USE(HW_PS2)
void cliPS2(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "read"))
  {
    cliPrintf("PS2 read raw data\n");
    uint8_t cnt = 0;

    while(cliKeepLoop())
    {
      ps2_rx_t * ps2_rx = ps2ReadRaw_struct();

      if (cnt > 20)
      {
        cliPrintf("struct> ");
        for (int i=0; i<2; i++)
        {
          cliPrintf("s[%d]: 0x%02X, ", i, ps2_rx->header[i]);
        }
        for (int i=0; i<6; i++)
        {
          cliPrintf("s[%d]: 0x%02X, ", i, ps2_rx->raw[i]);
        }
        cliPrintf("\n");

        cnt = 0;
        cliMoveUp(1);
      }

      cnt += 1;
      delay(10);
    }
    cliMoveDown(1);

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "state"))
  {
    ps2_state_t ps2_state = {0, 0};

    while(cliKeepLoop())
    {
      ps2ReadState(&ps2_state);

      cliMoveUp(1);
      delay(10);
    }
    cliMoveDown(1);

    ret = true;
  }


  if (ret == false)
  {
    cliPrintf("ps2 info\n");
    cliPrintf("ps2 read\n");
    cliPrintf("ps2 state\n");
  }
}

#endif


#endif
