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

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> /* for parallel output */
#include "vector.h"
#include "histogram.h"
#include "parallel_helpers.h"
#include "config.h"
#include "return_code.h"

/*	PRIVATE VARIABLE	==============================================*/

/* makes threads joinable */
static pthread_attr_t join;

/* false if not verbose, true if we are */
static bool mode;

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
 * Debug method lets us see the bin_cts of a p_graph
 */
/*static void print_bin_cts(p_histogram* p_graph);*/

/**
 * Transfers the bin counts from p_graph to the graph
 */
static void transfer_bin_counts(histogram* graph, p_histogram* p_graph);

/*	FUNCTIONS	======================================================*/

static unsigned long binary_find_bin(double data, double* bin_maxes, unsigned long start, unsigned long end){
	if(end-start > 0){
		unsigned long pivot = (start+end)/2;
		
		/* we belong to this bin */
		if(bin_maxes[pivot-1] <= data && data < bin_maxes[pivot]){
			return pivot;
		}
		
		/* we belong to smaller bin*/
		else if(data < bin_maxes[pivot-1]){
			return binary_find_bin(data, bin_maxes, start, pivot);
		}
		
		/* we belong to larger bin */
		else if(data > bin_maxes[pivot]){
			return binary_find_bin(data, bin_maxes, pivot, end);
		}
	}
	return start;
}

void* bin_data(void* data){
	p_histogram* p_graph;
	p_histogram** p_graphs;
	pthread_t* threads;
	void* status;
	unsigned long divisor,t,index;
	unsigned int spawn_size;
	int rc, i;
	
	p_graph = (p_histogram*) data;
	
	/* edge threads require a smaller divisor than p_graphs divisor */
	if(p_graph->is_edge){
		divisor = find_larger_power_of_two(p_graph->thread_count - p_graph->thread_id);
	}else{
		divisor = p_graph->divisor;
	}
	
	/* each thread spawns a calculated number of threads. ideal for tree sum */
	spawn_size = calculate_thread_spawn_size(divisor, p_graph->thread_id, p_graph->thread_count);
	
	/* Verbose mode */
	if(mode){
		printf(THREAD_SP_MSG,p_graph->thread_id, spawn_size);
	}
	
	/* if we get to spawn threads */
	if(spawn_size > 0){
		p_graphs = malloc(spawn_size*sizeof(p_histogram*));
		threads = malloc(spawn_size*sizeof(pthread_t));
		index = 0;
	
		/* spawn while we still need to spawn threads */
		while(divisor > 0 && index < spawn_size){
			
			/* calculates the next thread_id to spawn */
			t = p_graph->thread_id + (divisor/2);
			
			/* verbose mode */
			if(mode){
				printf(THREAD_CR_MSG, p_graph->thread_id, t);
			}
			
			/* create the p_histogram for the thread to spawn */
			p_graphs[index] = init_p_histogram(p_graph->graph, t, p_graph->thread_count);
			p_graphs[index]->divisor = divisor/2;
			
			/* The first thread spawned from an edge thread is also an edge thread */
			if(p_graph->is_edge && index == 0){
				p_graphs[index]->is_edge = true;
			}else{
				p_graphs[index]->is_edge = false;
			}
			
			/* create the thread */
			rc = pthread_create(&threads[index],&join,bin_data,(void*)p_graphs[index]);
			
			/* issue happened when createing thread */
			if(rc){
				printf(ERROR_THREAD_CR, rc);
				
				/* free the p_histogram and threads */
				delete_p_histogram_list(p_graphs,index+1);
				free(threads);
				exit(ERROR);
			}
			
			/* increment the index and divide the divisor */
			index += 1;
			divisor /= 2;
		}
	}
	
	/* bin the data this thread is assigned to */
	bin_data_values(p_graph);
	
	/* join the spawned threads */
	for(i = spawn_size-1; i>= 0; i--){
		
		/* join this thread */
		rc = pthread_join(threads[i], &status);
		
		/* error occured when joining the thread */
    	if(rc){
        	printf(ERROR_THREAD_JN, rc);
        	
        	/* free the p_histogram and threads */
        	delete_p_histogram_list(p_graphs,index);
        	free(threads);
        	exit(ERROR);
    	}
    	
    	/* sum the local bin counts */
    	sum_bin_counts(p_graph,p_graphs[i]);
	}
	
	/* free the p_histogram and threads if we spawned any */
	if(spawn_size > 0){
		delete_p_histogram_list(p_graphs, index);
		free(threads);
		/*free(status);*/
	}
	return NULL;
	/* exit thread */
	/*pthread_exit(NULL);*/
}

