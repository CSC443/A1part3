#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "merge.h"
#include "disk.h"
#include "record.h"

int merge_sort_join(int mem, int block_size);

int main (int argc, char *atgv[]){
    //sort file by uid1 with 200MB mem and optimal block size
    FILE *fp_read;
    int block_size = atoi(atgv[2]);
    static const int mem = 209715200 ;
    int block_num= (mem/block_size);
    disk_sort(atgv[1], mem, block_size, 1, "sorted_uid1.dat");
    disk_sort(atgv[1], mem, block_size, 2, "sorted_uid2.dat");

   // to do : scan sorted file uid1 and uid2, and calculate true friends;
   merge_sort_join(mem, block_size);
   return 0;
}


int merge_sort_join(int mem, int block_size){
    printf("%s\n","start meger join" );
    New_MergeManager * manager = (New_MergeManager *)calloc(1, sizeof(New_MergeManager));
    int records_per_block  = block_size/sizeof(Record);
    int mem_per_block = mem/block_size;
    int records_per_buffer = ((mem_per_block - 1)/2)*records_per_block * sizeof(Record);

    manager->input_buffer_capacity = records_per_buffer;
    manager->output_buffer_capacity = records_per_block;
    int current_input_file_positions[2];
    int current_input_buffer_positions[2];
    int total_input_buffer_elements[2];
    Record** input_buffers = malloc(2 * sizeof(Record *));
    strcpy(manager->output_file_name , "query1.dat");
    strcpy(manager->input_file_name_1, "sorted_uid1.dat");
    strcpy(manager->input_file_name_2, "sorted_uid2.dat");
    int i;
    for(i = 0; i < 2; i++){
        current_input_file_positions[i] = 0;
        current_input_buffer_positions[i] = 0;
        total_input_buffer_elements[i] = 0;
        input_buffers[i] = (Record *)calloc(manager->input_buffer_capacity, sizeof(Record));
    }   
    manager->output_buffer = (Record *)calloc(manager->output_buffer_capacity, sizeof(Record));
    manager->current_output_buffer_position = 0;
    manager->input_buffers = input_buffers;
    manager->current_input_file_positions = current_input_file_positions;
    manager->current_input_buffer_positions = current_input_buffer_positions;
    manager->total_input_buffer_elements = total_input_buffer_elements;
    new_merge_runs(manager);
    return 0 ;

}
