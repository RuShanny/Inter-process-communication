// Following functions are used by both programms

#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

void operation_time (info* dbase, char* ptr_tmp, int all_types_num) {
	int count, flag = 0;
	for (count = 0; count < all_types_num; count++) {
		if ((strcmp(ptr_tmp, dbase[count].name)) == 0) {
			struct timespec req = {0, dbase[count].time};
			struct timespec rem;
			nanosleep(&req, &rem);
			printf("buf: %s  ", ptr_tmp);
			printf("// operation was done for %lu nanosec\n", req.tv_nsec);
			flag = 1;
		}
	}
	if (flag == 0) printf("buf: %s // operation can NOT be DONE (not found in dbase)\n", ptr_tmp);			
}	

int get_num_of_blocks (int all) {
	int limit = get_limit();
	int num_of_blocks = 0;
	if ((all%limit) == 0) num_of_blocks = all/limit; 
	else num_of_blocks = all/limit + 1;
	return num_of_blocks;
}

int get_limit() {
	char* env_var = getenv("TABLELIMIT");
	int limit = atoi(env_var);
	return limit;
}

