/*
 * can_app.c
 *
 *  Created on: Jan 13, 2024
 *      Author: lucas
 */


#include "can_app.h"
//#include "cmsis_os2.h"
#include "cmsis_os.h"
#include "queue.h"
#include "main.h"



int testecountnucleo = 0;
int testecountblack = 0;
uint8_t canRX[8] = {};
uint8_t canTX[8] = {};
extern uint32_t TxMailbox;
extern osMessageQId queue_can_sendHandle;
extern osMessageQId queue_can_receiveHandle;
extern osMessageQId queue_tcp_sendHandle;
extern char tcpmsg[];

uint8_t deviceCount = 0;
device devices[10] = {0};

/***
 * @fn void ReceiveCAN_MSG(void*)
 * @brief Task to process received CAN messages
 *
 * @param argument
 */
void ReceiveCAN_MSG(void *argument)
{
  /* USER CODE BEGIN ProcessCAN_MSG */

	for (int i =0; i < MAX_DEVICES; i++)
	{
		devices[i].id = i+2;
	}

	CanPacket canMSG = {0};
	char msg[64] = {0};
  /* Infinite loop */
  for(;;)
  {
	BaseType_t xStatus = xQueueReceive(queue_can_receiveHandle, &canMSG, 0);
	if (xStatus == pdPASS)
	{
		switch (canMSG.packet.canBuffer.canDataFields.ctrl0.value)
		{
			case CONFIG:

				break;
			case DATA:
				uint8_t j = 0;
				for (int i = 0; i < MAX_SENSORS; i++)
				{
					if (canMSG.packet.canBuffer.canDataFields.ctrl1.bit[i])
					{
						devices[canMSG.packet.canID - 2 ].sensorData[i] = ( (uint16_t)canMSG.packet.canBuffer.canDataFields.data[j] << 8) | (canMSG.packet.canBuffer.canDataFields.data[j+1]) ;
						devices[canMSG.packet.canID - 2 ].sensorUpdated.bit[i] = true;
						j+=2;
					}

					if (devices[canMSG.packet.canID - 2 ].activeSensorNumber.value == devices[canMSG.packet.canID - 2].sensorUpdated.value)
					{
						buildMessage(DATA, &devices[canMSG.packet.canID - 2], msg);
						xQueueSendToBack(queue_tcp_sendHandle, msg, 0);
						memset(msg, 0, sizeof(msg));
						break;
					}
				}
				break;
			case SYNC:
				devices[canMSG.packet.canID - 2].deviceSync = true;
				devices[canMSG.packet.canID - 2].deviceSync = true;
				devices[canMSG.packet.canID - 2].activeSensorNumber.value = canMSG.packet.canBuffer.canDataFields.ctrl1.value;
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
	CanPacket canMsg = {0};
  /* Infinite loop */
  for(;;)
  {
	BaseType_t xStatus = xQueueReceive(queue_can_sendHandle, &canMsg, 0);
	if (xStatus == pdPASS)
	{
		// conseguiu tirar da fila

		TxHeader.StdId             = canMsg.packet.canID;
		TxHeader.RTR               = CAN_RTR_DATA;
		TxHeader.IDE               = CAN_ID_STD;
		TxHeader.DLC               = CAN_SIZE;
		TxHeader.TransmitGlobalTime = DISABLE;
		int status = HAL_CAN_AddTxMessage (&hcan1, &TxHeader, canMsg.packet.canBuffer.canData, &TxMailbox);
		if(status)
		{
			Error_Handler();
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
	canPacket.packet.canID = RxHeader.StdId;
	memcpy(&canPacket.packet.canBuffer, canRX, sizeof(canRX));
	xQueueSendToBackFromISR(queue_can_receiveHandle, &canPacket, &xHigherPriorityTaskWoken);

//	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

}


int send_sync_request(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	CanPacket canPacket = { 0 } ;
	canPacket.packet.canID = BROADCAST;
	canPacket.packet.canBuffer.canDataFields.ctrl0.value = SYNC;


	xQueueSendToBackFromISR(queue_can_sendHandle, &canPacket, &xHigherPriorityTaskWoken);
	return 0;
}

int send_sync_status(void)
{
//	xQueueSendToBackFromISR(queue_can_sendHandle, &canPacket, &xHigherPriorityTaskWoken);
}


/***
 * @fn void sendCanMsg_test(int)
 * @brief function to send can message with ID 0 (Broadcast)
 *
 * @param delay
 */
void sendCanMsg_test(int delay)
{
	  uint8_t tx[7] = {1,2,3,4,5,6,7};
	  TxHeader.StdId             = BROADCAST;     // ID do dispositivo
	  TxHeader.RTR               = CAN_RTR_DATA;       //(Remote Transmission Request) especifica Remote Fraame ou Data Frame.
	  TxHeader.IDE               = CAN_ID_STD;    //define o tipo de id (standard ou extended
	  TxHeader.DLC               = 7;      //Tamanho do pacote 0 - 8 bytes
	  TxHeader.TransmitGlobalTime = DISABLE;

	  int status = HAL_CAN_AddTxMessage(&hcan1, &TxHeader, tx, &TxMailbox);
	  if(status)
	  {
		 Error_Handler();
	  }
	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  HAL_Delay(delay);
}
