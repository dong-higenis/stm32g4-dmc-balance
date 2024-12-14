/*
 * ps2.c
 *
 *  Created on: Dec 10, 2024
 *      Author: user
 *  아두이노 ps2x_lib 코드 참조
 *  PS2 SPI 통신은 100kbps ~ 200kbps 에서 동작되므로 이 드라이버를 그대로 쓰려면 CPU Clock을 48Mhz대로 낮춰야 함
 */
#include "ps2.h"

#ifdef _USE_HW_PS2
#include "include/gpio.h"
#include "cli.h"

//#define PS2X_DEBUG

extern SPI_HandleTypeDef hspi3;

#define SPI_TIMEOUT (10)
typedef struct
{
  SPI_HandleTypeDef *p_hspi;
  uint8_t controller_type;
} ps2_info_t;

typedef struct
{
  uint8_t l_2 : 1;
  uint8_t r_2 : 1;
  uint8_t l_1 : 1;
  uint8_t r_1 : 1;
  uint8_t up : 1;
  uint8_t right : 1;
  uint8_t down : 1;
  uint8_t left : 1;  
} ps2_button_t;

typedef struct __packed
{
  union
  {
    uint8_t raw[18];
    struct
    {
      uint8_t header[3];
      ps2_button_t l_button;
      ps2_button_t r_button;
      uint8_t r_axis_x;
      uint8_t r_axis_y;
      uint8_t l_axis_x;
      uint8_t l_axis_y;
      uint8_t ext[12];
    };
  };
} ps2_data_t;

static ps2_data_t ps2_data;
static ps2_info_t info;

#if CLI_USE(HW_PS2)
static void cliPS2(cli_args_t *args);
#endif

void ps2ReConfigGamepad(void);

void ps2Init(void)
{
  info.p_hspi = &hspi3;
#if CLI_USE(HW_MOTOR)
  cliAdd("ps2", cliPS2);
#endif
  return;
}

void ps2SetSpiCS(bool high)
{
  gpioPinWrite(HW_GPIO_CH_SPI_CS, high ? _DEF_HIGH : _DEF_LOW);
}

uint8_t ps2SendCommand(uint8_t *tx, uint8_t *rx, uint8_t len)
{
  ps2SetSpiCS(false);
  HAL_SPI_TransmitReceive(info.p_hspi, tx, rx, len, SPI_TIMEOUT);
  ps2SetSpiCS(true);
  return true;
}

void ps2EnterConfig()
{
  const uint8_t cmd[5] = {0x01, 0x43, 0x00, 0x01, 0x00};
  uint8_t resp[5];
  ps2SendCommand((uint8_t *)cmd, resp, 5);
  delayUs(10);
}

void ps2ExitConfig()
{
  const uint8_t cmd[9] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
  uint8_t resp[9];
  ps2SendCommand((uint8_t *)cmd, resp, 9);
  delayUs(10);
}

void ps2SetMode()
{
  const uint8_t cmd[] = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
  uint8_t resp[9];
  ps2SendCommand((uint8_t *)cmd, resp, 9);
}

