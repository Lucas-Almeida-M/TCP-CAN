/*
 * eth_app.c
 *
 *  Created on: Jun 25, 2024
 *      Author: lucas
 */
#include "eth_app.h"


void decode_msg(uint8_t *msg) {
    // Encontrar o número entre dois '#'
    uint8_t id = 0;
    uint8_t mt = 0;
    uint16_t buff[20] = {0};
    int i = 0;
    char *ptr = (char *)msg;
    char *num_start = strchr(ptr, '#');
    if (num_start != NULL) {
        num_start++;
        char *num_end = strchr(num_start, '#');
        if (num_end != NULL && num_end > num_start) {
            char num_buf[32];
            strncpy(num_buf, num_start, num_end - num_start);
            num_buf[num_end - num_start] = '\0';
            id = atoi(num_buf);

            printf("Número entre '#': %d\n", id);
        }
    }

    // Encontrar o número entre dois '$'
    ptr = (char *)msg;
    num_start = strchr(ptr, '$');
    if (num_start != NULL) {
        num_start++;
        char *num_end = strchr(num_start, '$');
        if (num_end != NULL && num_end > num_start) {
            char num_buf[32];
            strncpy(num_buf, num_start, num_end - num_start);
            num_buf[num_end - num_start] = '\0';
            mt = atoi(num_buf);
            printf("Número entre '$': %d\n", mt);
        }
    }

    // Encontrar os números entre dois '&'
    ptr = (char *)msg;
    num_start = strchr(ptr, '&');
    while (num_start != NULL) {
        num_start++;
        char *num_end = strchr(num_start, '&');
        if (num_end != NULL && num_end > num_start) {
            char num_buf[32];
            strncpy(num_buf, num_start, num_end - num_start);
            num_buf[num_end - num_start] = '\0';
            buff[i] = atoi(num_buf);
            printf("Número entre '&': %d\n", buff[i]);
            i++;
            num_start = num_end;
        } else {
            break;
        }
        num_start = strchr(num_start, '&');
    }
}
