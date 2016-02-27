/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Vector array that holds the array of data (+ length)
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>

/*	TYPES	==========================================================*/

typedef struct{
	unsigned long size;
	double* array;
}vector;

/*	FUNCTIONS	======================================================*/

/**
 * Creates a vector pointer from the given file obj
 * Assumes that the given file pointer is not NULL
 * If the given input file contains more data the specified 
 * size in the data file, the extra data is ignored.
 * 
 * @returns NULL if the given file or is bad format
 */
vector* create_vector_from_file(FILE* file);

/**
 * delets the given vector
 */
void delete_vector(vector* vec);

/**
 * @returns a vector pointer to a vector with the given size
 */
vector* init_vector(unsigned long size);



#endif
