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


uint8_t canRX[8] = {};
uint8_t canTX[8] = {};
extern uint32_t TxMailbox;
extern osMessageQId queue_can_sendHandle;
extern osMessageQId queue_can_receiveHandle;



/***
 * @fn void ReceiveCAN_MSG(void*)
 * @brief Task to process received CAN messages
 *
 * @param argument
 */
void ReceiveCAN_MSG(void *argument)
{
  /* USER CODE BEGIN ProcessCAN_MSG */
	CanPacket canMSG = {0};
  /* Infinite loop */
  for(;;)
  {
	BaseType_t xStatus = xQueueReceive(queue_can_receiveHandle, &canMSG, 0);
	if (xStatus == pdPASS)
	{
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		// conseguiu tirar da fila
		//	netconn_write(&com1, &canPack, sizeof(canPack), NETCONN_COPY);

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

		TxHeader.StdId             = canMsg.packet.canID;      // ID do dispositivo
		TxHeader.RTR               = CAN_RTR_DATA;       		//(Remote Transmission Request) especifica Remote Fraame ou Data Frame.
		TxHeader.IDE               = CAN_ID_STD;    			//define o tipo de id (standard ou extended
		TxHeader.DLC               = CAN_SIZE;      			//Tamanho do pacote 0 - 8 bytes
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
