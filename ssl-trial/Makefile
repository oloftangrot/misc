
all: client server

cert: mycert.pem
	openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem

client: client.c
	gcc -Wall -o client  client.c -L/usr/lib -lssl -lcrypto

server: server.c
	gcc -Wall -o server server.c -L/usr/lib -lssl -lcrypto

clean:
	rm client server
