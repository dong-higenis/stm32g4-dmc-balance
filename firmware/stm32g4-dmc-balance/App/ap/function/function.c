/*
 * function.c
 *
 *  Created on: Jan 13, 2025
 *      Author: user
 */
#include "ap_def.h"

static union ConvertData convert_data;

float convertDataArrayToFloat(uint8_t *source)
{
	memcpy(convert_data.uint8, source, sizeof(convert_data.uint8));
	return convert_data.floatsingle;
}

float convertDataArrayToUint32(uint8_t *source)
{
	memcpy(convert_data.uint8, source, sizeof(convert_data.uint8));
	return convert_data.uint32;
}

float convertDataArrayToInt32(uint8_t *source)
{
	memcpy(convert_data.uint8, source, sizeof(convert_data.uint8));
	return convert_data.int32;
}
