/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Histogram program
 */

#include <stdio.h>
#include <string.h>
#include "config.h"
#include "return_code.h"
#include "histogram.h"
#include "vector.h"

int main(int argc, char* argv[]){
	unsigned long size, bins_size;
	int rc;
	histogram* graph;
	FILE* file;
	
	if(argc < 2){
		printf(HELP_MESSAGE);
		return ERROR;
	}
	
	if(strcmp(argv[1],HELP_FLAG)==0){
		printf(HELP_MESSAGE);
		return SUCCESS;
	}else if(strcmp(argv[1],RAND_FLAG)==0){
		if(argc < 4){
			printf(BAD_ARG_MESSAGE,RAND_FLAG);
			printf(HELP_MESSAGE);
			return ERROR;
		}else{
			rc = sscanf(argv[2],"%lu",&size);
			
			if(rc < 1){
				printf(BAD_NUM_MESSAGE,RAND_FLAG);
				printf(HELP_MESSAGE);
				return ERROR;
			}
			
			rc = sscanf(argv[3],"%lu",&bins_size);
			
			if(rc < 1){
				printf(BAD_BIN_MESSAGE,RAND_FLAG);
				printf(HELP_MESSAGE);
				return ERROR;
			}
			
			/* TODO */
			printf("TODO\n");
			return SUCCESS;
		}
	}else{
		if(argc < 3){
			printf(BAD_BIN_MESSAGE,argv[1]);
			printf(HELP_MESSAGE);
			return ERROR;
		}
		file = fopen(argv[1],READ_ONLY);
		
		if(!file){
			printf(ERROR_FILENAME,argv[1]);
			printf(HELP_MESSAGE);
			return ERROR;
		}
		
		rc = sscanf(argv[2],"%lu",&bins_size);
		
		if(rc < 1){
			printf(BAD_NIN_MESSAGE,argv[1]);
			printf(HELP_MESSAGE);
			return ERROR;
		}
		
		graph = init_histogram(bins_size);
		graph->data = create_vector_from_file(file);
	}
	
	return SUCCESS;
}
