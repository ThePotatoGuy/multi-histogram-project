/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Histogram program that bins data according to min/max of a bin's bounds
 * (Refer to actual histograms to understand how histograms work)
 * 
 * This program can bin given double/float data in parallel (and serial)
 * 
 * When loading from file, please use the following format for the file:
 * <number of lines of data>
 * <data 1>
 * <data 2>
 * ...
 * <data n>
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
	bool para_mode, rand_mode, file_mode, verb_mode;
	FILE* file;
	
	/* We need at least 1 argument */
	if(argc < 2){
		printf(HELP_MESSAGE);
		return ERROR;
	}
	
	index = 1;
	
	/* first argument is help flag */
	if(strcmp(argv[index],HELP_FLAG)==0){
		printf(HELP_MESSAGE);
		return SUCCESS;
	}
	
	/* first argument is verb flag */
	if(strcmp(argv[index],VERB_FLAG)==0){
		verb_mode = true;
		index += 1;
	}else{
		verb_mode = false;
	}
	
	para_mode = false;
	rand_mode = false;
	file_mode = false;
	graph = NULL;
	
	/* parse all arguments */
	while(index < argc){
		
		/* once we parse parallel flag and (file flag or random flag),
		 * we dont need to do anymore argument parsing 
		 */
		if((para_mode && file_mode) || (para_mode && rand_mode)){
			index = argc+1;
		}
		
		/* we found parallel flag */
		else if(strcmp(argv[index],PARA_FLAG)==0 && !para_mode){
			
			/* parallel flag requires at least 1 following argument */
			if(argc-index < 1){
				printf(BAD_ARGS_MESSAGE,PARA_FLAG);
				return ERROR;
			}else{
				index += 1;
				
				/* next argument is thread count */
				rc = sscanf(argv[index],"%lu",&thread_count);
				
				/* we didnt find a number */
				if(rc < 1){
					printf(BAD_NUM_MESSAGE,PARA_FLAG);
					return ERROR;
				}else{
					index += 1;
				}
				
				/* thread_count cannot be less than 2 */
				if(thread_count < 2){
					printf(ERROR_SINGLE_THREAD);
					return ERROR;
				}
				
				para_mode = true;
			}
		}
		
		/* we found randomg flag */
		else if(strcmp(argv[index],RAND_FLAG)==0 && !rand_mode){
			
			/* random flag requires at least 2 following arugments */
			if(argc-index < 2){
				printf(BAD_ARGS_MESSAGE,RAND_FLAG);
				return ERROR;
			}else{
				index += 1;
				
				/* next argument is size */
				rc = sscanf(argv[index],"%lu",&size);
				
				/* we didnt find a number */
				if(rc < 1){
					printf(BAD_NUM_MESSAGE,RAND_FLAG);
					return ERROR;
				}else{
					index +=1;
				}
				
				/* next argument is number of bins */
				rc = sscanf(argv[index],"%lu",&bins_size);
				
				/* we didnt find a number */
				if(rc < 1){
					printf(BAD_BIN_MESSAGE,RAND_FLAG);
					return ERROR;
				}else{
					index += 1;
				}
				
				rand_mode = true;
				
				/* create a histogram and randomized data vector */
				graph = init_histogram(bins_size);
				graph->data = create_vector_random(size);
			}	
		}
		
		/* no flags means filename */
		else if(!file_mode){
			
			/* filename requires at least 1 following argument */
			if(argc-index < 1){
				printf(BAD_BIN_MESSAGE,argv[index]);
				return ERROR;
			}
			
			/* open the file */
			file = fopen(argv[index],READ_ONLY);
			
			/* we couldnt find the file */
			if(!file){
				printf(ERROR_FILENAME,argv[index]);
				return ERROR;
			}else{
				index += 1;
			}
			
			/* next argument is number of bins */
			rc = sscanf(argv[index],"%lu",&bins_size);
			
			/* we didnt find a number */
			if(rc < 1){
				printf(BAD_NIN_MESSAGE,argv[index-1]);
				return ERROR;
			}else{
				index += 1;
			}
			
			file_mode = true;
			
			/* create a histogram and read vector data from file */
			graph = init_histogram(bins_size);
			graph->data = create_vector_from_file(file);
			
			/* close the file */
			fclose(file);
		}
	}
	
	/* we need to be in random mode or file mode to run */
	if(!rand_mode && !file_mode){
		printf(BAD_ARGS);
		printf(HELP_MESSAGE);
		return ERROR;
	}
	
	/* setup the graph's bins */
	rc = process_stats(graph);
	
	/* we had problems setting the graph's bins */
	if(rc < 0){
		printf(ERROR_UNKNOWN);
		return ERROR;
	}else if(rc > 0){
		printf(ERROR_NO_DATA);
		return ERROR;
	}
	
	/* parallization mode */
	if(para_mode){
		
		/* We cant have more threads than amount of data */
		if(thread_count > graph->data->size){
			printf(ERROR_TOO_MANY_THREADS,thread_count,graph->data->size);
			return ERROR;
		}
		
		process_data_parallel(graph,thread_count,verb_mode);
	}else{
		process_data_serial(graph);
	}
	
	/* print results */
	print_bins(graph);
	
	delete_histogram(graph);
	/*delete_vector(graph->data);*/
	
	return SUCCESS;
}
