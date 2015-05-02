/*
 * server.c
 *
 *  Created on: Oct 9, 2012
 *      Author: hmng
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "jsonrpc-c.h"

#define PORT 1234  // the port users will be connecting to

struct jrpc_server my_server;

cJSON * say_hello(jrpc_context * ctx, cJSON * params, cJSON *id) {
	return cJSON_CreateString("Hello!");
}

cJSON * exit_server(jrpc_context * ctx, cJSON * params, cJSON *id) {
	jrpc_server_stop(&my_server);
	return cJSON_CreateString("Bye!");
}

cJSON * add(jrpc_context * ctx, cJSON * params, cJSON *id) {
	if ( NULL == params ) fprintf ( stderr, "NULL pointer in params!\n" );
	else {
	  int op_a = cJSON_GetObjectItem(params,"op_a")->valueint;
	  fprintf( stderr, "Parsed 1'st integer %d\n", op_a );
	  int op_b = cJSON_GetObjectItem(params,"op_b")->valueint;
	  fprintf( stderr, "Parsed 2'nd integer %d\n", op_b );
	}
	return cJSON_CreateString("Result from add");
}

int main(void) {
	jrpc_server_init(&my_server, PORT);
	jrpc_register_procedure(&my_server, say_hello, "sayHello", NULL );
	jrpc_register_procedure(&my_server, exit_server, "exit", NULL );
	jrpc_register_procedure(&my_server, add, "add", NULL );
	jrpc_server_run(&my_server);
	jrpc_server_destroy(&my_server);
	return 0;
}
