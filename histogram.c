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
#include "parallel_helpers.h"
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

/**
 * Transfers the bin counts from p_graph to the graph
 */
static void transfer_bin_counts(histogram* graph, p_histogram* p_graph);

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

void* bin_data(void* data){
	p_histogram* p_graph;
	p_histogram** p_graphs;
	pthread_t* threads;
	void* status;
	unsigned long divisor,t,index;
	int rc, spawn_size, i;
	
	p_graph = (p_histogram*) data;
	
	if(p_graph->is_edge){
		divisor = find_larger_power_of_two(p_graph->thread_count - p_graph->thread_id);
	}else{
		divisor = p_graph->divisor;
	}
	
	spawn_size = calculate_thread_spawn_size(divisor, p_graph->thread_id, p_graph->thread_count);
	
	printf(THREAD_SP_MSG,p_graph->thread_id, spawn_size);
	
	if(spawn_size > 0){
		p_graphs = malloc(spawn_size*sizeof(p_histogram*));
		threads = malloc(spawn_size*sizeof(pthread_t));
		index = 0;
	
		while(divisor > 0 && index < spawn_size){
			t = p_graph->thread_id + (divisor/2);
			
			printf(THREAD_CR_MSG, p_graph->thread_id, t);
			
			p_graphs[index] = init_p_histogram(p_graph->graph, t, p_graph->thread_count);
			p_graphs[index]->divisor = divisor/2;
			
			if(p_graph->is_edge && index == 0){
				p_graphs[index]->is_edge = true;
			}else{
				p_graphs[index]->is_edge = false;
			}
			
			rc = pthread_create(&threads[index],&join,bin_data,(void*)p_graphs[index]);
			
			if(rc){
				printf(ERROR_THREAD_CR, rc);
				delete_p_histogram_list(p_graphs,index+1);
				free(threads);
				exit(ERROR);
			}
			
			/*printf("TEST %ld, %ld, %ld\n", index, divisor, t);*/
			
			index += 1;
			divisor /= 2;
		}
	}
	
	bin_data_values(p_graph);
	
	for(i = spawn_size-1; i>= 0; i--){
		rc = pthread_join(threads[i], &status);
    	if(rc){
        	printf(ERROR_THREAD_JN, rc);
        	delete_p_histogram_list(p_graphs,index);
        	free(threads);
        	exit(ERROR);
    	}
    	sum_bin_counts(p_graph,p_graphs[i]);
	}
	
	if(spawn_size > 0){
		delete_p_histogram_list(p_graphs, index);
		free(threads);
	}
	pthread_exit(NULL);
}

void bin_data_values(p_histogram* p_graph){
	unsigned long start_index, end_index, t, bin;
	
	start_index = calculate_start_index(p_graph->thread_id, p_graph->thread_count, p_graph->graph->data->size);
	end_index = calculate_end_index(p_graph->thread_id, p_graph->thread_count, p_graph->graph->data->size);
	
	for(t=start_index; t < end_index; t++){
		bin = find_bin(p_graph->graph->data->array[t],p_graph->graph);
		
		p_graph->loc_bin_counts[bin] += 1;
	}
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
		/*
		if(p_graph->graph){
			delete_histogram(p_graph->graph);
		}*/
		
		if(p_graph->loc_bin_counts){
			free(p_graph->loc_bin_counts);
		}
		
		free(p_graph);
	}
}

void delete_p_histogram_list(p_histogram** p_graphs, int size){
	int t;
	
	if(p_graphs){
		for(t=0; t < size; t++){
			if(p_graphs[t]){
				delete_p_histogram(p_graphs[t]);
			}
		}
		free(p_graphs);
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
	int rc,t,second_thread_base;
	void* status;
	
	threads = malloc(BASE_THD*sizeof(pthread_t));
	p_data = malloc(BASE_THD*sizeof(p_histogram*));
	second_thread_base = find_smaller_power_of_two(thread_count-1);
	
	pthread_attr_init(&join);
   	pthread_attr_setdetachstate(&join, PTHREAD_CREATE_JOINABLE);	
	
	for(t=0; t < BASE_THD; t++){
		p_data[t] = init_p_histogram(graph,t,thread_count);
		p_data[t]->divisor = second_thread_base;
	}
	
	p_data[1]->thread_id = second_thread_base;
	p_data[1]->is_edge = true;
	
	for(t=0; t < BASE_THD; t++){
		rc = pthread_create(&threads[t], &join, bin_data, (void*)p_data[t]);
		if(rc){
			printf(ERROR_THREAD_CR, rc);
			delete_p_histogram_list(p_data,BASE_THD);
			free(threads);
			exit(ERROR);
		}
	}
	
	for(t=0; t < BASE_THD; t++){
		rc = pthread_join(threads[t], &status);
    	if(rc){
        	printf(ERROR_THREAD_JN, rc);
        	delete_p_histogram_list(p_data,BASE_THD);
        	free(threads);
        	exit(ERROR);
    	}
	}
	
	sum_bin_counts(p_data[0],p_data[1]);
	transfer_bin_counts(graph,p_data[0]);
	
	/* Delete what we dont need anymore */
	pthread_attr_destroy(&join);
	delete_p_histogram_list(p_data,BASE_THD);
	free(threads);
	
	return SUCCESS;
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

void sum_bin_counts(p_histogram* p_graph_receive, p_histogram* p_graph_send){
	unsigned long t;
	
	for(t=0; t < p_graph_receive->graph->bin_count; t++){
		p_graph_receive->loc_bin_counts[t] += p_graph_send->loc_bin_counts[t];
	}
}

static void transfer_bin_counts(histogram* graph, p_histogram* p_graph){
	unsigned long t;
	
	for(t=0; t < graph->bin_count; t++){
		graph->bin_counts[t] = p_graph->loc_bin_counts[t];
	}
}
