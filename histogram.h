/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Histogram creates bin_data structs that holds 
 */
 
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "vector.h"

/*	TYPES	==========================================================*/

typedef struct{
	unsigned long bin_count;
	double min;
	double max;
	double bin_width;
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
 * finds the bin index where the given data belongs
 * 
 * @returns the index of the bin data belongs to
 * 	OR the bin_count if the data does not belong to any bin.
 */
unsigned long find_bin(double data, histogram* graph);

/**
 * Create a histogram struct with the given size
 * does NOT initalize the data portion of the histrogram struct
 */
histogram* init_histogram(unsigned long size);

/**
 * Prints the bin_counts
 * Assumes the graph exists
 */
void print_bins(histogram* graph);

/**
 * counts the data in graph according to bin seriall.
 * Assumes bin_maxes and min and max stuff has already been done
 */
void process_data_serial(histogram* graph);

/**
 * Proceess the min, max, bin_width, and bin_maxes of the given graph
 * 
 * USES RETURN_CODE
 * @return SUCCESS if the vector of data has been processed successfully
 * 	FAIL if the data vector is size 0
 * 	ERROR if the vector of data has not been set yet
 */
int process_stats(histogram* graph);

#endif
