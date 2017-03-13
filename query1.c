#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "record.h"
#include "merge.h"

int merge_sort(int buffer_num, int mem, int block_size);
/**
* Compares two records a and b 
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order: 
* positive: record a > record b
* negative: record a < record b
* zero: equal records
*/
int compare (const void *a, const void *b) {
 int a_uid2 = ((const struct record*)a)->uid2;
 int b_uid2= ((const struct record*)b)->uid2;
 return (a_uid2 - b_uid2);
}

int main (int argc, char *atgv[]){
    FILE *fp_read;
    int block_size = atoi(atgv[3]);
    int mem = atoi(atgv[2]);
    int block_num= (mem/block_size);
    if (!(fp_read = fopen (atgv[1], "rb" ))){
      return -1;
    }
    int records_per_block = block_size/sizeof(Record);
    int chunk_num = file_size/mem;
    int last_chunk_size = file_size - chunk_num*mem;
    int records_per_chunk = records_per_block*block_num;
    int records_last_chunk = last_chunk_size/sizeof(Record);
    int num_sublist;
    if (last_chunk_size!=0){
        num_sublist = chunk_num;

    }else{
        num_sublist = chunk_num + 1; 
    }
    if (((num_sublist+1)*block_size) > mem){
        perror("not enough memory");
        return 0;
    } 
    fseek(fp_read, 0L, SEEK_SET);
    int run = 0;
    //printf("chunk num is %d,block num per chunk  is %d, last_chunk_size is %d\n",chunk_num,block_num,last_chunk_size);
    
    while (run < chunk_num+1){
        FILE *fp_write;
        char k[chunk_num+1];
        sprintf(k,"%d",run);
        char * filename = (char *) calloc(20+chunk_num+1,sizeof(char));
        //printf("%s\n",filename );
        strcat(filename,"uid1sorted");
        strcat(filename,k);
        strcat(filename,".dat");
        //printf("%s\n",filename );
        //printf("%s\n",strcat(strcat("sorted",k), ".dat") );
        fp_write = fopen( filename, "wb");
        if(fp_write == NULL){
        perror("Error opening file");
        return -1;
        }   
        if (run == chunk_num) {
            if (last_chunk_size== 0){
                   break;

            }else{
                Record * buffer = (Record *) calloc (records_last_chunk, sizeof (Record));
                int r1 = fread (buffer, sizeof(Record), records_last_chunk, fp_read);
                if (r1==0){
                    perror("read buffer failed\n");
                }
                qsort (buffer, records_last_chunk, sizeof(Record), compare);
                fwrite(buffer, sizeof(Record), records_last_chunk, fp_write);
                fflush (fp_write);
                free (buffer);
            }            
        }
        else{
        Record * buffer = (Record *) calloc (records_per_chunk, sizeof (Record));
        //printf("run 1 is %d\n", run );
        int r = fread (buffer, sizeof(Record), records_per_chunk, fp_read);
        if (r==0){
                    perror("read buffer failed\n");
        }
        qsort (buffer, records_per_chunk, sizeof(Record), compare);
        fwrite(buffer, sizeof(Record), records_per_chunk, fp_write);
        fflush (fp_write);

        free(buffer);
        
       }
       free(filename);
       fclose(fp_write);

       run++; 

   }
   fclose(fp_read);

   if (!(fp_read = fopen (atgv[1], "rb" ))){
      return -1;
    }
   disk_sort(FILE *fp_read,int mem, int block_size);

   
    FILE *fp_read2;
    if (!(fp_read2 = fopen ("sorted_merge.dat", "rb" ))){
      return -1;
    }
    
    fseek(fp_read2, 0L, SEEK_SET);
    int run = 0;
    //printf("chunk num is %d,block num per chunk  is %d, last_chunk_size is %d\n",chunk_num,block_num,last_chunk_size);
    
    while (run < chunk_num+1){
        FILE *fp_write;
        char k[chunk_num+1];
        sprintf(k,"%d",run);
        char * filename = (char *) calloc(20+chunk_num+1,sizeof(char));
        //printf("%s\n",filename );
        strcat(filename,"uid2sorted");
        strcat(filename,k);
        strcat(filename,".dat");
        //printf("%s\n",filename );
        //printf("%s\n",strcat(strcat("sorted",k), ".dat") );
        fp_write = fopen( filename, "wb");
        if(fp_write == NULL){
        perror("Error opening file");
        return -1;
        }   
        if (run == chunk_num) {
            if (last_chunk_size== 0){
                   break;

            }else{
                Record * buffer = (Record *) calloc (records_last_chunk, sizeof (Record));
                int r1 = fread (buffer, sizeof(Record), records_last_chunk, fp_read2);
                if (r1==0){
                    perror("read buffer failed\n");
                }
                qsort (buffer, records_last_chunk, sizeof(Record), compare);
                fwrite(buffer, sizeof(Record), records_last_chunk, fp_write);
                fflush (fp_write);
                free (buffer);
            }            
        }
        else{
        Record * buffer = (Record *) calloc (records_per_chunk, sizeof (Record));
        //printf("run 1 is %d\n", run );
        int r = fread (buffer, sizeof(Record), records_per_chunk, fp_read2);
        if (r==0){
                    perror("read buffer failed\n");
        }
        qsort (buffer, records_per_chunk, sizeof(Record), compare);
        fwrite(buffer, sizeof(Record), records_per_chunk, fp_write);
        fflush (fp_write);

        free(buffer);
        
       }
       free(filename);
       fclose(fp_write);

       run++; 

   }
   fclose(fp_read2);
   // to do : scan sorted file uid1 and uid2, and calculate true friends;

   return 0;
}


