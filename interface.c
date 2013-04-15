#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "interface.h"
#include "shm.h"
#include "sockets.h"
#include "utils.h"

void InitSocket (connection* ptr, int type) {
	struct sock_params* ptr_params;
	ptr_params = (struct sock_params*)malloc(sizeof(struct sock_params));	
	ptr->connection_params = (void*)ptr_params;  
	ptr->recvblock = recv_sock;	
	ptr->sendblock = send_sock;	
	ptr->clean = clean_sock;
	ptr->prepare_process = prepare_process_sock;	
}

void InitShm (connection* ptr, char* argv[], int type) {
	struct shm_params* ptr_params;
	ptr_params = (struct shm_params*)malloc(sizeof(struct shm_params));	
	ptr->connection_params = (void*)ptr_params;  
	ptr->recvblock = recv_shmem;
	ptr->sendblock = send_shmem;
	ptr->clean = clean_shm;
	ptr->prepare_process = prepare_process_shm;
}

void init (char flag, connection* ptr, char* argv[], int type) {
	if (flag == 1) InitShm (ptr, argv, type);		
	else InitSocket(ptr, type);			
}

