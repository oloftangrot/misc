#ifndef TICKER_H
#define TICKER_H

#define TICKERNAME_LENGTH (32)

union tickerMsg_t {
  	char str[64];
  	int data[16];
} ;

#endif