int disk_sort(FILE *fp_read,int mem, int block_size){
 	//printf("start disk_sort\n");
 	//FILE *fp_write;
 	//FILE *fp_read;
    //int block_size = atoi(atgv[3]);
   // int mem = atoi(atgv[2]);
    
    int block_num= (mem/block_size);
    //printf("%d,%d\n", mem,block_size);
    //printf("This is block_num %d\n",block_num);
    //printf("size of record:%d\n",sizeof(Record));
    //int num_records = mem / sizeof(Record);

	//if (!(fp_read = fopen ( , "rb" ))){
	//	return -1;
	//}	

	// find file size
    fseek(fp_read, 0L, SEEK_END);
	int file_size = ftell(fp_read);
	//printf("file size %d\n", file_size);

	//int total_records = file_size/sizeof(Record);
    int records_per_block = block_size/sizeof(Record);
	int chunk_num = file_size/(block_num*block_size); 
    int last_chunk_size = file_size - chunk_num*(block_num*block_size);
    int records_per_chunk = records_per_block*block_num;
    int records_last_chunk = last_chunk_size/sizeof(Record);
    int num_sublist;
    if (last_chunk_size!=0){
    	num_sublist = chunk_num;

    }else{
        num_sublist = chunk_num + 1; 
    }
    //printf("%d\n",num_sublist);
    //printf("required memory for phase 2 is %d, mem is %d\n",(num_sublist+1)*block_size,mem );
    if (((num_sublist+1)*block_size) > mem){
    	perror("not enough memory");
    	return 0;
    } 
	//set pointe to the begining of the file
	fseek(fp_read, 0L, SEEK_SET);
    int run = 0;
    //printf("chunk num is %d,block num per chunk  is %d, last_chunk_size is %d\n",chunk_num,block_num,last_chunk_size);
    
    while (run < chunk_num+1){
    	FILE *fp_write;
    	char k[chunk_num+1];
		sprintf(k,"%d",run);
		char * filename = (char *) calloc(20+chunk_num+1,sizeof(char));
		//printf("%s\n",filename );
		strcat(filename,"sorted");
		strcat(filename,k);
		strcat(filename,".dat");
		//printf("%s\n",filename );
		//printf("%s\n",strcat(strcat("sorted",k), ".dat") );
		fp_write = fopen( filename, "wb");
		if(fp_write == NULL){
	 	perror("Error opening file");
	 	return -1;
	    }	
        if (run == chunk_num) {
        	if (last_chunk_size== 0){
                   break;

        	}else{
        		Record * buffer = (Record *) calloc (records_last_chunk, sizeof (Record));
        		int r1 = fread (buffer, sizeof(Record), records_last_chunk, fp_read);
                if (r1==0){
                    perror("read buffer failed\n");
                }
				qsort (buffer, records_last_chunk, sizeof(Record), compare);
				fwrite(buffer, sizeof(Record), records_last_chunk, fp_write);
				fflush (fp_write);
                free (buffer);
        	}            
        }
        else{
		Record * buffer = (Record *) calloc (records_per_chunk, sizeof (Record));
		//printf("run 1 is %d\n", run );
		int r = fread (buffer, sizeof(Record), records_per_chunk, fp_read);
		if (r==0){
                    perror("read buffer failed\n");
        }
		qsort (buffer, records_per_chunk, sizeof(Record), compare);
		fwrite(buffer, sizeof(Record), records_per_chunk, fp_write);
		fflush (fp_write);

		free(buffer);
		
	   }
	   free(filename);
	   fclose(fp_write);

	   run++; 

   }
   fclose(fp_read);
   merge_sort(num_sublist+1, mem, block_size);
   return 0;
   
 }

 int merge_sort(int buffer_num, int mem, int block_size){
 	MergeManager * manager = (MergeManager *)calloc(1, sizeof(MergeManager));

 	int records_per_block  = block_size/sizeof(Record);
 	int block_num = mem/block_size;
 	int records_per_buffer = records_per_block * (block_num / (buffer_num + 1));


 	manager->heap_capacity = buffer_num;
 	manager->heap = (HeapElement *)calloc(buffer_num, sizeof(HeapElement));
 	strcpy(manager->output_file_name , "sorted_merge.dat");
 	strcpy(manager->input_prefix, "sorted");
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
 	Record** input_buffers = malloc(buffer_num * sizeof(Record *));
 	int i;
 	for(i = 0; i < manager->heap_capacity; i++){
 		input_file_numbers[i] = i;
 		current_input_file_positions[i] = 0;
 		current_input_buffer_positions[i] = 0;
 		total_input_buffer_elements[i] = 0;
 		input_buffers[i] = (Record *)calloc(manager->input_buffer_capacity, sizeof(Record));
 	}	
 	manager->input_file_numbers = input_file_numbers;
 	manager->output_buffer = (Record *)calloc(manager->output_buffer_capacity, sizeof(Record));
 	manager->current_output_buffer_position = 0;
 	manager->input_buffers = input_buffers;
 	manager->current_input_file_positions = current_input_file_positions;
 	manager->current_input_buffer_positions = current_input_buffer_positions;
 	manager->total_input_buffer_elements = total_input_buffer_elements;
 	merge_runs(manager);
 	return 0;
 }
