#ifndef _WASHER_INTERFACE_H
#define _WASHER_INTERFACE_H

#define NAME_LENGTH 30
#define TOTAL_LENGTH 40
#define MAXSIZE 50
#include <sys/sem.h>

typedef struct connection {
	void* connection_params;
	void (*prepare_process)(struct connection*, int);
	void (*sendblock)(struct connection*, char*, int, int);
	void (*recvblock)(struct connection*, char*, int);
	void (*clean)(struct connection*, int);	
	void (*after_operation)(struct connection*);
	void (*before_operation)(struct connection*);
} connection;
	
#endif
