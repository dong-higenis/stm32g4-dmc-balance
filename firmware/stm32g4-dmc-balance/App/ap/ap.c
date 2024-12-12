/*
 * ap.c
 *
 *  Created on: Dec 4, 2024
 *      Author: user
 */

#include "ap.h"

void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);
  logBoot(true);
}

void apMain(void)
{
  uint32_t led_pre_time;
  led_pre_time = millis();

  uint32_t ps2_pre_time;
  ps2_pre_time = millis();

  ps2_state_t ps2_state = {0, 0};

	while(1)
	{
	  if (millis() - led_pre_time >=  500)
	  {
	    led_pre_time = millis();
	    ledToggle(_DEF_LED1);
	  }

	  imuUpdate();
	  cliMain();

    if (millis() - ps2_pre_time >= 5)
    {
      ps2_pre_time = millis();
      ps2MotorWrite(&ps2_state);
    }
  }
}


void ps2MotorWrite(ps2_state_t * ps2_state)
{
  ps2ReadState(ps2_state);

  if (ps2_state->left != 0)
  {
    motorWrite(0, 50, ps2_state->left);
  } else
  {
    motorWrite(0, 0, 0);
  }

  if (ps2_state->right != 0)
  {
    motorWrite(1, 50, ps2_state->right);
  } else
  {
    motorWrite(1, 0, 0);
  }
}

