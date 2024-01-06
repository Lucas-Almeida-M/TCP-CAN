#include <stdbool.h>
#include "Parse.h"

#define RESPONSE  		(uint8_t) 		0b00000001
#define SIGNAL   	 	(uint8_t) 		0b00000010
#define MULTIPLIER  	(uint8_t) 		0b00000100
//#define DEFAULT     	(uint8_t) 		0b00001000
#define STATUS  		(uint8_t) 		0b00010000
#define CONFIGURATION   (uint8_t) 		0b00100000
#define RESTART   		(uint8_t) 		0b01000000
#define OPEN_MESSAGE    (uint8_t) 		0b10000000


//void processIDCommand(CanPacket *canPacket);
