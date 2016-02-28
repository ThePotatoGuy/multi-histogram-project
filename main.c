/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Histogram program
 * 
 * This program can bin given double/float data in parallel (and serial)
 * 
 * USAGE:
 * 	histo_program.out [-h] [-p N] (-R N B or FILENAME B)
 * 
 * 	Required arguments:
 * 	-R N B		Randomly generate data of size N and apply the histogram 
 * 				data binning using bin size B.
 * 				(CANNOT be used with (FILENAME B))
 * 	FILENAME B	Load data to sort from a file using bin size B.
 * 				(CANNOT be used with (-R N B))
 * 
 * 	Optional arguments:
 * 	-h			Display help message and exit
 * 	-p N		Use parallel binning process with N number of threads
 * 				(Default mode is serial)
 */

#include <stdio.h>
#include <string.h>
#include "config.h"
#include "return_code.h"
#include "histogram.h"
#include "vector.h"

int main(int argc, char* argv[]){
	unsigned long size, bins_size, thread_count;
	int rc, index;
	histogram* graph;
	bool para_mode, rand_mode, file_mode;
	FILE* file;
	
	if(argc < 2){
		printf(HELP_MESSAGE);
		return ERROR;
	}
	
	index = 1;
	
	if(strcmp(argv[index],HELP_FLAG)==0){
		printf(HELP_MESSAGE);
		return SUCCESS;
	}
	
	para_mode = false;
	rand_mode = false;
	file_mode = false;
	graph = NULL;
	
	while(index < argc){
		if((para_mode && file_mode) || (para_mode && rand_mode)){
			index = argc+1;
		}
		
		else if(strcmp(argv[index],PARA_FLAG)==0 && !para_mode){
			if(argc-index < 1){
				printf(BAD_ARGS_MESSAGE,PARA_FLAG);
				return ERROR;
			}else{
				index += 1;
				
				rc = sscanf(argv[index],"%lu",&thread_count);
				
				if(rc < 1){
					printf(BAD_NUM_MESSAGE,PARA_FLAG);
					return ERROR;
				}else{
					index += 1;
				}
				
				para_mode = true;
			}
		}
		
		else if(strcmp(argv[index],RAND_FLAG)==0 && !rand_mode){
			if(argc-index < 2){
				printf(BAD_ARGS_MESSAGE,RAND_FLAG);
				return ERROR;
			}else{
				index += 1;
				
				rc = sscanf(argv[index],"%lu",&size);
				
				if(rc < 1){
					printf(BAD_NUM_MESSAGE,RAND_FLAG);
					return ERROR;
				}else{
					index +=1;
				}
				
				rc = sscanf(argv[index],"%lu",&bins_size);
				
				if(rc < 1){
					printf(BAD_BIN_MESSAGE,RAND_FLAG);
					return ERROR;
				}else{
					index += 1;
				}
				
				rand_mode = true;
				graph = init_histogram(bins_size);
				graph->data = create_vector_random(size);
			}	
		}
		
		else if(!file_mode){
			if(argc-index < 1){
				printf(BAD_BIN_MESSAGE,argv[index]);
				return ERROR;
			}
			
			file = fopen(argv[index],READ_ONLY);
			
			if(!file){
				printf(ERROR_FILENAME,argv[index]);
				return ERROR;
			}else{
				index += 1;
			}
			
			rc = sscanf(argv[index],"%lu",&bins_size);
			
			if(rc < 1){
				printf(BAD_NIN_MESSAGE,argv[index-1]);
				return ERROR;
			}else{
				index += 1;
			}
			
			file_mode = true;
			graph = init_histogram(bins_size);
			graph->data = create_vector_from_file(file);
			fclose(file);
		}
	}
	
	if(!rand_mode && !file_mode){
		printf(BAD_ARGS);
		printf(HELP_MESSAGE);
		return ERROR;
	}
	
	rc = process_stats(graph);
	
	if(rc < 0){
		printf(ERROR_UNKNOWN);
		return ERROR;
	}else if(rc > 0){
		printf(ERROR_NO_DATA);
		return ERROR;
	}
	
	if(para_mode){
		if(thread_count > graph->data->size){
			printf(ERROR_TOO_MANY_THREADS,thread_count,graph->data->size);
			return ERROR;
		}
		process_data_parallel(graph,thread_count);
	}else{
		process_data_serial(graph);
	}
	
	print_bins(graph);
	
	//delete_histogram(graph);
	
	return SUCCESS;
}
