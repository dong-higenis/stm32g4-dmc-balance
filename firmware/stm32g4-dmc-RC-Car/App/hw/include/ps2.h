/*
 * ps2.h
 *
 *  Created on: Dec 10, 2024
 *      Author: user
 */

#ifndef HW_INCLUDE_PS2_H_
#define HW_INCLUDE_PS2_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_PS2


typedef struct
{
  int8_t left;
  int8_t right;
} ps2_state_t;

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

void ps2Init(void);
uint8_t * ps2Read(void);
void ps2ReadState(ps2_state_t * ps2_state);
bool ps2ReadGamePad(uint8_t motor1, uint8_t motor2);
ps2_data_t ps2ReadGamePadData(uint8_t motor1, uint8_t motor2);

#endif

#ifdef __cplusplus
}
#endif

#endif /* HW_INCLUDE_PS2_H_ */
