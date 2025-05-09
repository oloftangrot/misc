#ifndef MESSAGE_H
#define MESSAGE_H

#define COMMON_MESSAGE_PORT (10000)
#define MESSENGERNAME_LENGTH (32) 
#define TICK_COUNTER_POS (8) // Assume the first 32 bytes is occupied by the ticker name. The first free data slot is 8.

union messengerMsg_t {
  	char str[64];
  	int data[16];
} ;

#endif
