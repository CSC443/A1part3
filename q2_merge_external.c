#include "merge.h"
#include "record.h"

//manager fields should be already initialized in the caller
int q2_merge_runs (Q2MergeManager * merger){	
	int  result; //stores SUCCESS/FAILURE returned at the end	
	
	//1. go in the loop through all input files and fill-in initial buffers
	if (q2_init_merge (merger)!=SUCCESS)
		return FAILURE;

	while (merger->current_heap_size > 0) { //heap is not empty
		Q2HeapElement smallest;
		Q2Record next; //here next comes from input buffer
		
		if(q2_get_top_heap_element (merger, &smallest)!=SUCCESS)
			return FAILURE;

		result = q2_get_next_input_element (merger, smallest.run_id, &next);
		if (result==FAILURE)
			return FAILURE;

		if(result==SUCCESS) {//next element exists, may also return EMPTY
			if(q2_insert_into_heap (merger, smallest.run_id, &next)!=SUCCESS)
				return FAILURE;
		}		


		merger->output_buffer [merger->current_output_buffer_position].uid1=smallest.UID1;
		merger->output_buffer [merger->current_output_buffer_position].count=smallest.COUNT;
		merger->output_buffer [merger->current_output_buffer_position].indegree=smallest.INDEGREE;
		merger->output_buffer [merger->current_output_buffer_position].outdegree=smallest.OUTDEGREE;
		
		merger->current_output_buffer_position++;

        //staying on the last slot of the output buffer - next will cause overflow
		if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
			if(q2_flush_output_buffer(merger)!=SUCCESS) {
				return FAILURE;			
				merger->current_output_buffer_position=0;
			}	
		}
	
	}

	
	//flush what remains in output buffer
	if(merger->current_output_buffer_position > 0) {
		//printf("last buffer size%d uid1 %d uid2 %d\n", merger->current_output_buffer_position, merger->output_buffer[1].uid1, merger->output_buffer[1].uid2);
		if(q2_flush_output_buffer(merger)!=SUCCESS)
			return FAILURE;
	}
	
	q2_clean_up(merger);
	return SUCCESS;	
}


int q2_get_top_heap_element (Q2MergeManager * merger, Q2HeapElement * result){
	Q2HeapElement item;
	int child, parent;

	if(merger->current_heap_size == 0){
		printf( "UNEXPECTED ERROR: popping top element from an empty heap\n");
		return FAILURE;
	}

	*result=merger->heap[0];  //to be returned

	//now we need to reorganize heap - keep the smallest on top
	item = merger->heap [--merger->current_heap_size]; // to be reinserted 

	parent =0;

	while ((child = (2 * parent) + 1) < merger->current_heap_size) {
		// if there are two children, compare them 
		if (child + 1 < merger->current_heap_size && 
				(q2_compare_heap_elements(&(merger->heap[child]),&(merger->heap[child + 1]))>0)) 
			++child;
		
		// compare item with the larger 
		if (q2_compare_heap_elements(&item, &(merger->heap[child]))>0) {
			merger->heap[parent] = merger->heap[child];
			parent = child;
		} 
		else 
			break;
	}
	
	merger->heap[parent] = item;
	
	return SUCCESS;
}

int q2_insert_into_heap (Q2MergeManager * merger, int run_id, Q2Record *input){

	Q2HeapElement new_heap_element;
	int child, parent;
	//printf("heap uid1 %d uid2 %d\n", input->uid1, input->uid2);

	new_heap_element.UID1 = input->uid1;
	new_heap_element.COUNT = input->count;
	new_heap_element.INDEGREE = input->indegree;
	new_heap_element.OUTDEGREE = input->outdegree;
	new_heap_element.run_id = run_id;
	
	if (merger->current_heap_size == merger->heap_capacity) {
		printf( "Unexpected ERROR: heap is full\n");
		return FAILURE;
	}
  	
	child = merger->current_heap_size++; /* the next available slot in the heap */
	
	while (child > 0) {
		parent = (child - 1) / 2;
		if (q2_compare_heap_elements(&(merger->heap[parent]),&new_heap_element)>0) {
			merger->heap[child] = merger->heap[parent];
			child = parent;
		} 
		else 
			break;
	}
	
	
	merger->heap[child]= new_heap_element;	
	return SUCCESS;
}


/*
** TO IMPLEMENT
*/

