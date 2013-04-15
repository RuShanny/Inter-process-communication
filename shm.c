#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#include "interface.h"
#include "shm.h"
#include "utils.h"

void before_recv(connection* ptr);
void before_send(connection* ptr);

void prepare_process_shm (connection* ptr, int type) {
	prepare_semaphores(ptr, "time_wash");			
	SHM_CAST(ptr->connection_params)->addr = shmem_buff("time_wash", &(SHM_CAST(ptr->connection_params)->shmid));
	if (type == 0) {
		before_recv(ptr);
	} else {
		before_send(ptr);
	}
} 

int semget_sem (char* file) {
		key_t key = ftok(file, 2);
		int semid;
		if ((semid = semget(key, NSEMS, IPC_CREAT|0660)) == -1) {
			printf("semget error: %s\n", strerror(errno));
			exit(1);
			}
		return semid;
}

char* shmem_buff (char* file, int* tmp) {
		int limit = get_limit();
		key_t key = ftok(file, 1);
		if (key == -1) {
			printf("error in shmem_buff: %s\n", strerror(errno));
			exit(1);
		}

		int size = limit*NAME_LENGTH;
		int shmid = shmget(key, size, IPC_CREAT|0660);
		
		if (shmid == -1) {
			printf("shmget error: %s\n", strerror(errno));
			exit(1);
		}
		*tmp = shmid;
		char* addr = shmat(shmid, 0, 0);
		if ((int)addr == -1) {
			printf("shmat() error: %s\n", strerror(errno));
			exit(1);
		}	
		return addr; 
}

void clean_buf (char* buf) {
	int j; 
	int limit = get_limit();
	for (j=0; j<limit; j++) {
		buf[j*NAME_LENGTH] = 0;			
	}
}

