#ifndef _UTILS_H
#define _UTILS_H
#define NAME_LENGTH 30

typedef struct info {
	char name[NAME_LENGTH];
	int time;
} info;

typedef struct data {
	char name[NAME_LENGTH];
	int number;
} data;

typedef struct universal {
	char name[NAME_LENGTH];
	int num;
} universal;

void operation_time (info* dbase, char* ptr_tmp, int all_types_num);
int get_num_of_blocks (int all);
int get_limit();

#endif
