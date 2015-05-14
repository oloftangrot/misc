all: esp8266trial1

esp8266trial1: esp8266trial1.c http_parser.c
	gcc -Wall -std=c99 -o $@ $^ ../serlink/posix_async_port.c 

clean:
	rm esp8266trial1
	rm *.o


