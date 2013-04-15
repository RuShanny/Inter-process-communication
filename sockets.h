#ifndef _WASH_SOCKETS_H
#define _WASH_SOCKETS_H

#include "interface.h"
#define PORT 5555
#define SERVER_PORT 5555
#define CLIENT_PORT 5556
#define SERVER_HOST "localhost"

#define SOCK_CAST(param) ((sock_params*)(param))

void prepare_process_sock (connection* ptr, int type);
void send_sock (connection* ptr, char* buf, int buf_size, int flag);
void recv_sock (connection* ptr, char* buf, int buf_size);
void client_connection(int socket);
void server_connection(connection* ptr);
void clean_sock(connection* ptr, int type);

typedef struct sock_params {
	int server_socket;
	int client_socket;
	int init_socket;
} sock_params;

#endif
