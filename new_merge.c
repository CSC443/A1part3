#include "merge.h"
#include "record.h"

//manager fields should be already initialized in the caller
int new_merge_runs (New_MergeManager * merger){	
	printf("new_merge_runs start\n");
	int result1; //stores SUCCESS/FAILURE returned at the end	
	int result2;
	//1. go in the loop through all input files and fill-in initial buffers
	if (new_init_merge (merger)!=SUCCESS)
		return FAILURE;
	// int t=0;
	// while (t<merger->total_input_buffer_elements[0]){
	// 	Record t1;
	// 	new_get_next_input_element(merger,0,0,&t1);
	// 	printf("%d\n",merger->total_input_buffer_elements[0]);
 //        printf("looking at table 1 :%d,%d\n", t1.uid1,t1.uid2);
 //        merger->current_input_buffer_positions[0]++;
 //        t=t+1;
	// }
    int i=0;
    int j=0; 
    while (i<merger->heap_capacity && j<merger->heap_capacity){
    	//printf("loop start\n");
    	Record r1;
    	Record r2;
        result1 = new_get_next_input_element(merger,i,0,&r1);
        if (result1==FAILURE)
			return FAILURE;
		if (result1==EMPTY)
		 	i = i + 1;
		    merger->current_input_file_positions[0]=0;
		    new_refill_buffer(merger,i,0);
            new_get_next_input_element(merger,i,0,&r1);
        result2 = new_get_next_input_element(merger,j,1,&r2); 
        if (result2==FAILURE)
			return FAILURE;
        if (result2 == EMPTY)
         	j = j + 1;
            merger->current_input_file_positions[1] = 0;
            new_refill_buffer(merger,j,1);
         	new_get_next_input_element(merger,j,1,&r2);
        printf("loop-i=%d record1:%d,%d  ,loop-j=%d  record2:%d,%d\n", i,r1.uid1,r1.uid2,j,r2.uid1,r2.uid2); 	
        if ((r1.uid1==r2.uid2) && (r1.uid2 == r2.uid1) && (r1.uid1 <=r1.uid2)){
        	printf("find match\n");
        	merger->current_input_buffer_positions[0]++;
        	merger->current_input_buffer_positions[1]++;
        	merger->output_buffer [merger->current_output_buffer_position].uid1=r1.uid1;
			merger->output_buffer [merger->current_output_buffer_position].uid2=r1.uid2;
		    merger->current_output_buffer_position++;
            
		    if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
		    	printf("go go go\n");
				if(new_flush_output_buffer(merger)!=SUCCESS) {
					return FAILURE;			
					merger->current_output_buffer_position=0;
				}	
			}
        }else if (r1.uid1 > r1.uid2) {
        	    merger->current_input_buffer_positions[0]++;
        	    
        } else if (r2.uid1 < r2.uid2){
        	    merger->current_input_buffer_positions[1]++;
        }
        else {
        	if (r1.uid1 < r2.uid2){
              merger->current_input_buffer_positions[0]++;
            } else{
              merger->current_input_buffer_positions[1]++;
            } 
        }

    }

	//flush what remains in output buffer
	if(merger->current_output_buffer_position > 0) {
		//printf("last buffer size%d uid1 %d uid2 %d\n", merger->current_output_buffer_position, merger->output_buffer[1].uid1, merger->output_buffer[1].uid2);
		if(new_flush_output_buffer(merger)!=SUCCESS)
			return FAILURE;
	}
	
	new_clean_up(merger);
	return SUCCESS;	
}


/*
** TO IMPLEMENT
*/

int new_init_merge (New_MergeManager * manager) {
	FILE *fp;
	FILE *fp1;
	char k[100];
	char * filename = (char *) calloc(121,sizeof(char));
	sprintf(k,"%d",0);
	strcat(filename,"uid1_");
	strcat(filename,k);
	strcat(filename,".dat");
    if (!(fp = fopen (filename , "rb" ))){
			//printf("here1\n");
			free(filename);
			return FAILURE;
			
	}else{
           fseek(fp, manager->current_input_file_positions[0]*sizeof(Record), SEEK_SET);
           int result = fread (manager->input_buffers[0], sizeof(Record), manager->input_buffer_capacity, fp);
			// if(result <= 0){
			// 	manager->current_heap_size--;
			// }
			manager->current_input_file_positions[0] =  result;
			manager->total_input_buffer_elements[0] = result;		
			//manager->current_input_buffer_positions[0]++;
	}	
    fclose(fp);
	free(filename);
// read for input buffer2 ; 

    char p[100];
	char * filename1 = (char *) calloc(121,sizeof(char));
	sprintf(p,"%d",0);
	strcat(filename1,"uid2_");
	strcat(filename1,p);
	strcat(filename1,".dat");
    if (!(fp1 = fopen (filename1 , "rb" ))){
			//printf("here1\n");
			free(filename1);
			return FAILURE;
			
	}else{
           fseek(fp1, manager->current_input_file_positions[1]*sizeof(Record), SEEK_SET);
           int result2 = fread (manager->input_buffers[1], sizeof(Record), manager->input_buffer_capacity, fp1);
			// if(result <= 0){
			// 	manager->current_heap_size--;
			// }
			manager->current_input_file_positions[1] =  result2;
			manager->total_input_buffer_elements[1] = result2;		
			//manager->current_input_buffer_positions[1]++;
	}	
    fclose(fp1);
	free(filename1);

	
	// int t=0;
	// while (t<manager->total_input_buffer_elements[0]){
	// 	Record t1;
	// 	new_get_next_input_element(manager,0,0,&t1);
	// 	printf("%d\n",manager->total_input_buffer_elements[0]);
 //        printf("looking at table 1 :%d,%d\n", t1.uid1,t1.uid2);
 //        manager->current_input_buffer_positions[0]++;
 //        t=t+1;
	// }
	return SUCCESS;
}

