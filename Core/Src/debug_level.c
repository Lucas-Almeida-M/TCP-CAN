/*
 * debug_level.c
 *
 *  Created on: Mar 9, 2024
 *      Author: lucas
 */
#include "debug_level.h"
//#include "parse.h"
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
#define F7

int print_debug (char *msg)
{
	#ifdef NUCLEO
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 10);
	#endif //NUCLEO

	#ifdef BLACK_PILL
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(&msg), 10);
	#endif //BLACK_PILL

	#ifdef F7
	HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), 10);
	#endif //NUCLEO

	return 0;
}
