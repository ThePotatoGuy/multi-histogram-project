/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Histogram creates bin_data structs that holds 
 */
 
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <stdbool.h>
#include "vector.h"

/*	TYPES	==========================================================*/

/* Basic histogram */
typedef struct{
	unsigned long bin_count;
	/*unsigned long min;
	unsigned long max;*/
	double min;
	double max;
	double bin_width;
	double* bin_maxes;
	unsigned long* bin_counts;
	vector* data;
}histogram;

/* a modified histogram for parallel usage */
typedef struct{
	histogram* graph;
	unsigned long* loc_bin_counts;
	unsigned long thread_id;
	unsigned long thread_count;
	unsigned long divisor;
	bool is_edge;
}p_histogram;

/*	FUNCTIONS	======================================================*/

/**
 * Parallelized data binning
 */
void* bin_data(void* data);

/**
 * Bins a set number of data depending on thread_id
 * for paralliation
 */
void bin_data_values(p_histogram* p_graph);

/**
 * Deletes the given histogram
 */
void delete_histogram(histogram* gram);

/**
 * Deletes the given p_histogram
 * DOES NOT DELETE the embedded histogram
 */
void delete_p_histogram(p_histogram* p_graph);

/**
 * Deletes the given list of p_histogranms
 */
void delete_p_histogram_list(p_histogram** p_graphs, int size);

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
 * Creates a p_histogram struct with the given histogram and thread_id and thread_count
 * Assumes histogram data has already been initalized
 */
p_histogram* init_p_histogram(histogram* graph, unsigned long thread_id, unsigned long thread_count);

/**
 * Prints the bin_counts
 * Assumes the graph exists
 */
void print_bins(histogram* graph);

/**
 * counts the data in graph according to bin in parallel
 * Assumes bin_maxes and min and max stuff has already been done
 * 
 * @returns return codes from p_thread (messages will have already been printed out)
 */
int process_data_parallel(histogram* graph, unsigned long thread_count);

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

/**
 * Sums the loc_bin_cts between the given p_histograms and
 * sets the sums to p_graph_receive
 */
void sum_bin_counts(p_histogram* p_graph_receive, p_histogram* p_graph_send);

#endif