int new_flush_output_buffer (New_MergeManager * manager) {
	//printf("Write buffer to disk\n");
	FILE *fp_write;
	if (!(fp_write = fopen (manager->output_file_name , "a" ))){
		printf("cannnot open query1.dat\n");
		return FAILURE;
	}
	//fseek(fp_write, 0L, SEEK_END);
	fwrite(manager->output_buffer, sizeof(Record), manager->current_output_buffer_position, fp_write);
	fflush (fp_write);
	fclose(fp_write);
	manager->current_output_buffer_position = 0;
	return SUCCESS;
}

int new_get_next_input_element(New_MergeManager * manager, int file_num, int type, Record *result) {
	if(manager->current_input_buffer_positions[type] == manager->total_input_buffer_elements[type]){
		manager->current_input_buffer_positions[type] = 0;
		if(new_refill_buffer (manager, file_num, type)==FAILURE){
			return FAILURE;
		}
        
		if(manager->current_input_file_positions[type] == -1){
			return EMPTY;
		}
	}
	*result = manager->input_buffers[type][manager->current_input_buffer_positions[type]];
	//printf("filenumber %d uid1 %d uid2 %d\n", file_number, result->uid1, result->uid2);
	//manager->current_input_buffer_positions[type]++;
	
	return SUCCESS;
}

int new_refill_buffer (New_MergeManager * manager, int file_number, int type) {
	FILE *fp_read;
	char k[100];
	sprintf(k,"%d",file_number);
	char * filename = (char *) calloc(121,sizeof(char));
	if (type == 0 ){
		strcat(filename,"uid1_");
	}else{
        strcat(filename,"uid2_");
	}
	strcat(filename,k);
	strcat(filename,".dat");
	if (!(fp_read = fopen (filename , "rb" ))){
		free(filename);
		return FAILURE;
		
	}else{
	  if (type ==0){
		fseek(fp_read, manager->current_input_file_positions[0]*sizeof(Record), SEEK_SET);
		int result = fread (manager->input_buffers[0], sizeof(Record), manager->input_buffer_capacity, fp_read);
		if(result <= 0){
			manager->current_input_file_positions[0] = -1;
            //return EMPTY;
		}else{
			manager->current_input_file_positions[0]+= result;
			// if(result < manager->input_buffer_capacity){
			// 	manager->current_input_file_positions[file_number] = -1;
			// }
			manager->total_input_buffer_elements[0] = result;
		}
	  }else{
	  	fseek(fp_read, manager->current_input_file_positions[1]*sizeof(Record), SEEK_SET);
		int result = fread (manager->input_buffers[1], sizeof(Record), manager->input_buffer_capacity, fp_read);
		if(result <= 0){
			manager->current_input_file_positions[1] = -1;
			//return EMPTYl
		}else{
			manager->current_input_file_positions[1]+= result;
			// if(result < manager->input_buffer_capacity){
			// 	manager->current_input_file_positions[file_number] = -1;
			// }
			manager->total_input_buffer_elements[1] = result;
		}

	  }	
	}
	free(filename);
	fclose(fp_read);
	return SUCCESS;
}

void new_clean_up (New_MergeManager * merger) {
	//printf("clean up\n");
	//free(merger->heap);
	//printf("1\n");
	int i;
	for(i = 0; i < 2; i++){
		//printf("%d\n",i);
		free(merger->input_buffers[i]);
		//printf("after %d\n",i);
	}
	free(merger->input_buffers);
	//printf("2\n");
	free(merger->output_buffer);
	//printf("3\n");
	free(merger);
	//printf("4\n");
	
}
