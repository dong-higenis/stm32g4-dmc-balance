/*
 * function.h
 *
 *  Created on: Jan 13, 2025
 *      Author: user
 */

#ifndef AP_FUNCTION_FUNCTION_H_
#define AP_FUNCTION_FUNCTION_H_

#include "ap_def.h"

union ConvertData
{
	uint32_t uint32;
	int32_t int32;
	uint8_t uint8[4];
	float floatsingle;
};

float convertDataArrayToFloat(uint8_t *source);
float convertDataArrayToUint32(uint8_t *source);
float convertDataArrayToInt32(uint8_t *source);

#endif /* AP_FUNCTION_FUNCTION_H_ */