int q2_init_merge (Q2MergeManager * manager) {
	FILE *fp;
	
	//printf("%d\n", manager->heap_capacity);
	int i;
	for(i = 0; i < manager->heap_capacity; i++){
		char k[100];
		char * filename = (char *) calloc(121,sizeof(char));
		sprintf(k,"%d",manager->input_file_numbers[i]);
		strcat(filename,manager->input_prefix);
		strcat(filename,k);
		strcat(filename,".dat");
		//printf("%s\n",filename );
		if (!(fp = fopen (filename , "rb" ))){
			//printf("here1\n");
			free(filename);
			return FAILURE;
			
		}else{
			//printf("here\n");
			fseek(fp, manager->current_input_file_positions[i]*sizeof(Q2Record), SEEK_SET);
			int result = fread (manager->input_buffers[i], sizeof(Q2Record), manager->input_buffer_capacity, fp);
			if(result <= 0){
				manager->current_heap_size--;
			}
			manager->current_input_file_positions[i] =  result;
			manager->total_input_buffer_elements[i] = result;		
			q2_insert_into_heap(manager, manager->input_file_numbers[i], &manager->input_buffers[i][manager->current_input_buffer_positions[i]]);
			manager->current_input_buffer_positions[i]++;
		}
		fclose(fp);
		free(filename);
	}
	
	//printf("end init\n");
	
	return SUCCESS;
}

int q2_flush_output_buffer (Q2MergeManager * manager) {
	//printf("Write buffer to disk\n");
	FILE *fp_write;
	if (!(fp_write = fopen (manager->output_file_name , "a" ))){
		return FAILURE;
	}
	//fseek(fp_write, 0L, SEEK_END);
	fwrite(manager->output_buffer, sizeof(Q2Record), manager->current_output_buffer_position, fp_write);
	fflush (fp_write);
	fclose(fp_write);
	manager->current_output_buffer_position = 0;
	return SUCCESS;
}

int q2_get_next_input_element(Q2MergeManager * manager, int file_number, Q2Record *result) {
	
	if(manager->current_input_buffer_positions[file_number] == manager->total_input_buffer_elements[file_number]){
		manager->current_input_buffer_positions[file_number] = 0;
		
			
		if(q2_refill_buffer (manager, file_number)!=SUCCESS){
			return FAILURE;
		}

		if(manager->current_input_file_positions[file_number] == -1){
			return EMPTY;
		}
	}
	*result = manager->input_buffers[file_number][manager->current_input_buffer_positions[file_number]];
	//printf("filenumber %d uid1 %d uid2 %d\n", file_number, result->uid1, result->uid2);
	manager->current_input_buffer_positions[file_number]++;
	
	return SUCCESS;
}

int q2_refill_buffer (Q2MergeManager * manager, int file_number) {
	FILE *fp_read;
	char k[100];
	sprintf(k,"%d",manager->input_file_numbers[file_number]);
	char * filename = (char *) calloc(121,sizeof(char));
	strcat(filename,manager->input_prefix);
	strcat(filename,k);
	strcat(filename,".dat");
	if (!(fp_read = fopen (filename , "rb" ))){
		free(filename);
		return FAILURE;
		
	}else{
		fseek(fp_read, manager->current_input_file_positions[file_number]*sizeof(Q2Record), SEEK_SET);
		int result = fread (manager->input_buffers[file_number], sizeof(Q2Record), manager->input_buffer_capacity, fp_read);
		if(result <= 0){
			manager->current_input_file_positions[file_number] = -1;
		}else{
			manager->current_input_file_positions[file_number]+= result;
			// if(result < manager->input_buffer_capacity){
			// 	manager->current_input_file_positions[file_number] = -1;
			// }
			manager->total_input_buffer_elements[file_number] = result;
		}
		
	}
	free(filename);
	fclose(fp_read);
	return SUCCESS;
}

void q2_clean_up (Q2MergeManager * merger) {
	//printf("clean up\n");
	free(merger->heap);
	//printf("1\n");
	int i;
	for(i = 0; i < merger->heap_capacity; i++){
		free(merger->input_buffers[i]);
		//printf("i\n");
	}
	for(i = 0; i < merger->heap_capacity; i++){
		char k[100];
		char * filename = (char *) calloc(121,sizeof(char));
		sprintf(k,"%d",merger->input_file_numbers[i]);
		strcat(filename,merger->input_prefix);
		strcat(filename,k);
		strcat(filename,".dat");
		//printf("%s\n",filename );
		remove(filename);
		free(filename);
	}
	free(merger->input_buffers);
	//printf("2\n");
	free(merger->output_buffer);
	//printf("3\n");
	free(merger);
	//printf("4\n");
	
}


int q2_compare_heap_elements(Q2HeapElement *a, Q2HeapElement *b){
	if(a->COUNT > b->COUNT){
		return 1;
	}
	return 0;
}
