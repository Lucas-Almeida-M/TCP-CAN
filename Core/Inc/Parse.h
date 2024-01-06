

#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_


#include "can.h"
#include <stdio.h>
#include <stdbool.h>
#include "library_can.h"
#include "stdio.h"
#include "stdlib.h"
#include "main.h"
#include "cmsis_os.h"
#include "fila.h"


#define STD_ID  0
#define EXT_ID  1
#define FILTER_TYPE_16 0
#define FILTER_TYEPE_32 1
#define FILTER_MAX_NUM 14


#define CAN_HEADER 2
#define CAN_SIZE 8


extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef TxHeader;
extern CAN_RxHeaderTypeDef RxHeader;


typedef union // Ini
{

	enum FilterIds
	{
		CANID_BROADCAST = 0x00,
		CANID_NUCLEO = 0x01,
		CANID_BLUEPILL1= 0x02,
		CANID_BLUEPILL2= 0x03,
		CANID_COUNT

	} FilterId;

	uint32_t FilterIdList[CANID_COUNT];

} CanFilterList;



typedef union CANPACKET
{
	uint8_t buffer[CAN_SIZE + 1];
	struct
	{
		uint8_t canID;
		uint8_t seq;
		union cont
		{
			struct control
			{
				bool bitControl0 : 1;
				bool bitControl1 : 1;
				bool bitControl2 : 1;
				bool bitControl3 : 1;
				bool bitControl4 : 1;
				bool bitControl5 : 1;
				bool bitControl6 : 1;
				bool bitControl7 : 1;
			}c;
			uint8_t control;
		}ctrl;

		uint8_t data [CAN_SIZE - CAN_HEADER];
	}packet;

} CanPacket;




//typedef union UARTPACKET
//{
//	uint8_t buffer[CAN_SIZE + 1];
//	struct
//	{
//		uint8_t canID;
//		uint8_t seq;
//		uint8_t crtl;
//		uint8_t data [(CAN_SIZE + 1) - CAN_HEADER];
//	}packet;
//
//
//}UartPacket;




typedef struct  {

	uint8_t canID;
	CanPacket Can;

}CommunicationPacket;



bool ValidatePacket(uint8_t canID);

void DecodeUartPacket(CommunicationPacket *comPacket, uint8_t *buffer);

void DecodeCanPacket(uint32_t canID, CanPacket *canPacket, uint8_t *buffer);
bool CanWritePacket(uint32_t id, uint8_t *buffer, uint8_t can_rtr, uint16_t tamanho);
void ConfigFilterList (uint32_t id1, uint32_t id2, uint32_t id3, uint32_t id4, uint32_t filterBank, uint8_t idType, uint8_t filterScale);





void LoadFilterList(CanFilterList *filterIdList);
void InitFilterList(uint32_t *idList, uint8_t numFilters, uint8_t filterScale);


#endif /* SRC_PARSER_H_ */
