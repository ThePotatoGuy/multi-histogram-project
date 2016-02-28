/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * Contains both a serial bin counter and parallel bin counter
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> /* for parallel output */
#include "vector.h"
#include "histogram.h"
#include "config.h"
#include "return_code.h"

/*	PRIVATE VARIABLE	==============================================*/

static pthread_attr_t join;

/*	PRIVATE FUNCTION PROTOTYPES	======================================*/

/**
 * binary searches for the correct bin for the given data
 * the data MUST belong to a bin.
 * 
 * @returns the bin data belongs to
 */
static unsigned long binary_find_bin(double data, double* bin_maxes, unsigned long start, unsigned long end);

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

static unsigned long binary_find_bin(double data, double* bin_maxes, unsigned long start, unsigned long end){
	
	if(end-start > 0){
		unsigned long pivot = (start+end)/2;
		if(bin_maxes[pivot-1] <= data && data < bin_maxes[pivot]){
			return pivot;
		}else if(data < bin_maxes[pivot-1]){
			return binary_find_bin(data, bin_maxes, start, pivot);
		}else if(data > bin_maxes[pivot]){
			return binary_find_bin(data, bin_maxes, pivot, end);
		}
	}
	printf("test\n");
	return start;
}

static void calculate_bin_maxes(histogram* graph){
	unsigned long t;
	
	printf(H_BM_MSG);
	
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
			delete_vector(gram->data);
		}
		
		free(gram);
	}
}

void delete_p_histogram(p_histogram* p_graph){
	if(p_graph){
		if(p_graph->graph){
			delete_histogram(p_graph->graph);
		}
		
		if(p_graph->loc_bin_counts){
			free(p_graph->loc_bin_counts);
		}
		
		free(p_graph);
	}
}

unsigned long find_bin(double data, histogram* graph){
	unsigned long t;
	
	if(data < graph->min || data > graph->max){
		return graph->bin_count;
	}
	
	/* Check if the value is in the first bin */
	if(graph->min <= data && data < graph->bin_maxes[0]){
		return 0;
	}
	
	if(data == graph->max){
		return (graph->bin_count)-1;
	}
	
	return binary_find_bin(data, graph->bin_maxes, 1, graph->bin_count);
	
	/* Linear Search */
	/*
	for(t=0; t < graph->bin_count; t++){
		if(data < graph->bin_maxes[t]){
			return t;
		}
	}
	
	return graph->bin_count;*/
}

static int find_min_max(histogram* graph){
	double min, max;
	unsigned long t;
	
	printf(H_MM_MSG);
	
	if(!graph->data){
		return ERROR;
	}
	
	if(graph->data->size < 1){
		return FAIL;
	}
	
	min = graph->data->array[0];
	max = graph->data->array[0];
	
	for(t=1; t < graph->data->size; t++){
		if(graph->data->array[t] < min){
			min = graph->data->array[t];
		}
		if(graph->data->array[t] > max){
			max = graph->data->array[t];
		}
	}
	
	/*graph->min = (unsigned long)min;
	graph->max = (unsigned long)(max+1);*/
	graph->min = min;
	graph->max = max;
	
	/*printf("%lf %lf\n",graph->min, graph->max);*/
	return SUCCESS;
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
		graph->bin_maxes[t] = 0;
		graph->bin_counts[t] = 0;
	}
	
	return graph;
}

p_histogram* init_p_histogram(histogram* graph, unsigned long thread_id, unsigned long thread_count){
	p_histogram* p_graph;
	unsigned long t;
	
	p_graph = malloc(sizeof(p_histogram));
	p_graph->loc_bin_counts = malloc(graph->bin_count*sizeof(unsigned long));
	p_graph->graph = graph;
	p_graph->thread_id = thread_id;
	p_graph->thread_count = thread_count;
	p_graph->divisor = 0;
	p_graph->is_edge = false;
	
	for(t=0; t < graph->bin_count; t++){
		p_graph->loc_bin_counts[t] = 0;
	}
	
	return p_graph;
}

void print_bins(histogram* graph){
	unsigned long t;
	
	printf(BINS_MESSAGE,BINS_MSG_BIN,BINS_MSG_COT,BINS_MSG_MAX);
	
	for(t=0; t < graph->bin_count; t++){
		printf("%9lu | %9lu | %9lf\n",t,graph->bin_counts[t],graph->bin_maxes[t]);
	}
}

int process_data_parallel(histogram* graph, unsigned long thread_count){
	pthread_t* threads;
	p_histogram** p_data;
	int rc;
	int t;
	
	threads = malloc(BASE_THD*sizeof(pthread_t));
	p_data = malloc(BASE_THD*sizeof(p_histogram*));
	
	pthread_attr_init(&join);
   	pthread_attr_setdetachstate(&join, PTHREAD_CREATE_JOINABLE);
	
	
	for(t=0; t < BASE_THD; t++){
		p_data[t] = init_p_histogram(graph, 
	}
	
	
}

void process_data_serial(histogram* graph){
	unsigned long t, bin;
	
	printf(H_BD_MSG);
	
	for(t=0; t < graph->data->size; t++){
		bin = find_bin(graph->data->array[t], graph);
		
		/*printf("%lu\n", bin);*/
		
		if(bin == graph->bin_count){
			printf("Bad data at index %u\n", bin);
		}else{
			graph->bin_counts[bin] += 1;
		}
	}
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
