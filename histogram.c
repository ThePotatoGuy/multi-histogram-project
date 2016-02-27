/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 */

#include <stdlib.h>
#include "histogram.h"

/*	FUNCTIONS	======================================================*/

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
	
	graph = malloc(sizeof(histogram));
	graph->bin_maxes = malloc(size*sizeof(double));
	graph->bin_counts = malloc(size*sizeof(unsigned long));
	
	return graph;
}
