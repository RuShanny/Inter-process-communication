#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "utils.h"

void init (char flag, connection* ptr, char* argv[], int type);
int readfile_wipe (char* file_name);
int count_num_of_strings_in_file (char* file_name);
void wiping (int all_num, connection* ptr, info* dbase, int all_types_num);

info dbase_wipe[MAXSIZE];

int main (int argc, char* argv[]) {
	char flag_for_init = atoi(argv[3]);
	connection connect;
	connection* ptr = &connect;
	int type = 0;

	int num = readfile(argv[1], dbase_wipe);	
	printf("///////////////////////////////////// \n");
	int all_dirty_dishes = count_num_of_strings_in_file(argv[2]);
	printf("all_dirty_dishes = %d\n", all_dirty_dishes);
	char flag = atoi(argv[3]);
	
	init(flag_for_init, ptr, argv, type);
	ptr->prepare_process(ptr, type);
	wiping(all_dirty_dishes, ptr, dbase_wipe, num);
	ptr->clean(ptr, type);
	return 0;
}

void wiping (int all_num, connection* ptr, info* dbase, int all_types_num) {
	int limit = get_limit();
	int times = get_num_of_blocks(all_num);
	int i, current_num = 0;
	int buf_size = NAME_LENGTH*limit;	
	char buffer_recv[buf_size];
	memset(buffer_recv, 0, buf_size);
    
	for ( i = 0; i < times ; i++ ){
		printf("began to get info from source\n");
		char tmp[NAME_LENGTH];
		ptr->recvblock(ptr, buffer_recv, buf_size);
		while ( current_num < limit ) {
			strcpy(tmp, &buffer_recv[current_num*NAME_LENGTH]);
			if (tmp[0] == 0) {
				printf("data is over\n");
				break;					
			}
			char* ptr_tmp = tmp;
			operation_time(dbase, ptr_tmp, all_types_num);
			current_num++;

		}
		current_num = 0;
	}
}

int count_num_of_strings_in_file (char* file_name) {
	FILE* f = fopen( file_name, "r" );
	if (f == NULL) {
		perror("failed to OPEN file");
		system ("PAUSE");
		return 0;
	}
	int number = 0;
	do {	
		char fstr[TOTAL_LENGTH];
		char *tmp = fstr;
		char* tmpstr = fgets(tmp, TOTAL_LENGTH, f);
		
		if (tmpstr == NULL) {
			printf("End of FILE\n");
			break;
		}
	
		if (strlen(tmpstr) == 1) {
			printf("End of FILE\n");
			break;
		}
		char* tmp1 = strchr(tmp,':')+1;
		int len = strlen(tmp1);
		int num;
		sscanf(tmp1+1, "%d", &num);	
		number+=num;
		
	} while(1);

	return number;
}

int readfile (char* file_name, info* dbase) {
	FILE* f = fopen( file_name, "r" );
	if (f == NULL) {
		perror("failed to OPEN file");
		system ("PAUSE");
		return 0;
	}
	int number = 0;
	number = read_types(f, number, dbase);
	fclose(f);
	return number;	
}

int read_types (FILE* f, int number, info* dbase) {	
	do {	
		char fstr[TOTAL_LENGTH];
		char *tmp = fstr;
		char* tmpstr = fgets(tmp, TOTAL_LENGTH, f);
		
		if (tmpstr == NULL) {
			printf("End of FILE\n");
			break;
		}
	
		if (strlen(tmpstr) == 1) {
			printf("End of FILE\n");
			break;
		}
		
		char* tmp1 = strchr(tmp,':') + 1;
		int len = strlen(tmp1);
		int oddlen = strlen(tmp)-len;

		strncpy(dbase[number].name, tmp, oddlen);
		dbase[number].name[oddlen-1] = '\0';
		sscanf(tmp1+1, "%d", &dbase[number].time);		
		
		printf("name: %s\n", dbase[number].name);
		printf("time: %d\n\n", dbase[number].time);

		number++;

		if (number > MAXSIZE) {
		     printf("buffer overflow is detected\n");
		     printf("please, decrease number of elements in file to be read\n");	
		     exit(1);	
		}
	} while(1);
return number;
}