void bin_data_values(p_histogram* p_graph){
	unsigned long start_index, end_index, t, bin;
	
	/* assign data range for this thread */
	start_index = calculate_start_index(p_graph->thread_id, p_graph->thread_count, p_graph->graph->data->size);
	end_index = calculate_end_index(p_graph->thread_id, p_graph->thread_count, p_graph->graph->data->size);
	
	/*printf("Thread %lu = %lu:%lu\n",p_graph->thread_id,start_index,end_index);*/
	
	for(t=start_index; t <= end_index; t++){
		
		/* find bin for this data */
		bin = find_bin(p_graph->graph->data->array[t],p_graph->graph);
		
		/* increment the local bin counts */
		p_graph->loc_bin_counts[bin] += 1;
	}
}

static void calculate_bin_maxes(histogram* graph){
	unsigned long t;
	
	/* display status message */
	printf(H_BM_MSG);
	
	for(t=0; t < graph->bin_count; t++){
		
		/* calculate upper bounds for the bins */
		graph->bin_maxes[t] = graph->min + graph->bin_width * (t+1);
	}
}

static void calculate_bin_width(histogram* graph){
	
	/* calcuates the width for a bin */
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
		
		/* we dont delete the histogram because it is shared
		 * between threads 
		 */
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
	/*unsigned long t;*/
	
	/* if the data is less than min or greater than max, throw it out */
	if(data < graph->min || data > graph->max){
		return graph->bin_count;
	}
	
	/* Check if the value is in the first bin */
	if(graph->min <= data && data < graph->bin_maxes[0]){
		return 0;
	}
	
	/* check if value belongs in last bin (because it is a max value) */
	if(data == graph->max){
		return (graph->bin_count)-1;
	}
	
	/* Binary search for bin */
	return binary_find_bin(data, graph->bin_maxes, 1, graph->bin_count);
	
	/* Linear Search for bin (deprecated) */
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
	
	/* print status message */
	printf(H_MM_MSG);
	
	/* if the data doesnt exist */
	if(!graph->data){
		return ERROR;
	}
	
	/* size of data is 0 */
	if(graph->data->size < 1){
		return FAIL;
	}
	
	/* set min and max to first data */
	min = graph->data->array[0];
	max = graph->data->array[0];
	
	/* find the min and max of the data */
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
	
	/* set the min and max to the given graph */
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
	
	/* initalize bin datas to 0 */
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
	
	/* initialize bin data to 0 */
	for(t=0; t < graph->bin_count; t++){
		p_graph->loc_bin_counts[t] = 0;
	}
	
	return p_graph;
}

/* Debug method only 
static void print_bin_cts(p_histogram* p_graph){
	unsigned long t;
	
	printf("Thread %lu: ",p_graph->thread_id);
	
	for(t=0; t < p_graph->graph->bin_count; t++){
		printf("%lu, ",p_graph->loc_bin_counts[t]);
	}
	
	printf("\n");
}*/

void print_bins(histogram* graph){
	unsigned long t;
	
	/* print bin header */
	printf(BINS_MESSAGE,BINS_MSG_BIN,BINS_MSG_COT,BINS_MSG_MAX);
	
	for(t=0; t < graph->bin_count; t++){
		
		/* print bin number, bin count, and bin upper bound */
		printf(BINS_DATA_MSG,t,graph->bin_counts[t],graph->bin_maxes[t]);
	}
}

