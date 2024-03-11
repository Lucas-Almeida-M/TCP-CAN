/*
 * can_app.h
 *
 *  Created on: Jan 13, 2024
 *      Author: lucas
 */

#ifndef INC_CAN_APP_H_
#define INC_CAN_APP_H_

#include "can.h"
#include "Parse.h"
#include "stdbool.h"

#define MAX_SENSORS 8
#define MAX_DEVICES 10
#define HEADER_ID   2

typedef struct _device
{
	uint8_t deviceNum;
	bool deviceSync;
	uint8_t activeSensorNumber;
	uint8_t sensorUpdated;
	uint32_t sensorData[8];

}device;

//typedef struct _DeviceState
//{
//	bool devices[10]; // HABILITAR DE ACORDO COM device
//	uint8_t deviceCount;
//}DeviceState;

int send_sync_request(void);




#endif /* INC_CAN_APP_H_ */
