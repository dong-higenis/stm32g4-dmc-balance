/*
 * bluetooth.c
 *
 *  Created on: Jan 10, 2025
 *      Author: user
 */
#include "ap_def.h"

#define BT_RX_DATA_MAX	8

uint8_t rx_flow_stage = 0;
uint8_t rx_cmd = 0;
uint8_t rx_len = 0;
uint8_t rx_type = 0;
uint8_t rx_data_index = 0;
uint8_t check_sum = 0;

static void cliBluetooth(cli_args_t *args);

void btInit(void)
{
  cliAdd("bt", cliBluetooth);
}

void btUpdate(void)
{
	uint8_t rx_data;
	static uint8_t rx_data_array[BT_RX_DATA_MAX];

  if (uartAvailable(1) > 0)
	{
  	rx_data = uartRead(1);

		switch(rx_flow_stage)
		{
			case 0: // sync
				if(rx_data == 0x5A)
				{
					memset(rx_data_array, 0x00, sizeof(rx_data_array));
					rx_cmd = 0;
					rx_len = 0;
					rx_type = 0;
					check_sum = 0;
					rx_data_index = 0;
					rx_flow_stage++;
				}
				break;

			case 1: // cmd
				rx_cmd = rx_data;
				check_sum = rx_data;
				rx_flow_stage++;
				break;

			case 2: // len
				rx_len = rx_data;
				check_sum ^= rx_data;
				rx_flow_stage++;
				break;

			case 3: // type
				rx_type = rx_data;
				check_sum ^= rx_data;
				rx_flow_stage++;
				break;

			case 4: // data
				rx_data_array[rx_data_index] = rx_data;
				check_sum ^= rx_data;
				rx_data_index++;
				if(rx_data_index == rx_len)
				{
					rx_flow_stage++;
				}
				break;

			case 5: // crc
				if(check_sum == rx_data)
				{
					btRxDataProcess(rx_cmd, rx_len, rx_type, rx_data_array);
					rx_flow_stage = 0;
				}
				else
				{
					logPrintf("[BT] CRC Fail : packet:%d, cal:%d\n",check_sum, rx_data);
					rx_flow_stage = 0;
				}
				break;
			default:
				rx_flow_stage = 0;
				break;
		}
	}
}

void btRxDataProcess(uint8_t cmd, uint8_t len, uint8_t type, uint8_t *data_array)
{
	float tmp_fp = 0.0f;

	switch(cmd)
	{
		case 0x01:
			logPrintf("[BT]	PID_TYPE_ANGLE setPdata\n");
			tmp_fp = convertDataArrayToFloat(data_array);
			setPdata(PID_TYPE_ANGLE,tmp_fp);
			break;

		case 0x02:
			logPrintf("[BT]	PID_TYPE_ANGLE setIdata\n");
			tmp_fp = convertDataArrayToFloat(data_array);
			setIdata(PID_TYPE_ANGLE,tmp_fp);
			break;

		case 0x03:
			logPrintf("[BT]	PID_TYPE_ANGLE setDdata\n");
			tmp_fp = convertDataArrayToFloat(data_array);
			setDdata(PID_TYPE_ANGLE,tmp_fp);
			break;

		case 0x04:
			logPrintf("[BT]	PID_TYPE_SPEED setPdata\n");
			tmp_fp = convertDataArrayToFloat(data_array);
			setPdata(PID_TYPE_SPEED,tmp_fp);
			break;

		case 0x05:
			logPrintf("[BT]	PID_TYPE_SPEED setIdata\n");
			tmp_fp = convertDataArrayToFloat(data_array);
			setIdata(PID_TYPE_SPEED,tmp_fp);
			break;

		case 0x06:
			logPrintf("[BT]	PID_TYPE_SPEED setDdata\n");
			tmp_fp = convertDataArrayToFloat(data_array);
			setDdata(PID_TYPE_SPEED,tmp_fp);
			break;

		case 0x07://	angle I-sum setting
			tmp_fp = convertDataArrayToFloat(data_array);
			setIsumData(PID_TYPE_ANGLE,tmp_fp);
			break;

		case 0x08://	speed I-sum setting
			tmp_fp = convertDataArrayToFloat(data_array);
			setIsumData(PID_TYPE_SPEED,tmp_fp);
			break;

		case 0x09://	angle output limit setting
			tmp_fp = convertDataArrayToFloat(data_array);
			setOutputLimit(PID_TYPE_ANGLE,tmp_fp);
			break;

		case 0x0A://	speed output limit setting
			tmp_fp = convertDataArrayToFloat(data_array);
			setOutputLimit(PID_TYPE_SPEED,tmp_fp);
			break;

		case 0x0B:
			tmp_fp = convertDataArrayToFloat(data_array);
			setPitchAngleOffset(tmp_fp);
			break;

		case 0x0C:

			break;

		case 0xFF:// Moving
			if(data_array[0]==0)
			{
				setPitchAngleOffset(0.0f);
			}
			else if(data_array[0]==1)
			{
				setPitchAngleOffset(5.0f);
			}
			else if(data_array[0]==2)
			{
				setPitchAngleOffset(-5.0f);
			}
			break;
	}
	logPrintf("[BT]		tmp_fp : %f\n", tmp_fp);
}

void cliBluetooth(cli_args_t *args)
{
	bool ret = false;

  if (args->argc == 2 && args->isStr(0, "test"))
  {
    uint8_t uart_ch;

    uart_ch = constrain(args->getData(1), 1, UART_MAX_CH) - 1;

    if (uart_ch != cliGetPort())
    {
      uint8_t rx_data;

      while(1)
      {
        if (uartAvailable(uart_ch) > 0)
        {
          rx_data = uartRead(uart_ch);
          cliPrintf("<- _DEF_UART%d RX : 0x%X\n", uart_ch + 1, rx_data);
        }

        if (cliAvailable() > 0)
        {
          rx_data = cliRead();
          if (rx_data == 'q')
          {
            break;
          }
          else
          {
            uartWrite(uart_ch, &rx_data, 1);
            cliPrintf("-> _DEF_UART%d TX : 0x%X\n", uart_ch + 1, rx_data);
          }
        }
      }
    }
    else
    {
      cliPrintf("This is cliPort\n");
    }
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("bt info\n");
  }
}
