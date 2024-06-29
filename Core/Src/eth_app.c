/*
 * eth_app.c
 *
 *  Created on: Jun 25, 2024
 *      Author: lucas
 */
#include "eth_app.h"

extern osMessageQId queue_tcp_receiveHandle;
extern osMessageQId queue_can_sendHandle;


static void decode_msg(uint8_t *msg, uint8_t *id, uint8_t *mt, uint8_t *buff, uint8_t *bufflen)
{


    int i = 0;
	char *ptr = (char*) msg;
	char *num_start = strchr(ptr, '#');
	if (num_start != NULL)
	{
		num_start++;
		char *num_end = strchr(num_start, '#');
		if (num_end != NULL && num_end > num_start)
		{
			char num_buf[32];
			strncpy(num_buf, num_start, num_end - num_start);
			num_buf[num_end - num_start] = '\0';
			*id = atoi(num_buf);
		}
	}


    ptr = (char *)msg;
    num_start = strchr(ptr, '$');
    if (num_start != NULL)
    {
        num_start++;
        char *num_end = strchr(num_start, '$');
        if (num_end != NULL && num_end > num_start)
        {
            char num_buf[32];
            strncpy(num_buf, num_start, num_end - num_start);
            num_buf[num_end - num_start] = '\0';
            *mt = atoi(num_buf);
        }
    }


    ptr = (char *)msg;
    num_start = strchr(ptr, '&');
    while (num_start != NULL)
    {
        num_start++;
        char *num_end = strchr(num_start, '&');
        if (num_end != NULL && num_end > num_start)
        {
            char num_buf[32];
            strncpy(num_buf, num_start, num_end - num_start);
            num_buf[num_end - num_start] = '\0';
            buff[i] = atoi(num_buf);
            *bufflen+=1;
            printf("Número entre '&': %d\n", buff[i]);
            i++;
            num_start = num_end;
        } else
        {
            break;
        }
        num_start = strchr(num_start, '&');
    }
}


void ReceiveTCP_MSG(void const * argument)
{
  /* USER CODE BEGIN ReceiveCAN_MSG */
	uint8_t id = 0;
	uint8_t mt = 0;
	uint8_t bufflen = 0;
	uint8_t buff[6] = {0};
	char msg[64] = {0};

  /* Infinite loop */
  for(;;)
  {
    BaseType_t xStatus = xQueueReceive(queue_tcp_receiveHandle, &msg, 0);
	if (xStatus == pdPASS)
	{
		CanPacket canMSG = {0};
		decode_msg((uint8_t*)&msg, &id, &mt, buff, &bufflen);

		canMSG.canID = id;
		canMSG.canDataFields.ctrl0 = mt;
		memcpy(canMSG.canDataFields.data, buff, bufflen);

		xQueueSendToBack(queue_can_sendHandle, &canMSG ,0);

		memset(&id, 0, sizeof(id));
		memset(&mt, 0, sizeof(mt));
		memset(&bufflen, 0, sizeof(bufflen));
		memset(buff, 0, sizeof(buff));
	}
    osDelay(1);
  }
  /* USER CODE END ReceiveCAN_MSG */
}