int process_data_parallel(histogram* graph, unsigned long thread_count, bool verb_mode){
	pthread_t* threads;
	p_histogram** p_data;
	int rc,t,second_thread_base;
	void* status;
	
	/* print status message */
	printf(H_BD_MSG,METH_PAR);
	
	threads = malloc(BASE_THD*sizeof(pthread_t));
	p_data = malloc(BASE_THD*sizeof(p_histogram*));
	mode = verb_mode;
	
	/* we use this value to create the second thread, which should
	 * always be a power of two. This helps us out in the tree sum
	 * 
	 * These two threads will spawn the rest of the threads 
	 */
	second_thread_base = find_smaller_power_of_two(thread_count-1);
	
	/* set the joinable attribute for threads */
	pthread_attr_init(&join);
   	pthread_attr_setdetachstate(&join, PTHREAD_CREATE_JOINABLE);	
	
	/* initalize the first two p_graphs for the first two threads */
	for(t=0; t < BASE_THD; t++){
		p_data[t] = init_p_histogram(graph,t,thread_count);
		p_data[t]->divisor = second_thread_base;
	}
	
	/* the second thread is the edge thread */
	p_data[1]->thread_id = second_thread_base;
	p_data[1]->is_edge = true;
	
	/* create the two threads */
	for(t=0; t < BASE_THD; t++){
		
		/* create a thread */
		rc = pthread_create(&threads[t], &join, bin_data, (void*)p_data[t]);
		
		/* problem creating thread */
		if(rc){
			printf(ERROR_THREAD_CR, rc);
			
			/* free the p_histogram and threads */
			delete_p_histogram_list(p_data,BASE_THD);
			free(threads);
			exit(ERROR);
		}
	}
	
	/* join the threads */
	for(t=0; t < BASE_THD; t++){
		
		/* join a thread */
		rc = pthread_join(threads[t], &status);
		
		/* error joining thread */
    	if(rc){
        	printf(ERROR_THREAD_JN, rc);
        	
        	/* free the p_histogram and threads */
        	delete_p_histogram_list(p_data,BASE_THD);
        	free(threads);
        	exit(ERROR);
    	}
	}
	
	/* sum these two threads' bin counts and set it to the og histogram's
	 * bin counts
	 */
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
	
	/* print status message */
	printf(H_BD_MSG,METH_SER);
	
	/* find the bins for this data */
	for(t=0; t < graph->data->size; t++){
		bin = find_bin(graph->data->array[t], graph);
		
		/*printf("%lu\n", bin);*/
		
		/* this part really wont happen ever */
		if(bin == graph->bin_count){
			printf("Bad data at index %lu\n", bin);
		}else{
			graph->bin_counts[bin] += 1;
		}
	}
}

int process_stats(histogram* graph){
	int rc;
	
	/* find the min and max of the data */
	rc = find_min_max(graph);
	
	/* data dont exist */
	if(rc){
		return rc;
	}
	
	/* calculate the bin width and upper bounds */
	calculate_bin_width(graph);
	calculate_bin_maxes(graph);
	
	return SUCCESS;
}

void sum_bin_counts(p_histogram* p_graph_receive, p_histogram* p_graph_send){
	unsigned long t;
	
	/*print_bin_cts(p_graph_receive);
	print_bin_cts(p_graph_send);*/
	
	/* sum the bin counts and set them to the receiving p_histogram */
	for(t=0; t < p_graph_receive->graph->bin_count; t++){
		p_graph_receive->loc_bin_counts[t] += p_graph_send->loc_bin_counts[t];
	}
}

static void transfer_bin_counts(histogram* graph, p_histogram* p_graph){
	unsigned long t;
	
	/* set the histogram bins to the p_histogram bins */
	for(t=0; t < graph->bin_count; t++){
		graph->bin_counts[t] = p_graph->loc_bin_counts[t];
	}
}
