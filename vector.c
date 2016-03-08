/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Vector array that holds the array of data (+ length)
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "vector.h"
#include "config.h"

#define INPUT_BUFFER_SIZE 100

/*	FUNCTIONS	======================================================*/

vector* create_vector_from_file(FILE* file){
	char buffer[INPUT_BUFFER_SIZE];
	vector* vec;
	int rc;
	bool no_overflow;
	unsigned long size,index;
	double num;
	
	/* first line of the file should be size */
	fgets(buffer,INPUT_BUFFER_SIZE,file);
	rc = sscanf(buffer,"%lu",&size);
	
	/* no size is bad */
	if(rc < 1){
		return NULL;
	}
	
	/* initalize the vector */
	vec = init_vector(size);
	index = 0;
	
	/* if the file has more lines than the given size, stop reading*/
	no_overflow = true;
	
	while( fgets(buffer,INPUT_BUFFER_SIZE,file) != NULL && no_overflow){
		
		/* read a number */
		rc = sscanf(buffer,"%lf",&num);
		
		/* no number means get out of here */
		if(rc < 1){
			
			/* free the vector */
			delete_vector(vec);
			return NULL;
		}else{
			
			/* file should not have more lines than the given size */
			if(index < size){
				vec->array[index++] = num;
			}else{
				
				/* file has more lines than the given size */
				no_overflow = false;
			}
		}
	}
	return vec;
}

vector* create_vector_random(unsigned long size){
	vector* vec;
	unsigned long t;
	
	vec = init_vector(size);
	
	/* set the seed */
	srand(time(NULL));
	
	for(t=0; t < size; t++){
		
		/* gets random doubles from 0 to 10 */
		vec->array[t] = ((double)rand()/(double)RAND_MAX) *10;
	}
	
	return vec;
}

void delete_vector(vector* vec){
	if(vec){
		if(vec->array){
			free(vec->array);
		}
		free(vec);
	}
}

vector* init_vector(unsigned long size){
	vector* vec;
	
	printf(VEC_MSG,size);
	
	vec = malloc(sizeof(vector));
	vec->array = malloc(size*sizeof(double));
	vec->size = size;
	
	return vec;
}


