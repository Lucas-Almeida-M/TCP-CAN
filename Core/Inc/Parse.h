

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


typedef union
{

	enum FilterIds
	{
		SYNC     		= 0x00,
		SYNC_RESPONSE   = 0x01,
		DATA     		= 0x02,
		CONFIG   		= 0x03,
		CONFIG_RESPONSE = 0x04,
		REBOOT   		= 0x05,
		CANID_COUNT

	} FilterId;

	uint32_t FilterIdList[CANID_COUNT];

} CanFilterList;



enum Devices
{
	Device0   = 0x0,
	Device1   = 0x1,
	Device2   = 0x2,
	Device3   = 0x3,
	Device4   = 0x4,
	Device5   = 0x5,
	Device6   = 0x6,
	Device7   = 0x7,
	Device8   = 0x8,
	Device9   = 0x9,
	BroadCast = 0xfe,
	ClientBoard = 0xff
};


typedef struct candata
{
	uint8_t deviceNum;
	uint8_t control;
	uint8_t data [CAN_SIZE - CAN_HEADER];
}CanData;

typedef struct
{
	uint8_t canID;
	CanData canDataFields;
} CanPacket;


bool CanWritePacket(uint32_t id, uint8_t *buffer, uint8_t can_rtr, uint16_t tamanho);
void ConfigFilterList (uint32_t id1, uint32_t id2, uint32_t id3, uint32_t id4, uint32_t filterBank, uint8_t idType, uint8_t filterScale);


void LoadFilterList(CanFilterList *filterIdList);
void InitFilterList(uint32_t *idList, uint8_t numFilters, uint8_t filterScale);
void CAN_ConfigFilter(void);


#endif /* SRC_PARSER_H_ */
