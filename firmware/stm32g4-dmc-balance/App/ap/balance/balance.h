/*
 * balance.h
 *
 *  Created on: Dec 10, 2024
 *      Author: user
 */

#ifndef AP_APPLICATION_BALANCE_H_
#define AP_APPLICATION_BALANCE_H_

#include "ap_def.h"

#define IMU_DATA_DIMENSIONS_COUNT	2

typedef enum
{
	P_GAIN = 0,
	I_GAIN = 1,
	D_GAIN = 2,
	INTEGRAL_LIMIT = 3,
	OUTPUT_LIMIT = 4
} PidGainType_t;

typedef enum
{
	AVG_TYPE_ROLL = 0,
	AVG_TYPE_PITCH = 1
} AverageDataType_t;

typedef enum
{
	DATA_TYPE_ROLL = 0,
	DATA_TYPE_PITCH = 1
} DataType_t;

typedef enum
{
	PID_TYPE_ANGLE = 0,
	PID_TYPE_SPEED = 1
} PIDType_t;

typedef struct
{
	bool avg_is_ready;
	float pid_out;
	float	avg_data;
	float imu_data;
	float calculate;
	float calculate_avg;
	uint8_t speed;
	uint8_t direction;
	uint16_t avg_count;
	uint16_t avg_index;
} balance_data_t;

void balanceInit(void);
void balanceUpdate(void);
void balanceController(void *arg);
pid_data_t getPIDdata(uint8_t ch);
void setPIDdata(pid_data_t *target, float gain_p, float gain_i, float gain_d);
balance_data_t balanceGetPidGain(DataType_t data_type);
void setPdata(uint8_t ch, float gain);
void setIdata(uint8_t ch, float gain);
void setDdata(uint8_t ch, float gain);
void setIsumData(uint8_t ch, float gain);
void setOutputLimit(uint8_t ch, float gain);
void setPitchAngleOffset(float gain);


#endif /* AP_APPLICATION_BALANCE_H_ */
