/*
 * debug_level.h
 *
 *  Created on: Mar 9, 2024
 *      Author: lucas
 */

#ifndef INC_DEBUG_LEVEL_H_
#define INC_DEBUG_LEVEL_H_

#include "main.h"
#include "Parse.h"

enum debugLevel
{
	NONE,
	TRACE
};

int print_debug (char *msg);

#define DEBUG_LEVEL TRACE

#endif /* INC_DEBUG_LEVEL_H_ */
