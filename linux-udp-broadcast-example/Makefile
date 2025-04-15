
OBJS_SERVER = b_server.o
OBJS_CLIENT = b_client.o
OBJS_LISTNER = b_listner.o
LIBS_SERVER = 
LIBS_CLIENT = 
LIBS_LISTNER = 

CFLAGS = -c -Wall
CC = gcc

PROS = b_client b_server b_listner

all: $(PROS)

.c.o:
	$(CC) $(CFLAGS) $<

b_client: $(OBJS_CLIENT)
	$(CC) -o $@ $^ $(LIBS_CLIENT)

b_server: $(OBJS_SERVER)
	$(CC) -o $@ $^ $(LIBS_SERVER)

b_listner: $(OBJS_LISTNER)
	$(CC) -o $@ $^ $(LIBS_LISTNER)

clean:
	rm -rf $(PROS) $(OBJS_CLIENT) $(OBJS_SERVER)
