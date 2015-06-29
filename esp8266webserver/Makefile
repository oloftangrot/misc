#  Copyright (C) 2015  Olof Tångrot
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

all: esp8266trial1 esp8266trial2 esp8266trial2-1

esp8266trial1: esp8266trial1.c http_parser.c posix_async_port.c
	gcc -Wall -std=c99 -o $@ $^  

esp8266trial2: esp8266trial2.cpp http_parser.c wap.h posix_async_port.c 
	g++ -g -Wall -o $@ $^ 

esp8266trial2-1: esp8266trial2.cpp http_parser.c wap.h serial.c 
	g++ -g -Wall -o $@ $^ 
	
clean:
	rm esp8266trial1 
	rm esp8266trial2
	rm esp8266trial2-1
	rm *.o


