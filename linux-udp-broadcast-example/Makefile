
OBJS_SERVER = b_server.o
OBJS_TICKER = b_ticker.o
OBJS_LISTNER = b_listner.o
LIBS_SERVER = 
LIBS_TICKER = 
LIBS_LISTNER = 

CFLAGS = -c -Wall
CC = gcc

PROS = b_ticker b_server b_listner

all: $(PROS)

.c.o:
	$(CC) $(CFLAGS) $<

b_ticker: $(OBJS_TICKER)
	$(CC) -o $@ $^ $(LIBS_TICKER)

b_server: $(OBJS_SERVER)
	$(CC) -o $@ $^ $(LIBS_SERVER)

b_listner: $(OBJS_LISTNER)
	$(CC) -o $@ $^ $(LIBS_LISTNER)

clean:
	rm -rf $(PROS) $(OBJS_TICKER) $(OBJS_SERVER) $(OBJS_LISTNER)
