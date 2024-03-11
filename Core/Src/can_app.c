/*
 * can_app.c
 *
 *  Created on: Jan 13, 2024
 *      Author: lucas
 */


#include "can_app.h"
#include "cmsis_os.h"
#include "queue.h"
#include "main.h"
#include "debug_level.h"


uint8_t canRX[8] = {};
uint8_t canTX[8] = {};
extern uint32_t TxMailbox;
extern osMessageQId queue_can_sendHandle;
extern osMessageQId queue_can_receiveHandle;
extern osMessageQId queue_tcp_sendHandle;
extern char tcpmsg[];

uint8_t deviceCount = 0;
device devices[10] = {0};
char CanMsgDebug [64] = {0};

/***
 * @fn void ReceiveCAN_MSG(void*)
 * @brief Task to process received CAN messages
 *
 * @param argument
 */
void ReceiveCAN_MSG(void *argument)
{
  /* USER CODE BEGIN ProcessCAN_MSG */

	for (int i = 0; i < MAX_DEVICES; i++)
	{
		devices[i].deviceNum = i;
	}

	CanPacket canMSG = {0};
	char msg[64] = {0};
  /* Infinite loop */
  for(;;)
  {
	BaseType_t xStatus = xQueueReceive(queue_can_receiveHandle, &canMSG, 0);
	if (xStatus == pdPASS)
	{


 		switch (canMSG.canID)
		{
			case CONFIG:
				if (canMSG.canDataFields.control == 1)
				{
					memset(&devices[canMSG.canDataFields.deviceNum].sensorUpdated, 0 ,sizeof(devices[canMSG.canDataFields.deviceNum].sensorUpdated) );
					memset(&devices[canMSG.canDataFields.deviceNum].sensorData, 0 , sizeof(devices[canMSG.canDataFields.deviceNum].sensorData));
				}
				break;
			case DATA:
				uint8_t j = 0;
				for (int i = 0; i < MAX_SENSORS; i++)
				{
					if (canMSG.canDataFields.control & (1<<i) )
					{
						devices[canMSG.canDataFields.deviceNum].sensorData[i] = ( (uint16_t)canMSG.canDataFields.data[j] << 8) | (uint16_t)(canMSG.canDataFields.data[j+1]) ;
						setbit(&devices[canMSG.canDataFields.deviceNum ].sensorUpdated, i, 1);
						j+=2;
					}

					if (devices[canMSG.canDataFields.deviceNum].activeSensorNumber == devices[canMSG.canDataFields.deviceNum].sensorUpdated)
					{
						buildMessage(canMSG.canID, canMSG.canDataFields.deviceNum, &devices[canMSG.canDataFields.deviceNum], msg);
						xQueueSendToBack(queue_tcp_sendHandle, msg, 0);
						memset(msg, 0, sizeof(msg));
						memset(&devices[canMSG.canDataFields.deviceNum].sensorUpdated, 0 ,sizeof(devices[canMSG.canDataFields.deviceNum ].sensorUpdated) );
						memset(&devices[canMSG.canDataFields.deviceNum].sensorData, 0 , sizeof(devices[canMSG.canDataFields.deviceNum].sensorData));
						break;
					}
				}
				break;
			case SYNC:
				devices[canMSG.canDataFields.deviceNum].deviceSync = true;
				devices[canMSG.canDataFields.deviceNum].activeSensorNumber = canMSG.canDataFields.control;
				osDelay(5);
				break;
		}
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		// conseguiu tirar da fila

	}
    osDelay(1);
  }
  /* USER CODE END ProcessCAN_MSG */
}

/***
 * @fn void SendCAN_MSG(void*)
 * @brief Task to send CAN messages
 *
 * @param argument
 */
void SendCAN_MSG(void *argument)
{
  /* USER CODE BEGIN SendCAN_MSG */
	CanPacket canMSG = {0};
	uint8_t buffer[8] = {0};
  /* Infinite loop */
  for(;;)
  {
	BaseType_t xStatus = xQueueReceive(queue_can_sendHandle, &canMSG, 0);
	if (xStatus == pdPASS)
	{
		// conseguiu tirar da fila

		TxHeader.StdId             = canMSG.canID;
		TxHeader.RTR               = CAN_RTR_DATA;
		TxHeader.IDE               = CAN_ID_STD;
		TxHeader.DLC               = CAN_SIZE;
		TxHeader.TransmitGlobalTime = DISABLE;

		memcpy(buffer, &canMSG.canDataFields, sizeof(buffer));
		int status = HAL_CAN_AddTxMessage (&hcan1, &TxHeader, buffer,  &TxMailbox);
		if(status)
		{
			Error_Handler();
		}

		if (DEBUG_LEVEL > NONE)
		{
			sprintf(CanMsgDebug, "CAN message sent [ID: %d] [MT: %d] \r\n", canMSG.canID, canMSG.canDataFields.deviceNum);
			print_debug (CanMsgDebug);
			memset(CanMsgDebug, 0, sizeof(CanMsgDebug));
		}
	}


    osDelay(1);
  }
  /* USER CODE END SendCAN_MSG */
}




/***
 * @fn void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef*)
 * @brief driver function that receives can messages from CAN FIFO
 *
 * @param hcan
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	CanPacket canPacket = { 0 } ;


	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, canRX);
	canPacket.canID = RxHeader.StdId;
	memcpy(&canPacket.canDataFields, canRX, sizeof(canRX));
	if (DEBUG_LEVEL > NONE)
	{
		sprintf(CanMsgDebug, "CAN message received [ID: %d] [MT: %d] \r\n", canPacket.canID, canPacket.canDataFields.deviceNum);
		print_debug (CanMsgDebug);
		memset(CanMsgDebug, 0, sizeof(CanMsgDebug));
	}
	xQueueSendToBackFromISR(queue_can_receiveHandle, &canPacket, &xHigherPriorityTaskWoken);

//	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

}


int send_sync_request(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	CanPacket canPacket = { 0 } ;
	canPacket.canID = SYNC;
	canPacket.canDataFields.deviceNum = BroadCast;

	xQueueSendToBackFromISR(queue_can_sendHandle, &canPacket, &xHigherPriorityTaskWoken);
	return 0;
}


/***
 * @fn void sendCanMsg_test(int)
 * @brief function to send can message with ID 0 (Broadcast)
 *
 * @param delay
 */
void sendCanMsg_test(int delay)
{
	  uint8_t tx[8] = {0,1,2,3,4,5,6,7};
	  TxHeader.StdId             = SYNC;     // ID do dispositivo
	  TxHeader.RTR               = CAN_RTR_DATA;       //(Remote Transmission Request) especifica Remote Fraame ou Data Frame.
	  TxHeader.IDE               = CAN_ID_STD;    //define o tipo de id (standard ou extended
	  TxHeader.DLC               = 8;      //Tamanho do pacote 0 - 8 bytes
	  TxHeader.TransmitGlobalTime = DISABLE;

	  int status = HAL_CAN_AddTxMessage(&hcan1, &TxHeader, tx, &TxMailbox);
	  if(status)
	  {
		 Error_Handler();
	  }
	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  HAL_Delay(delay);
}
