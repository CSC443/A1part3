#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "merge.h"
#include "disk.h"
#include "record.h"


/**
* Compares two records a and b 
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order: 
* positive: Q2Record a > Q2Record b
* negative: Q2Record a < Q2Record b
* zero: equal records
*/


int compare_celebrity(const void *a, const void *b) {
    int a_count = ((const struct q2_record*)a)->count;
    int b_count = ((const struct q2_record*)b)->count;
    if(b_count > a_count){
        return 1;
    }
    return -1;

}

// int main(int argc, char *atgv[]){
//     int block_size = atoi(atgv[3]);
//     int mem = atoi(atgv[2]);
//     disk_sort(atgv[1], mem, block_size, atoi(atgv[4]), atgv[5]);
// }

int disk_sort_q2(char* filename, int mem, int block_size, char* output_filename){

 	FILE *fp_read;
    
    int block_num= (mem/block_size);

	if (!(fp_read = fopen (filename , "rb" ))){
		return -1;
	}	

	// find file size
    fseek(fp_read, 0L, SEEK_END);
	int file_size = ftell(fp_read);
    int records_per_block = block_size/sizeof(Q2Record);
	int chunk_num = file_size/mem;
    int last_chunk_size = file_size - chunk_num*mem;
    int records_per_chunk = records_per_block*block_num;
    int records_last_chunk = last_chunk_size/sizeof(Q2Record);
    int num_sublist;
    //printf("records_per_chunk:%d\n",records_per_chunk);
    //printf("size of Q2Record is %lu\n" , sizeof(Q2Record));
    if (last_chunk_size!=0){
    	num_sublist = chunk_num;

    }else{
        num_sublist = chunk_num + 1; 
    }
    if (((num_sublist+1)*block_size) > mem){
    	perror("not enough memory");
    	return 0;
    } 
	//set pointe to the begining of the file
	fseek(fp_read, 0L, SEEK_SET);
    int run = 0;
    while (run < chunk_num+1){
    	FILE *fp_write;
    	char k[chunk_num+1];
		sprintf(k,"%d",run);
		char * filename = (char *) calloc(20+chunk_num+1,sizeof(char));
		//printf("%s\n",filename );

        strcat(filename,"sorted3_");

		strcat(filename,k);        
        
		strcat(filename,".dat");
		//printf("%s\n",filename );
		//printf("%s\n",strcat(strcat("sorted",k), ".dat") );
		fp_write = fopen(filename, "wb");
		if(fp_write == NULL){
	 	     perror("Error opening file");
	 	     return -1;
	    }	
        if (run == chunk_num) {
        	if (last_chunk_size== 0){
                   break;

        	}else{
        		Q2Record * buffer = (Q2Record *) calloc (records_last_chunk, sizeof (Q2Record));
        		int r1 = fread (buffer, sizeof(Q2Record), records_last_chunk, fp_read);
                if (r1==0){
                    perror("read buffer failed\n");
                }

                qsort (buffer, records_last_chunk, sizeof(Q2Record), compare_celebrity);
                

				fwrite(buffer, sizeof(Q2Record), records_last_chunk, fp_write);
				fflush (fp_write);
                free (buffer);
        	}            
        }
        else{
		Q2Record * buffer = (Q2Record *) calloc (records_per_chunk, sizeof (Q2Record));
		//printf("run 1 is %d\n", run );
		int r = fread (buffer, sizeof(Q2Record), records_per_chunk, fp_read);
		if (r==0){
                    perror("read buffer failed\n");
        }

        qsort (buffer, records_per_chunk, sizeof(Q2Record), compare_celebrity);
        
//		qsort (buffer, records_per_chunk, sizeof(Q2Record), compare);
		fwrite(buffer, sizeof(Q2Record), records_per_chunk, fp_write);
		fflush (fp_write);

		free(buffer);
		
	   }
	   free(filename);
	   fclose(fp_write);

	   run++; 

   }
   fclose(fp_read);
   merge_sort_q2(num_sublist+1, mem, block_size, output_filename);
   return 0;
   
 }

 int merge_sort_q2(int buffer_num, int mem, int block_size, char* output_filename){
    //printf("merge\n");
 	Q2MergeManager * manager = (Q2MergeManager *)calloc(1, sizeof(Q2MergeManager));

 	int records_per_block  = block_size/sizeof(Q2Record);
 	int block_num = mem/block_size;
 	int records_per_buffer = records_per_block * (block_num / (buffer_num + 1));

 	manager->heap_capacity = buffer_num;
 	manager->heap = (Q2HeapElement *)calloc(buffer_num, sizeof(Q2HeapElement));
 	strcpy(manager->output_file_name , output_filename);

    strcpy(manager->input_prefix, "sorted3_"); 
 	
 	if(block_num % (buffer_num + 1) > 0){
 		int extra_block = block_num % (buffer_num + 1);
 		manager->output_buffer_capacity = records_per_buffer + extra_block * records_per_block;
 	}else{
 		manager->output_buffer_capacity = records_per_buffer;
 	}
 	
 	manager->input_buffer_capacity = records_per_buffer;
 	
 	int input_file_numbers[buffer_num];
 	int current_input_file_positions[buffer_num];
 	int current_input_buffer_positions[buffer_num];
 	int total_input_buffer_elements[buffer_num];
 	Q2Record** input_buffers = malloc(buffer_num * sizeof(Q2Record *));
 	int i;
 	for(i = 0; i < manager->heap_capacity; i++){
 		input_file_numbers[i] = i;
 		current_input_file_positions[i] = 0;
 		current_input_buffer_positions[i] = 0;
 		total_input_buffer_elements[i] = 0;
 		input_buffers[i] = (Q2Record *)calloc(manager->input_buffer_capacity, sizeof(Q2Record));
 	}	
 	manager->input_file_numbers = input_file_numbers;
 	manager->output_buffer = (Q2Record *)calloc(manager->output_buffer_capacity, sizeof(Q2Record));
 	manager->current_output_buffer_position = 0;
 	manager->input_buffers = input_buffers;
 	manager->current_input_file_positions = current_input_file_positions;
 	manager->current_input_buffer_positions = current_input_buffer_positions;
 	manager->total_input_buffer_elements = total_input_buffer_elements;
 	q2_merge_runs(manager);
 	return 0;
 }
