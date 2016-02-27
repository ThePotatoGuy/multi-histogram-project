/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 */

#include <stdlib.h>
#include "histogram.h"
#include "return_code.h"

/*	PRIVATE FUNCTION PROTOTYPES	======================================*/

/**
 * Calculatse and sets the bin_maxes of the given graph
 * Assumes bin_width is already set
 */
static void calculate_bin_maxes(histogram* graph);

/**
 * Calculates the bin_width of this graph;
 * Assumes the min and max of this graph is already set.
 */
static void calculate_bin_width(histogram* graph);

/**
 * Finds the min and maximum of the data in the given graph and sets
 * it to the corresponding values of graph
 * 
 * USES RETURN CODE
 * @returns SUCCESS if the min and max was found and set successfully
 * 	FAIL if the data vector is size 0
 * 	ERROR if the data vector was not initalized.
 */
static int find_min_max(histogram* graph);

/*	FUNCTIONS	======================================================*/

static void calculate_bin_maxes(histogram* graph){
	unsigned long t;
	
	for(t=0; t < graph->bin_count; t++){
		graph->bin_maxes[t] = graph->min + graph->bin_width * (t+1);
	}
}

static void calculate_bin_width(histogram* graph){
	graph->bin_width = (graph->max - graph->min)/graph->bin_count;
}

void delete_histogram(histogram* gram){
	if(gram){
		if(gram->bin_maxes){
			free(gram->bin_maxes);
		}
		
		if(gram->bin_counts){
			free(gram->bin_counts);
		}
		
		if(gram->data){
			delete_vector(data);
		}
		
		free(gram);
	}
}

histogram* init_histogram(unsigned long size){
	histogram* graph;
	unsigned long t;
	
	graph = malloc(sizeof(histogram));
	graph->bin_maxes = malloc(size*sizeof(double));
	graph->bin_counts = malloc(size*sizeof(unsigned long));
	graph->bin_count = size;
	graph->min = 0;
	graph->max = 0;
	graph->bin_width = 0;
	
	for(t=0; t < size; t++){
		graph->bin_maxes = 0;
		graph->bin_counts = 0;
	}
	
	return graph;
}

int process_stats(histogram* graph){
	int rc;
	
	rc = find_min_max(graph);
	
	if(rc){
		return rc;
	}
	
	calculate_bin_width(graph);
	calculate_bin_maxes(graph);
	
	return SUCCESS;
}

static int find_min_max(histogram* graph){
	double min, max;
	unsigned long t;
	
	if(!graph->data){
		return ERROR;
	}
	
	if(graph->data->size < 1){
		return FAIL;
	}
	
	min = graph->data[0];
	max = graph->data[0];
	
	for(t=1; t < graph->data->size; t++){
		if(graph->data[t] < min){
			min = graph->data[t];
		}
		if(graph->data[t] > max){
			max = graph->data[t];
		}
	}
	
	graph->min = min;
	graph->max = max;
	return SUCCESS;
}
