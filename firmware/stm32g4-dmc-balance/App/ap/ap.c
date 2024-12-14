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
    }
  }
}


void ps2MotorWrite(ps2_state_t * ps2_state)
{
}

