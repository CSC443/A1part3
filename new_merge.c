#include "merge.h"
#include "record.h"

//manager fields should be already initialized in the caller
int new_merge_runs (New_MergeManager * merger){	
	//printf("new_merge_runs start\n");	
	//1. go in the loop through all input files and fill-in initial buffers
	if (new_init_merge (merger)!=SUCCESS)
		return FAILURE;
	Record *r1 = (Record *) calloc (1, sizeof (Record)) ;
    Record *r2 = (Record *) calloc (1, sizeof (Record)) ;
    int count = 0;

    if(merger->is_query1 == 0){
    	query1_join(r1, r2, merger, &count);
    }else{
    	query2_join(r1, r2, merger, &count);
    }

    
	free(r1);
	free(r2);
	//flush what remains in output buffer
	if(merger->current_output_buffer_position > 0) {
		//printf("last buffer size%d uid1 %d uid2 %d\n", merger->current_output_buffer_position, merger->output_buffer[1].uid1, merger->output_buffer[1].uid2);
		if(new_flush_output_buffer(merger)!=SUCCESS)
			return FAILURE;
	}
	if(merger->is_query1 == 0){
		printf("total number match %d\n", count);
	}else{
		printf("distinct user number: %d\n", count);
	}
	new_clean_up(merger);
	return SUCCESS;	
}

int query2_join(Record *r1, Record *r2, New_MergeManager * merger, int *count){
	int result1; //stores SUCCESS/FAILURE returned at the end	
	int result2;
	while (merger->current_input_file_positions[0] != -1 || merger->current_input_file_positions[1] != -1){
        if(merger->current_input_file_positions[0] != -1){
        	result1 = new_get_next_input_element(merger,0,r1);
        }else{
        	result1 = EMPTY;
        }
        
        if (result1==FAILURE)
			return FAILURE;
        if(merger->current_input_file_positions[1] != -1){
        	result2 = new_get_next_input_element(merger,1,r2);
        }else{
        	result2 = EMPTY;
        }
        if (result2==FAILURE)
			return FAILURE;
		if(result1 == EMPTY && result2 == EMPTY){
			break;
		}
		if(merger->current_input_file_positions[0] == -1 || r1->uid1 > r2->uid1){
			merger->current_input_buffer_positions[1]++;
	    	merger->output_buffer_q2 [merger->current_output_buffer_position].uid1=r2->uid1;
			merger->output_buffer_q2 [merger->current_output_buffer_position].count= r2->uid2;
			merger->output_buffer_q2 [merger->current_output_buffer_position].indegree = r2->uid2;
			merger->output_buffer_q2 [merger->current_output_buffer_position].outdegree = 0;
		    merger->current_output_buffer_position++;
		    if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
				if(new_flush_output_buffer(merger)!=SUCCESS) {
					return FAILURE;			
					merger->current_output_buffer_position=0;
				}	
			}
		}else if(merger->current_input_file_positions[1] == -1 || r1->uid1 < r2->uid1){
			merger->current_input_buffer_positions[0]++;
	    	merger->output_buffer_q2 [merger->current_output_buffer_position].uid1=r1->uid1;
			merger->output_buffer_q2 [merger->current_output_buffer_position].count= -r1->uid2;
			merger->output_buffer_q2 [merger->current_output_buffer_position].indegree = 0;
			merger->output_buffer_q2 [merger->current_output_buffer_position].outdegree = r1->uid2;
		    merger->current_output_buffer_position++;
		    if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
				if(new_flush_output_buffer(merger)!=SUCCESS) {
					return FAILURE;			
					merger->current_output_buffer_position=0;
				}	
			}
		}else if (r1->uid1==r2->uid1){
	    	merger->current_input_buffer_positions[0]++;
	    	merger->current_input_buffer_positions[1]++;
	    	merger->output_buffer_q2 [merger->current_output_buffer_position].uid1=r1->uid1;
			merger->output_buffer_q2 [merger->current_output_buffer_position].count=r2->uid2 - r1->uid2;
			merger->output_buffer_q2 [merger->current_output_buffer_position].indegree = r2->uid2;
			merger->output_buffer_q2 [merger->current_output_buffer_position].outdegree = r1->uid2;
		    merger->current_output_buffer_position++;
	    	
	        
		    if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
				if(new_flush_output_buffer(merger)!=SUCCESS) {
					return FAILURE;			
					merger->current_output_buffer_position=0;
				}	
			}
	    }
	    *count = *count + 1;
        

    }
	
    return SUCCESS;	
}