bool ps2ReadGamePad(uint8_t motor1, uint8_t motor2)
{
  uint8_t cmd[9] = {0x01, 0x42, 0, 0, 0, 0, 0, 0, 0};
  uint8_t ext_cmd[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  static uint32_t last_read_tm = 0;

  if (millis() - last_read_tm > 1500)
  {
    // reconfig
    ps2ReConfigGamepad();
  }

  if (motor2 > 0)
  {
    motor2 = map(motor2, 0, 255, 0x40, 0xFF);
  }
  cmd[3] = motor1;
  cmd[4] = motor2;

  for (uint8_t i = 0; i < 5; i++)
  {
    ps2SetSpiCS(false);
    HAL_SPI_TransmitReceive(info.p_hspi, cmd, ps2_data.raw, 9, SPI_TIMEOUT);
    if (ps2_data.raw[1] == 0x79)
    {
      HAL_SPI_TransmitReceive(info.p_hspi, ext_cmd, &ps2_data.raw[9], 12, SPI_TIMEOUT);
    }
    ps2SetSpiCS(true);

    if ((ps2_data.raw[1] & 0xF0) == 0x70)
    {
      break;
    }
    ps2ReConfigGamepad();
    delay(1);
  }
  last_read_tm = millis();
  // buttons = ((cmd_resp[4] << 8) | cmd_resp[3]);
  return ((ps2_data.raw[1] & 0xF0) == 0x70);
}

void ps2ReConfigGamepad(void)
{
  ps2EnterConfig();
  ps2SetMode();
  ps2ExitConfig();
}

uint8_t ps2ConfigGamepad()
{
  const uint8_t type_read[9] = {0x01, 0x45, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
  uint8_t resp[9];

  ps2ReadGamePad(false, 0);
  ps2ReadGamePad(false, 0);

  if (ps2_data.raw[1] != 0x41 && ps2_data.raw[1] != 0x73 && ps2_data.raw[1] != 0x79)
  {
#ifdef PS2X_DEBUG
    cliPrintf("Controller mode not matched or no controller found\n");
    cliPrintf("Expected 0x41, 0x73 or 0x79, but got data[1]: %02x\n", ps2_data.raw[1]);
#endif
    return 1; // return error code 1
  }

  for (int i = 0; i <= 10; i++)
  {
    ps2EnterConfig();
    delay(4);

    ps2SetSpiCS(false);
    HAL_SPI_TransmitReceive(info.p_hspi, type_read, resp, 9, SPI_TIMEOUT);
    ps2SetSpiCS(true);

    info.controller_type = resp[3];
    ps2SetMode();
    ps2ExitConfig();

    ps2ReadGamePad(false, 0);

    if (ps2_data.raw[1] == 0x73)
    {
      break;
    }

    if (i == 10)
    {
#ifdef PS2X_DEBUG
      cliPrintf("Controller not accepting commands\n");
      cliPrintf("mode stil set at data[1]: %02X\n", ps2_data.raw[1]);
#endif
      return 2; // exit function with error
    }
  }
  return 0;
}

#if CLI_USE(HW_PS2)
void cliPS2(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "config"))
  {
    uint8_t error = ps2ConfigGamepad();
    cliPrintf("ps2 Configuration : %d\n", error);
    ret = true;
  }
  if (args->argc == 1 && args->isStr(0, "reconfig"))
  {
    ps2ReConfigGamepad();
    cliPrintf("ps2 reConfiguration\n");
    ret = true;
  }
  if (args->argc == 1 && args->isStr(0, "read"))
  {
    while (cliKeepLoop())
    {
      if (ps2ReadGamePad(false, 0))
      {
        cliPrintf("%x %x %x %x %x %x %x %x %x %x %x %x %d %d %d %d                         ", ps2_data.l_button.up, ps2_data.l_button.down, ps2_data.l_button.left, ps2_data.l_button.right,
                  ps2_data.r_button.up, ps2_data.r_button.down, ps2_data.r_button.left, ps2_data.r_button.right,
                  ps2_data.r_button.l_1, ps2_data.r_button.l_2, ps2_data.r_button.r_1, ps2_data.r_button.r_2,
                  ps2_data.l_axis_x, ps2_data.l_axis_y, ps2_data.r_axis_x, ps2_data.r_axis_y);
        cliPrintf("\n");
        cliMoveUp(1);
      }
      else
      {
#ifdef PS2X_DEBUG
        cliPrintf("read failed");
        for (uint8_t i = 0; i < 9; i++)
        {
          cliPrintf(" %02X ", ps2_data.raw[i]);
        }
        cliPrintf("\n");
        cliMoveUp(1);
#endif
      }
      delay(10);
    }
    // cliMoveDown(1);

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "raw"))
  {
    while (cliKeepLoop())
    {
      if (ps2ReadGamePad(false, 0))
      {
        for (uint8_t i = 0; i < 9; i++)
        {
          cliPrintf(" %02X ", ps2_data.raw[i]);
        }
        cliPrintf("\n");
        cliMoveUp(1);
      }
      else
      {
#ifdef PS2X_DEBUG
        cliPrintf("read failed");
        for (uint8_t i = 0; i < 9; i++)
        {
          cliPrintf(" %02X ", ps2_data.raw[i]);
        }
        cliPrintf("\n");
#endif
      }
      delay(10);
    }
    // cliMoveDown(1);

    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("ps2 info\n");
    cliPrintf("ps2 raw\n");
    cliPrintf("ps2 read\n");
    cliPrintf("ps2 config\n");
    cliPrintf("ps2 reconfig\n");
  }
}

#endif

#endif
