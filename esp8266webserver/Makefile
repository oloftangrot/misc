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

all: esp8266trial1 esp8266trial2

esp8266trial1: esp8266trial1.c http_parser.c
	gcc -Wall -std=c99 -o $@ $^ ../serlink/posix_async_port.c 

esp8266trial2: esp8266trial2.cpp http_parser.c
	g++ -Wall -o $@ $^ ../serlink/posix_async_port.c 
	
clean:
	rm esp8266trial1 esp8266trial2
	rm *.o


