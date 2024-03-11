


#include <Parse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"


extern struct Queue* queue;



uint32_t TxMailbox;

CAN_FilterTypeDef *canFilterConfig1;


bool CanWritePacket(uint32_t id, uint8_t *buffer, uint8_t can_rtr, uint16_t tamanho)
{

	TxHeader.StdId             = id;
	TxHeader.RTR               = can_rtr;
	TxHeader.IDE               = CAN_ID_STD;
	TxHeader.DLC               = tamanho;
	TxHeader.TransmitGlobalTime = DISABLE;


	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, buffer, &TxMailbox) != HAL_OK)
	{

		return false;
	}

	 while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3) { }

	return true;
}

void LoadFilterList(CanFilterList *filterIdList) {

	for (int i = 0; i < CANID_COUNT; i++) {

        filterIdList->FilterIdList[i] = (uint32_t) i;
    }
}


//metodo de configuração dos ID can
void ConfigFilterList (uint32_t id1, uint32_t id2, uint32_t id3, uint32_t id4, uint32_t filterBank, uint8_t idType, uint8_t filterScale)
{
    CAN_FilterTypeDef filterConfig = {0};

    filterConfig.FilterActivation = ENABLE;
    filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filterConfig.FilterBank = filterBank;

    if (filterScale == FILTER_TYPE_32) {

        filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
        filterConfig.FilterMode = CAN_FILTERMODE_IDLIST;

        if (idType == EXT_ID) {

            filterConfig.FilterIdHigh = (id1 & 0x1FFFFFFF) | (id1 << 31);
            filterConfig.FilterIdLow = (id2 & 0x1FFFFFFF) | (id2 << 31);
            filterConfig.FilterMaskIdHigh = (id3 & 0x1FFFFFFF) | (idType << 31);
            filterConfig.FilterMaskIdLow = (id4 & 0x1FFFFFFF) | (idType << 31);

        } else {
            filterConfig.FilterIdHigh = id1 & 0x1FFFFFFF;
            filterConfig.FilterIdLow = id2 & 0x1FFFFFFF;
            filterConfig.FilterMaskIdHigh = id3 & 0x1FFFFFFF;
            filterConfig.FilterMaskIdLow = id4 & 0x1FFFFFFF;
        }

    } else {

        filterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
        filterConfig.FilterMode = CAN_FILTERMODE_IDLIST;

        if (idType == EXT_ID) {
            filterConfig.FilterIdHigh = ((id1 & 0x1FFF) << 5) | (idType << 15);
            filterConfig.FilterIdLow = ((id2 & 0x1FFF) << 5) | (idType << 15);
            filterConfig.FilterMaskIdHigh = ((id3 & 0x1FFF) << 5) | (idType << 15);
            filterConfig.FilterMaskIdLow = ((id4 & 0x1FFF) << 5) | (idType << 15);
        } else {
            filterConfig.FilterIdHigh = (id1 & 0x7FF) << 5;
            filterConfig.FilterIdLow = (id2 & 0x7FF) << 5;
            filterConfig.FilterMaskIdHigh = (id3 & 0x7FF) << 5;
            filterConfig.FilterMaskIdLow = (id4 & 0x7FF) << 5;
        }
    }

    HAL_CAN_ConfigFilter(&hcan1, &filterConfig);

}

void InitFilterList(uint32_t *idList, uint8_t numFilters, uint8_t filterScale)
{


	uint16_t filterBankCounter = 0;
	uint8_t maxFilterBanks = FILTER_MAX_NUM;
	uint32_t id, id1, id2, id3, id4 = 0;


	if (numFilters > maxFilterBanks)
	{
		numFilters = maxFilterBanks;
	}


	if (filterScale == FILTER_TYPE_32)
	{
		for (id = 0; id < numFilters && filterBankCounter < maxFilterBanks; id += 2)
		{
			id1 = idList[id];
			id2 = idList[id + 1];
			ConfigFilterList(id1, id2, 0x0000, 0x0000, filterBankCounter, STD_ID, filterScale);
			filterBankCounter++;
		}
	}
	else
	{
		for (id = 0; id < numFilters && filterBankCounter < maxFilterBanks;id += 4)
		{
			id1 = idList[id];
			id2 = idList[id + 1];
			id3 = idList[id + 2];
			id4 = idList[id + 3];
			ConfigFilterList(id1, id2, id3, id4, filterBankCounter, STD_ID, filterScale);
			filterBankCounter++;
		}
	}
}

void CAN_ConfigFilter(void)
{
 	CAN_FilterTypeDef filterConfig = {0};

	filterConfig.FilterActivation = ENABLE;
	filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	filterConfig.FilterBank = 0;
    filterConfig.FilterIdHigh = 0xff << 5;
    filterConfig.FilterIdLow = 0x00  << 5;
    filterConfig.FilterMaskIdHigh = 0xff << 5;
    filterConfig.FilterMaskIdLow = 0x00 << 5;

	HAL_CAN_ConfigFilter(&hcan1, &filterConfig);

	filterConfig.FilterBank = 1;
	filterConfig.FilterIdHigh = 0xfe << 5;
	filterConfig.FilterIdLow = 0x00  << 5;
	filterConfig.FilterMaskIdHigh = 0xfe << 5;
	filterConfig.FilterMaskIdLow = 0x00 << 5;

	HAL_CAN_ConfigFilter(&hcan1, &filterConfig);

	filterConfig.FilterBank = 2;
	filterConfig.FilterIdHigh = 0x00 << 5;
	filterConfig.FilterIdLow = 0x00  << 5;
	filterConfig.FilterMaskIdHigh = 0x00 << 5;
	filterConfig.FilterMaskIdLow = 0x00 << 5;

	HAL_CAN_ConfigFilter(&hcan1, &filterConfig);

	filterConfig.FilterBank = 3;
	filterConfig.FilterIdHigh = 0x01 << 5;
	filterConfig.FilterIdLow = 0x00  << 5;
	filterConfig.FilterMaskIdHigh = 0x01 << 5;
	filterConfig.FilterMaskIdLow = 0x00 << 5;

	HAL_CAN_ConfigFilter(&hcan1, &filterConfig);

}





