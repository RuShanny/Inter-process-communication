#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "utils.h"

void init (char flag, connection* ptr, char* argv[], int type);
universal* parsing (char *tmp);
int readfile (char* file_name, int flag_for_read, info* dbase, data* storage);
void washing(int all_num, connection* ptr, info* dbase, int all_types_num, data* dirty);

info dbase[MAXSIZE] = {0};
data dirty[MAXSIZE] = {0};

int main (int argc, char* argv[]) {
	connection connect;
	connection* ptr = &connect;
	char flag_for_init = atoi(argv[3]);
	int flag_for_read = 0, type = 1;

	int num = readfile(argv[1], flag_for_read, dbase, dirty);		
	flag_for_read = 1;
	int num_dirty = readfile(argv[2], flag_for_read, dbase, dirty);
 
	int i, all_dirty_dishes = 0;
	for ( i = 0; i < num_dirty; i++ ) all_dirty_dishes += dirty[i].number;
	printf("all_dirty_dishes = %d\n", all_dirty_dishes);

	init(flag_for_init, ptr, argv, type);
	ptr->prepare_process(ptr, type);
	washing(all_dirty_dishes, ptr, dbase, num, dirty);
	ptr->clean(ptr, type);
	return 0;
}

void washing(int all_num, connection* ptr, info* dbase, int all_types_num, data* storage) {
	int limit = get_limit();
	int buf_size = limit*NAME_LENGTH;
	char buffer_send[buf_size];
	memset(buffer_send, 0, buf_size);
	char* ptr_buf_send = buffer_send;
	int i, current_num = 0, current_type = 0;
	int flag = 0;

	for ( i = 0; i < all_num; i++ ){
		strcpy(&ptr_buf_send[current_num*NAME_LENGTH], storage[current_type].name);
		char* ptr_tmp = &ptr_buf_send[current_num*NAME_LENGTH];
		operation_time(dbase, ptr_tmp, all_types_num);
		storage[current_type].number--;

		if ( storage[current_type].number == 0 ) current_type++;
		current_num++;
		
		if ( current_num == limit ) {
			ptr->sendblock(ptr, buffer_send, buf_size, flag);
			current_num = 0;
			clean_buf (ptr_buf_send);	
		}
	}
	if ( current_num != 0 ) {
		flag = 1;
		ptr->sendblock(ptr, buffer_send, buf_size, flag);
		current_num = 0;
	}		
}

universal* parsing (char *tmp) {
	char* tmp1 = strchr(tmp,':')+1;
	int len = strlen(tmp1);
	static universal new;
	universal* ptr_uni;
	ptr_uni = &new;
	strncpy(new.name, tmp, strlen(tmp)-len);
	new.name[strlen(tmp)-len-1] = '\0';
	sscanf(tmp1+1, "%d", &new.num);		
	return ptr_uni;
}

int readfile (char* file_name, int flag_for_read, info* dbase, data* storage) {
	FILE* f = fopen( file_name, "r" );
	if (f == NULL) {
		perror("failed to OPEN file");
		system ("PAUSE");
		return 0;
	}
	int num_of_elem = 0;
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
		
		universal* ptr_uni = parsing(tmp);
		
		if (flag_for_read == 0) {
			strcpy(dbase[num_of_elem].name, ptr_uni->name);
			dbase[num_of_elem].time = ptr_uni->num;
			printf("name: %s\n", dbase[num_of_elem].name);
			printf("time: %d\n\n", dbase[num_of_elem].time);
			
		} else {
			strcpy(storage[num_of_elem].name, ptr_uni->name);
			storage[num_of_elem].number = ptr_uni->num;	
			printf("name: %s\n", storage[num_of_elem].name);
			printf("number: %d\n\n", storage[num_of_elem].number);
		}
		num_of_elem++;

		if (num_of_elem > MAXSIZE) {
		     printf("buffer overflow is detected\n");
		     printf("please, decrease number of elements in file to be read\n");	
		     exit(1);	
		}
	} while(1);
	fclose(f);
	return num_of_elem;	
}
