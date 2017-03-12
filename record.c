#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include "record.h"

void csv_to_record(char* filename, char* block_size){
	struct timeb t_begin, t_end;
	long time_spent_ms;
	FILE *fp, *fp_write;
	char current_line[MAX_CHARS_PER_LINE];
	char *line;
	int records_per_block = atoi(block_size) / sizeof(Record);
	Record *buffer = (Record *) calloc (records_per_block, sizeof (Record));
	//open  csv file to read
	fp = fopen(filename, "r");
	if(fp == NULL){
		perror("Error opening file");
		return;
	}

	//open dat file to write 
	fp_write = fopen("data7.dat", "wb");
	if(fp_write == NULL){
		perror("Error opening file");
		return;
	}	

	//track number of records have been converted in each block cycle.
	int i = 0;

	//Parse the lines in csv file to an array of array chars
	int total_records = 0;
    ftime(&t_begin); 
	while((line = fgets(current_line, MAX_CHARS_PER_LINE, fp)) != NULL){
		line [strcspn (line, "\r\n")] = '\0';
		char *attr = strtok(line, ",");
		int j = 0;
		while(attr){

			if(j == 0){
				buffer[i].uid1 = atoi(attr);
			}else{
				buffer[i].uid2 = atoi(attr);
			}		
			attr = strtok(NULL, ",");
			j++;
		}
		i++;
		total_records++;
		
		//When buffer is full, write to disk
		if(i == records_per_block){
			write_buffer_to_disk(buffer, records_per_block, fp_write);
			i = 0;

		}	

	}

	//When buffer is smaller than block size after covert all the record.
	if(i < records_per_block){
		write_buffer_to_disk(buffer, i, fp_write);
	}

    ftime(&t_end); 

	time_spent_ms = (long) (1000 *(t_end.time - t_begin.time)
     + (t_end.millitm - t_begin.millitm));

	printf ("Data rate: %.3f MBPS\n", ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/(1024*1024));
	printf("total records: %d\n",(total_records));	
	write_result_to_file("write_block_seq.txt", atoi(block_size), ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/(1024*1024));
	
	free(buffer);
	fclose (fp);
	fclose (fp_write);
	return;

}


void write_buffer_to_disk(Record* buffer, int total_records, FILE *fp){

	if(fp){
		fwrite(buffer, sizeof(Record), total_records, fp);
		fflush (fp);
	}else{
		return;
	}

}

void write_result_to_file(char *filename, int block_size, float speed){
	FILE *fp;
	if (!(fp = fopen (filename , "a" ))){
		return;
	}
    fprintf(fp, "%d, %.3f\n", block_size, speed);
    fclose(fp);

}

