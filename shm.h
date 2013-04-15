#ifndef WASH_SHM_H
#define WASH_SHM_H
#include "interface.h"
#define NSEMS 1

#define SHM_CAST(param) ((shm_params*)(param))

typedef struct shm_params {	
	int semid1;
	int semid2;
	int shmid;
	char* addr;
	struct sembuf sem1_lock;
	struct sembuf sem2_lock;
	struct sembuf sem1_unlock; 
	struct sembuf sem2_unlock; 
} shm_params;

void prepare_process_shm (connection* ptr, int type);
int semget_sem (char* file);
char* shmem_buff (char* file, int* shmid);
void prepare_semaphores (connection* ptr, char* file);
void before_recv (connection* ptr);
void after_recv (connection* ptr);
void before_send (connection* ptr);
void after_send (connection* ptr, int flag);
void clean_shm (connection* ptr, int type);
void recv_shmem (connection* ptr, char* buffer, int buf_size);
void send_shmem (connection* ptr, char* buffer, int buf_size, int flag);

#endif
