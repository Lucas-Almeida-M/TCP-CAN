

#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_


#include "can.h"
#include <stdio.h>
#include <stdbool.h>
#include "stdio.h"
#include "stdlib.h"
#include "main.h"
#include "cmsis_os.h"
#include "fila.h"


#define STD_ID  0
#define EXT_ID  1
#define FILTER_TYPE_16 0
#define FILTER_TYPE_32 1
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
		BROADCAST  = 0x00,
		BOARD_F7   = 0x01,
		DEVICE_1   = 0x02, // HABILITAR DE ACORDO COM DEVICE
		DEVICE_2   = 0x03,
		DEVICE_3   = 0x04,
		DEVICE_4   = 0x05,
		DEVICE_5   = 0x06,
		DEVICE_6   = 0x07,
		DEVICE_7   = 0x08,
		DEVICE_8   = 0x09,
		DEVICE_9   = 0x0A,
		DEVICE_10  = 0x0B,
		CANID_COUNT

	} FilterId;

	uint32_t FilterIdList[CANID_COUNT];

} CanFilterList;

typedef struct _DeviceState
{
	bool devices[10]; // HABILITAR DE ACORDO COM device
	uint8_t deviceCount;
}DeviceState;

enum MessageType
{
	CONFIG = 0,
	DATA   = 1,
	SYNC   = 2
};

typedef struct control
{
	bool bit0 : 1;
	bool bit1 : 1;
	bool bit2 : 1;
	bool bit3 : 1;
	bool bit4 : 1;
	bool bit5 : 1;
	bool bit6 : 1;
	bool bit7 : 1;
}controlBit;

typedef struct candata
{
	union Ctrl0
	{
		controlBit controlBits;
		uint8_t value;
	}ctrl0;

	union ctrl1
	{
		controlBit controlBits;
		uint8_t value;
	}ctrl1;

	uint8_t data [CAN_SIZE - CAN_HEADER];
}CanData;

typedef union CANPACKET
{
	uint8_t buffer[CAN_SIZE + 1];
	struct
	{
		uint8_t canID;
		union canbuf
		{
			CanData canDataFields;
			uint8_t canData[8];
		}canBuffer;

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



bool ValidatePacket(uint8_t canID);


void DecodeCanPacket(uint32_t canID, CanPacket *canPacket, uint8_t *buffer);
bool CanWritePacket(uint32_t id, uint8_t *buffer, uint8_t can_rtr, uint16_t tamanho);
void ConfigFilterList (uint32_t id1, uint32_t id2, uint32_t id3, uint32_t id4, uint32_t filterBank, uint8_t idType, uint8_t filterScale);





void LoadFilterList(CanFilterList *filterIdList);
void InitFilterList(uint32_t *idList, uint8_t numFilters, uint8_t filterScale);


#endif /* SRC_PARSER_H_ */