int query1_join(Record *r1, Record *r2, New_MergeManager * merger, int *count){
	int result1; //stores SUCCESS/FAILURE returned at the end	
	int result2;
	while (merger->current_input_file_positions[0] != -1 && merger->current_input_file_positions[1] != -1){
        if(merger->current_input_file_positions[0] != -1){
        	result1 = new_get_next_input_element(merger,0,r1);
        }else{
        	result1 = EMPTY;
        }
        
        if (result1==FAILURE)
			return FAILURE;
        if(merger->current_input_file_positions[1] != -1){
        	result2 = new_get_next_input_element(merger,1,r2);
        }else{
        	result2 = EMPTY;
        }
        if (result2==FAILURE)
			return FAILURE;
		if(result1 == EMPTY && result2 == EMPTY){
			break;
		}
		if ((r1->uid1==r2->uid2) && (r1->uid2 == r2->uid1) && (r1->uid1 < r1->uid2)){
	    	//printf("find match\n");
	    	*count = *count + 1;
	    	merger->current_input_buffer_positions[0]++;
	    	merger->current_input_buffer_positions[1]++;
	    	merger->output_buffer [merger->current_output_buffer_position].uid1=r1->uid1;
			merger->output_buffer [merger->current_output_buffer_position].uid2=r1->uid2;
		    merger->current_output_buffer_position++;
	    	//printf("record1:%d,%d  ,record2:%d,%d\n", r1->uid1,r1->uid2,r2->uid1,r2->uid2); 	
	        
		    if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
		    	//printf("go go go\n");
				if(new_flush_output_buffer(merger)!=SUCCESS) {
					return FAILURE;			
					merger->current_output_buffer_position=0;
				}	
			}
	    }else if (r1->uid1 < r2->uid2) {
	    	    merger->current_input_buffer_positions[0]++;
	    	    
	    } else if (r1->uid1 > r2->uid2){
	    	    merger->current_input_buffer_positions[1]++;
	    }
	    else {
	    	if (r1->uid2 < r2->uid1){
	          merger->current_input_buffer_positions[0]++;
	        } else{
	          merger->current_input_buffer_positions[1]++;
	        } 
	    }

        

    }
	
    return SUCCESS;	
}

/*
** TO IMPLEMENT
*/

int new_init_merge (New_MergeManager * manager) {
	FILE *fp1;
	FILE *fp2;

    if (!(fp1 = fopen (manager->input_file_name_1, "rb" ))){
		return FAILURE;
			
	}else{
           fseek(fp1, manager->current_input_file_positions[0]*sizeof(Record), SEEK_SET);
           int result = fread (manager->input_buffers[0], sizeof(Record), manager->input_buffer_capacity, fp1);
			manager->current_input_file_positions[0] =  result;
			manager->total_input_buffer_elements[0] = result;		
	}	
    fclose(fp1);

// read for input buffer2 ; 
    if (!(fp2 = fopen (manager->input_file_name_2, "rb" ))){
		return FAILURE;
			
	}else{
           fseek(fp2, manager->current_input_file_positions[1]*sizeof(Record), SEEK_SET);
           int result2 = fread (manager->input_buffers[1], sizeof(Record), manager->input_buffer_capacity, fp2);
			manager->current_input_file_positions[1] =  result2;
			manager->total_input_buffer_elements[1] = result2;		
	}	
    fclose(fp2);

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
	if(manager->is_query1 == 0){
		fwrite(manager->output_buffer, sizeof(Record), manager->current_output_buffer_position, fp_write);
	}else{
		fwrite(manager->output_buffer_q2, sizeof(Q2Record), manager->current_output_buffer_position, fp_write);
	}
	
	fflush (fp_write);
	fclose(fp_write);
	manager->current_output_buffer_position = 0;
	return SUCCESS;
}

int new_get_next_input_element(New_MergeManager * manager, int type, Record *result) {
	if(manager->current_input_buffer_positions[type] == manager->total_input_buffer_elements[type]){
		manager->current_input_buffer_positions[type] = 0;
		if(new_refill_buffer (manager, type)==FAILURE){
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

int new_refill_buffer (New_MergeManager * manager, int type) {
	
	

	if(type == 0){
		FILE *fp_read;
		if (!(fp_read = fopen (manager->input_file_name_1, "rb" ))){
			return FAILURE;
			
		}else{
           fseek(fp_read, manager->current_input_file_positions[0]*sizeof(Record), SEEK_SET);
           int result = fread (manager->input_buffers[0], sizeof(Record), manager->input_buffer_capacity, fp_read);
           if(result <= 0){
				manager->current_input_file_positions[0] = -1;
			}
			manager->current_input_file_positions[0] +=  result;
			manager->total_input_buffer_elements[0] = result;		
		}
		fclose(fp_read);
	}
	if(type == 1){
		FILE *fp_read;
		if (!(fp_read = fopen (manager->input_file_name_2, "rb" ))){
			return FAILURE;
			
		}else{
           	fseek(fp_read, manager->current_input_file_positions[1]*sizeof(Record), SEEK_SET);
           	int result = fread (manager->input_buffers[1], sizeof(Record), manager->input_buffer_capacity, fp_read);
           	if(result <= 0){
				manager->current_input_file_positions[1] = -1;
			}
			manager->current_input_file_positions[1] +=  result;
			manager->total_input_buffer_elements[1] = result;		
		}
		fclose(fp_read);
	}
		
	return SUCCESS;
}

void new_clean_up (New_MergeManager * merger) {
	int i;
	for(i = 0; i < 2; i++){
		free(merger->input_buffers[i]);
	}
	free(merger->input_buffers);
	if(merger->is_query1 == 0){
		free(merger->output_buffer);
	}else{
		free(merger->output_buffer_q2);
	}
	
	free(merger);
	
}
