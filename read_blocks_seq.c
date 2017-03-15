#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include "record.h"

int main(int argc, char *atgv[]){
	int block_size = atoi(atgv[2]);
	int records_per_block = block_size/sizeof(Record);
	FILE *fp_read;

	/* allocate buffer for 1 block */
	Record * buffer = (Record *) calloc (records_per_block, sizeof (Record)) ;
	
	if (!(fp_read = fopen (atgv[1] , "rb" ))){
		return -1;
	}

	//Figure out the file size
	fseek(fp_read, 0L, SEEK_END);
	int file_size = ftell(fp_read);

	//Set the point back to begining of the file
	fseek(fp_read, 0L, SEEK_SET);

	printf("%lu\n", file_size/sizeof(Record));
	
	/* read records into buffer */
	struct timeb t_begin, t_end;
    long time_spent_ms;
    ftime(&t_begin);
	int result = 0;
	
	int current_max_id = -1;
	int current_max_followers = 0;
	int previous_max_id = -1;
	int previous_max_followers = -1;
	int id_count = 0;
    
	int record_count = 0;

	while((result = fread (buffer, sizeof(Record), records_per_block, fp_read)) > 0){
		int pointer = 0;
		records_per_block = block_size/sizeof(Record);

		//Check if the total number of records read from file is less than one block 
		if (result!=records_per_block){
			records_per_block = result;
			record_count+=records_per_block;
		}else{
			record_count+=records_per_block;
		}

		while(pointer < records_per_block){
			printf("uid %d, uid2 %d\n", buffer[pointer].uid1, buffer[pointer].uid2);
			if(current_max_id == -1){
				current_max_id = buffer[pointer].uid1;
				current_max_followers = 1;
				id_count++;
			}else if(current_max_id == buffer[pointer].uid1){
				current_max_followers++;
			}else{
				if(previous_max_followers < current_max_followers && previous_max_id != current_max_id){
					previous_max_followers = current_max_followers;
					previous_max_id = current_max_id;
				}
				current_max_id = buffer[pointer].uid1;
				current_max_followers = 1;
				id_count++;
			}
			pointer++;
			
		}

		
	}
	//Check for the last unique uid
	if(previous_max_followers < current_max_followers && previous_max_id != current_max_id){
		previous_max_followers = current_max_followers;
		previous_max_id = current_max_id;
		id_count++;
	}
	
    ftime(&t_end);
    time_spent_ms = (long) (1000 *(t_end.time - t_begin.time)
       + (t_end.millitm - t_begin.millitm));
    printf("%lu\n", record_count*sizeof(Record));
    printf ("Data rate: %.3f MBPS\n", ((record_count*sizeof(Record))/(float)time_spent_ms * 1000)/(1024*1024));
	printf ("total records: %d\n", (record_count));
	printf("uid %d with max followers %d, total number of uid %d, avg %.3f\n", previous_max_id, previous_max_followers, id_count, record_count/(float)id_count);
	write_result_to_file("read_block_seq.txt", block_size, ((record_count*sizeof(Record))/(float)time_spent_ms * 1000)/(1024*1024));
	fclose (fp_read);
	free (buffer);
	return 0;
}

