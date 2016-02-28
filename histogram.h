/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Histogram functions sort/bin data in a histogram.
 * Each histogram also contains the data it binned
 * 
 * p_histograms are histogram wrappers that include extra information
 * for parallelization 
 * 
 * is_edge property:
 * With odd/non-power-of-two numbers of threads, the functions used
 * to calculate number of threads to spawn for tree sum do not work
 * completely as expected. This is circumvented through the is_edge
 * property, which calculates the divisor in a different way during
 * the tree sum process.
 */
 
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <stdbool.h>
#include "vector.h"

/*	TYPES	==========================================================*/

/* Basic histogram */
typedef struct{
	unsigned long bin_count; /* number of bins */
	/*unsigned long min;
	unsigned long max;*/
	double min; /* the min value in the data */
	double max; /* the max value in the data */
	double bin_width; /* the width of a bin */
	double* bin_maxes; /* array of the upper bounds of bins */
	unsigned long* bin_counts; /* array of the number of data in each bin */
	vector* data; /* The data that is/will be binned */
}histogram;

/* a modified histogram for parallel usage */
typedef struct{
	histogram* graph; /* the histogram (should be shared by all threads) */
	unsigned long* loc_bin_counts; /* local bin counts for this thread */
	unsigned long thread_id; 
	unsigned long thread_count;
	unsigned long divisor; /* divisor, used for tree sum */
	bool is_edge; /* edge threads have special spawn properties, refer to top */
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
int process_data_parallel(histogram* graph, unsigned long thread_count, bool verb_mode);

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
