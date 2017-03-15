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

void csv_to_record(char* filename, char* block_size);

void write_buffer_to_disk(Record* buffer, int total_records, FILE *fp);
void write_result_to_file(char *filename, int block_size, float speed);



#endif