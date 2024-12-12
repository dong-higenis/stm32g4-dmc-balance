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


void ps2Init(void);
uint8_t * ps2Read(void);
void ps2ReadState(ps2_state_t * ps2_state);



#endif

#ifdef __cplusplus
}
#endif

#endif /* HW_INCLUDE_PS2_H_ */
