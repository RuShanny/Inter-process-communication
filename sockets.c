#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <assert.h>

#include "interface.h"
#include "sockets.h"
#include "utils.h"

void get_socket_ID (int* sock);
void client_connection (int sock);
void server_connection (connection* ptr);

void prepare_process_sock (connection* ptr, int type) {
	if (type == 0) {
		get_socket_ID(&(SOCK_CAST(ptr->connection_params)->init_socket));  
		client_connection(SOCK_CAST(ptr->connection_params)->init_socket);
	} else {
		get_socket_ID(&(SOCK_CAST(ptr->connection_params)->server_socket));
		server_connection(ptr);	
		}
}

void get_socket_ID (int* sock) {
	if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {   //create socket of server
		printf("failed to create the socket");
		exit(1);
	} else printf("socket was successfully created\n");
}
void client_connection (int sock) {
	struct hostent *h; 	
	struct sockaddr_in server, client;  			// structres sockaddr_in for server and client
	memset((char*)&client, '\0', sizeof(client));  		//clean structure client
	
	client.sin_family = AF_INET;				//full sock structure
	client.sin_addr.s_addr = INADDR_ANY;
	client.sin_port = CLIENT_PORT;
	
	if ((bind (sock, (struct sockaddr*)&client, sizeof(client))) < 0) {
		printf("failed to bind the socket: %s\n", strerror(errno));
		exit(1);
	} else printf("socket was successfully bound\n");
	
	memset((char*)&client, '\0', sizeof(client));    
	
	printf("sock in client %d\n", sock);
	if ((h = gethostbyname(SERVER_HOST)) == NULL) {
		printf("Failed to get host by name: %s\n", strerror(errno));
		exit(1);
	}
	
	server.sin_family = AF_INET;
	if ((memcpy((char*)&server.sin_addr, h->h_addr, h->h_length)) == NULL) {
		printf("Failed to set server address: %s\n", strerror(errno));        
		exit(1);
	}
	server.sin_port = SERVER_PORT;
	socklen_t addrlen = (socklen_t)(sizeof(server));   
	int res_connect = 0;
	res_connect = connect(sock, (struct sockaddr*)&server, addrlen);
	while(1) {
		if (res_connect!= -1) break;
			else if (errno != ECONNREFUSED) {
			printf("error: ");
			printf("failed to connect to server: %s\n", strerror(errno));
			exit(1);
			}	
		res_connect = connect(sock, (struct sockaddr*)&server, addrlen);
	}
	printf("client connected to server\n");	
}

void recv_sock (connection* ptr, char* buffer_recv, int buf_size) {  
	int recv_buf_size = recv(SOCK_CAST(ptr->connection_params)->init_socket, buffer_recv, buf_size, 0);
	if ( recv_buf_size != -1 ) printf("data was received\n");
		else {
			printf("failed to receive data\n");
			printf("error: %s\n", strerror(errno));
			exit(1);
		}
}

void clean_sock (connection* ptr, int type) {
	if (type == 0) {
		if ((shutdown(SOCK_CAST(ptr->connection_params)->init_socket, 0)) == 0) printf("connection1 was killed\n");
			else { 
				printf("failed to kill connection: %s", strerror(errno));
				exit(1);
			}
		close(SOCK_CAST(ptr->connection_params)->init_socket);

		


	} else {	
		if ((shutdown(SOCK_CAST(ptr->connection_params)->server_socket, 0)) == 0) printf("connection2 was killed\n");
			else { 
				printf("failed to kill connection: %s", strerror(errno));
				exit(1);
			}
		close(SOCK_CAST(ptr->connection_params)->server_socket);
	}
	free(ptr->connection_params);	
}

void server_connection (connection* ptr) {
	struct sockaddr_in serv, client;  		
	memset((char*)&serv, '\0', sizeof(serv));	//clean structure
	
	serv.sin_family = AF_INET;			
	serv.sin_addr.s_addr = INADDR_ANY;
	serv.sin_port = PORT;
	printf("server_socket = %d\n", SOCK_CAST(ptr->connection_params)->server_socket);


	const int on = 1;
	setsockopt(SOCK_CAST(ptr->connection_params)->server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);


	if (bind (SOCK_CAST(ptr->connection_params)->server_socket, (struct sockaddr*)&serv, sizeof(serv)) < 0) {
		printf("failed to bind the server socket\n");
		printf("%s\n", strerror(errno));
		exit(1);
	} else printf("server socket was successfully bound\n");
	
	printf("Server started\n");
	
	printf("server_sock = %d\n", SOCK_CAST(ptr->connection_params)->server_socket);

	if ((listen (SOCK_CAST(ptr->connection_params)->server_socket, 3)) < 0) {
		printf("failed to listen()\n");
		exit(1);
	} else printf("listen() is ok\n");

	int addr = sizeof(client);
	if ((SOCK_CAST(ptr->connection_params)->client_socket = accept(SOCK_CAST(ptr->connection_params)->server_socket, (struct sockaddr*)&client, &addr)) < 0) {
		printf("failed to accept the client connection, client_socket = %d\n", SOCK_CAST(ptr->connection_params)->client_socket);
		exit(1);
	} else printf("accepted the client connection\n"); 
	
	printf("client_sock = %d\n", SOCK_CAST(ptr->connection_params)->client_socket);

}


void send_sock (connection* ptr, char* buffer_send, int buf_size, int flag) {
	int sent_buf_size = send(SOCK_CAST(ptr->connection_params)->client_socket, buffer_send, buf_size, 0);
	if ( sent_buf_size == buf_size ) printf("data was sent successfully\n");
		else {
			printf("failed to send(): %s\n", strerror(errno));
		}
}
