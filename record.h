#include <stdlib.h>
#include <stdio.h>
#ifndef RECORD_H
#define RECORD_H

#define MAX_CHARS_PER_LINE 1024
#define MB (1024 * 1024)

typedef struct record {
	int uid1;
	int uid2;
} Record;

typedef struct q2_record {
	int uid1;
	int count;
	int indegree;
	int outdegree;
} Q2Record;

void csv_to_record(char* filename, char* block_size);

void write_buffer_to_disk(Record* buffer, int total_records, FILE *fp);
void write_result_to_file(char *filename, int block_size, float speed);

int flush_outputbuffer (char* filename, Record *output_buffer, int current_output_buffer_position);
int write_degree(char* input_filename, int block_size, int is_outdegree, char* output_filename);


#endif