#include <stdlib.h>
#include <stdio.h>
#ifndef DISK_H
#define DISK_H
 int merge_sort(int buffer_num, int mem, int block_size, char* output_filename, int sort_uid);
 int disk_sort(char* filename, int mem, int block_size, int sort_uid, char* output_filename);
#endif