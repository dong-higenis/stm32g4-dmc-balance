/*
 * bluetooth.h
 *
 *  Created on: Jan 10, 2025
 *      Author: user
 */

#ifndef AP_COMMUNICATION_BLUETOOTH_H_
#define AP_COMMUNICATION_BLUETOOTH_H_

void btInit(void);
void btUpdate(void);
void btRxDataProcess(uint8_t cmd, uint8_t len, uint8_t type, uint8_t *data_array);

#endif /* AP_COMMUNICATION_BLUETOOTH_H_ */
