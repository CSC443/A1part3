#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include "record.h"
// int flush_outputbuffer (char* filename, Record *output_buffer, int current_output_buffer_position);
// int write_degree(char* input_filename, int block_size, int is_outdegree, char* output_filename);

int write_degree(char* input_filename, int block_size, int is_outdegree, char* output_filename){
	static const int mem = 209715200;
	int mem_per_block = mem/block_size;
	int records_per_block = block_size/sizeof(Record);
	int output_buffer_capacity = records_per_block*(mem_per_block - 1);
	Record * output_buffer = (Record *)calloc(output_buffer_capacity, sizeof(Record));
	int current_output_buffer_position = 0;
	printf("%d\n", is_outdegree);

	FILE *fp_read;

	/* allocate buffer for 1 block */
	Record * buffer = (Record *) calloc (records_per_block, sizeof (Record)) ;
	
	if (!(fp_read = fopen (input_filename , "rb" ))){
		return -1;
	}


	//Figure out the file size
	fseek(fp_read, 0L, SEEK_END);
	int file_size = ftell(fp_read);

	//Set the point back to begining of the file
	fseek(fp_read, 0L, SEEK_SET);

	printf("%lu\n", file_size/sizeof(Record));
	
	/* read records into buffer */
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
		int current_id = 0;
		while(pointer < records_per_block){
			//printf("uid %d, uid2 %d\n", buffer[pointer].uid1, buffer[pointer].uid2);
			
			if(is_outdegree == 0){
				current_id = buffer[pointer].uid1;
			}else{
				current_id = buffer[pointer].uid2;
			}
			if(current_max_id == -1){
				current_max_id = current_id;
				
				current_max_followers = 1;
				id_count++;
			}else if(current_max_id == current_id){
				current_max_followers++;
			}else{
				if(previous_max_followers < current_max_followers && previous_max_id != current_max_id){
					previous_max_followers = current_max_followers;
					previous_max_id = current_max_id;
				}
				output_buffer[current_output_buffer_position].uid1 = current_max_id;
				output_buffer[current_output_buffer_position].uid2 = current_max_followers;
				current_output_buffer_position++;
				if(current_output_buffer_position == output_buffer_capacity){
					flush_outputbuffer(output_filename, output_buffer, current_output_buffer_position);
					current_output_buffer_position = 0;
				}
				current_max_id = current_id;
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
	output_buffer[current_output_buffer_position].uid1 = current_max_id;
	output_buffer[current_output_buffer_position].uid2 = current_max_followers;
	current_output_buffer_position++;
	flush_outputbuffer(output_filename, output_buffer, current_output_buffer_position);
	
	

	fclose (fp_read);
	free(output_buffer);
	free (buffer);
	return 0;
}

int flush_outputbuffer (char* filename, Record *output_buffer, int current_output_buffer_position) {
	FILE *fp_write;
	if (!(fp_write = fopen (filename , "a" ))){
		return 1;
	}
	fwrite(output_buffer, sizeof(Record), current_output_buffer_position, fp_write);
	fflush (fp_write);
	fclose(fp_write);
	return 0;
}
