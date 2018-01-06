#-
# ----------------------------------------------------------------------------
# "THE BEER-WARE LICENSE" (Revision 69):
# <syrinx@FreeBSD.org> wrote this file.  As long as you retain this notice you
# can do whatever you want with this stuff. If we meet some day, and you think
# this stuff is worth it, you can buy me a beer in return.   -Shteryana Shopova
# ----------------------------------------------------------------------------
#
#CFLAGS=-Wall -Werror -Wunreachable-code -Wshadow -g 
CFLAGS=-Wall -g -Werror -Wunreachable-code -Wshadow
LDFLAGS=-lssl -lcrypto 
CC=/usr/bin/gcc

all: sslserver sslclient

sslserver: sslserver.c
	${CC} ${CFLAGS} -o sslserver sslserver.c ${LDFLAGS}

sslclient: sslclient.c
	${CC} ${CFLAGS} -o sslclient sslclient.c ${LDFLAGS}

clean:
	rm sslserver sslclient
