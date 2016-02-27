/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Vector array that holds the array of data (+ length)
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "vector.h"

#define INPUT_BUFFER_SIZE 10

/*	FUNCTIONS	======================================================*/

vector* create_vector_from_file(FILE* file){
	char buffer[INPUT_BUFFER_SIZE];
	vector* vec;
	int rc;
	bool no_overflow;
	unsigned long size,index;
	double num;
	
	fgets(buffer,INPUT_BUFFER_SIZE,file);
	rc = sscanf(buffer,"%u",&size);
	
	if(rc < 1){
		return NULL;
	}
	
	vec = init_vector(size);
	index = 0;
	no_overflow = true;
	
	while( fgets(buffer,INPUT_BUFFER_SIZE,file) != NULL && no_overflow){
		rc = sscanf(buffer,"%f",&num);
		
		if(rc < 1){
			delete_vector(vec);
			return NULL;
		}else{
			if(index < size){
				vec.array[index++] = num;
			}else{
				no_overflow = false;
			}
		}
	}
	return vec;
}

vector* init_vector(unsigned long size){
	vector* vec;
	
	vec = malloc(sizeof(vec));
	vec.array = malloc(size*sizeof(double));
	vec.size = size;
	
	return vec;
}

void delete_vector(vector* vec){
	if(vec){
		if(vec.array){
			free(vec.array);
		}
		free(vec.array);
	}
}

