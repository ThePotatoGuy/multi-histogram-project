/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Histogram creates bin_data structs that holds 
 */
 
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

/*	TYPES	==========================================================*/

typedef struct{
	unsigned long size;
	double* bin_maxes;
	unsigned long* bin_counts;
	vector* data;
}histogram;

/*	FUNCTIONS	======================================================*/

/**
 * Deletes the given histogram
 */
void delete_histogram(histogram* gram);

/**
 * Create a histogram struct with the given size
 * does NOT initalize the data portion of the histrogram struct
 */
histogram* init_histogram(unsigned long size);

#endif