void prepare_semaphores (connection* ptr, char* file) {
	SHM_CAST(ptr->connection_params)->semid1 = semget_sem(file);
	SHM_CAST(ptr->connection_params)->semid2 = semget_sem("time_washer");

	SHM_CAST(ptr->connection_params)->sem1_lock.sem_num = 0;
	SHM_CAST(ptr->connection_params)->sem1_lock.sem_op = -1;
	SHM_CAST(ptr->connection_params)->sem1_lock.sem_flg = 0;

	SHM_CAST(ptr->connection_params)->sem1_unlock.sem_num = 0;
	SHM_CAST(ptr->connection_params)->sem1_unlock.sem_op = 1;
	SHM_CAST(ptr->connection_params)->sem1_unlock.sem_flg = 0;

	SHM_CAST(ptr->connection_params)->sem2_lock.sem_num = 0;
	SHM_CAST(ptr->connection_params)->sem2_lock.sem_op = -1;
	SHM_CAST(ptr->connection_params)->sem2_lock.sem_flg = 0;

	SHM_CAST(ptr->connection_params)->sem2_unlock.sem_num = 0;
	SHM_CAST(ptr->connection_params)->sem2_unlock.sem_op = 1;
	SHM_CAST(ptr->connection_params)->sem2_unlock.sem_flg = 0;
	
	printf("sem1 value: %d\n",semctl(SHM_CAST(ptr->connection_params)->semid1, 0, GETVAL, 0));
	printf("sem2 value: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid2, 0, GETVAL, 0));
}

void before_recv (connection* ptr) {
	if (semop(SHM_CAST(ptr->connection_params)->semid1, &(SHM_CAST(ptr->connection_params)->sem1_unlock), 1)==-1) {
			printf("CLIENT semop sem1_unlock error %s\n", strerror(errno));    		//open 1 semaphore
			exit(1);
										    
	} 
//	printf("I've unlocked sem1\n");
//	printf("sem1 value: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid1, 0, GETVAL, 0));


	//printf("I'm going to lock sem2\n");
	//printf("sem2 value before: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid2, 0, GETVAL, 0));		
	if (semop(SHM_CAST(ptr->connection_params)->semid2, &(SHM_CAST(ptr->connection_params)->sem2_lock), 1)==-1) {
		printf("semop sem2_lock error %s\n", strerror(errno));    			//close 2 semaphore
		exit(1);
		}
//	printf("I've locked sem2\n");
//	printf("sem2 value after: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid2, 0, GETVAL, 0));
}

void after_recv (connection* ptr) {
//	printf("I'm going to unlock sem1\n");
//	printf("sem1 value before: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid1, 0, GETVAL, 0));
	if ( semop(SHM_CAST(ptr->connection_params)->semid1, &(SHM_CAST(ptr->connection_params)->sem1_unlock), 1) == -1 ) {
		printf("semop sem1_unlock error %s\n", strerror(errno));    			//open 1 semaphore
		exit(1);
	}
//	printf("I've unlocked sem1\n");
//	printf("sem1 value after: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid1, 0, GETVAL, 0));	

//	printf("I'm going to lock sem2\n");
//	printf("sem2 value before: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid2, 0, GETVAL, 0));
	
	if ( semop(SHM_CAST(ptr->connection_params)->semid2, &(SHM_CAST(ptr->connection_params)->sem2_lock), 1) == -1 ) {
		printf("semop sem2_lock error %s\n", strerror(errno));    			//close 2 semaphore
		exit(1);
	}
//	printf("I've locked sem2\n");
//	printf("sem2 value after: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid2, 0, GETVAL, 0));
	
}

void recv_shmem (connection* ptr, char* buffer_recv, int buf_size) {
	if ( (memcpy(buffer_recv, SHM_CAST(ptr->connection_params)->addr, buf_size)) == NULL ) {
		printf("error in recv_shmem: %s\n", strerror(errno));
		exit(1);	
	}
	after_recv(ptr);
}

void send_shmem (connection* ptr, char* buffer_recv, int buf_size, int flag) {
	if ( (memcpy(SHM_CAST(ptr->connection_params)->addr, buffer_recv, buf_size)) == NULL ) {
		printf("error in send_shmem: %s\n", strerror(errno));
		exit(1);	
	}
	printf("data was send\n");
	clean_buf(buffer_recv);
	after_send(ptr, flag);	
}

void clean_shm (connection* ptr, int type) {
	/*shmdt(SHM_CAST(ptr->connection_params)->addr);									//it's not necessary now to clean shared memory
	struct shmid_ds mds;
	
	int res = shmctl(SHM_CAST(ptr->connection_params)->shmid, IPC_RMID, &mds);
	if (res ==-1) printf("shmctl can't delete shmem\n");
		else printf("shmem has been deleted\n"); */
	if (type == 1) {
		int flag = 1;
		after_send(ptr, flag);	
	}

	if (type == 0) {
		int sem_res = semctl(SHM_CAST(ptr->connection_params)->semid1, 0, IPC_RMID, 0);
		if ( sem_res == -1 ) printf("semctl can't delete 1 semaphore ID: %d\n", SHM_CAST(ptr->connection_params)->semid1);
		else printf("1 semaphore %d has been deleted\n", SHM_CAST(ptr->connection_params)->semid1);
		
		int sem_res1 = semctl(SHM_CAST(ptr->connection_params)->semid2, 0, IPC_RMID, 0);
		if ( sem_res1 == -1 ) printf("semctl can't delete 2 semaphore ID: %d\n", SHM_CAST(ptr->connection_params)->semid2);
		else printf("2 semaphore %d has been deleted\n", SHM_CAST(ptr->connection_params)->semid2);
	}
}

void before_send (connection* ptr) {
	
//	printf("sem1 value before lock: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid1, 0, GETVAL, 0));			
	if (semop(SHM_CAST(ptr->connection_params)->semid1, &(SHM_CAST(ptr->connection_params)->sem1_lock), 1)==-1) {
		printf("SERVER semop sem1_lock error %s\n", strerror(errno));   				 //close 1 semaphore
		exit(1);
	}
//	printf("I've locked sem1\n");
//	printf("sem1  value after lock: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid1, 0, GETVAL, 0));
}

void after_send (connection* ptr, int flag) {
//	printf("I'm going to unlock sem2\n");
//	printf("sem2 value: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid2, 0, GETVAL, 0));
	if ( semop(SHM_CAST(ptr->connection_params)->semid2, &(SHM_CAST(ptr->connection_params)->sem2_unlock), 1) == -1 ) {
		printf("semop sem2_unlock error %s\n", strerror(errno));    				 //close 2 semaphore
		exit(1);
	}
//	printf("I've unlocked sem2\n");
//	printf("sem2 value: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid2, 0, GETVAL, 0));
	
	if (flag == 0) {	
//		printf("I'm going to lock sem1\n");
//		printf("sem1 value before: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid1, 0, GETVAL, 0));
		if (semop(SHM_CAST(ptr->connection_params)->semid1, &(SHM_CAST(ptr->connection_params)->sem1_lock), 1)==-1) {
			printf("semop sem1_lock error %s\n", strerror(errno));    			//close 1 semaphore
			exit(1);
		}
//		printf("I've locked sem1\n");
//		printf("sem1 value after: %d\n\n",semctl(SHM_CAST(ptr->connection_params)->semid1, 0, GETVAL, 0));
	}
	
}

