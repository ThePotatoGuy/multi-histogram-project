/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * These functions help parallel methods to their stuff
 */

#include <math.h>
#include "parallel_helpers.h"

/*	FUNCTIONS	======================================================*/

int calculate_end_index(int thread_id, int thread_count, unsigned long size){
	return (( (thread_id+1)*size) / thread_count) -1 ;
}

int calculate_start_index(int thread_id, int thread_count, unsigned long size){
	return (thread_id * size)  / thread_count ;
}

int calculate_thread_spawn_size(int divisor, int thread_id, int thread_count){
	int log_bse;
	
	if(thread_id >= thread_count){
		return 0;
	}
	else{
		log_bse = find_smallest_expo_of_two(value);
		if(thread_count - log_bse <= thread_id){
			log_bse = thread_count - thread_id-1;
		}
		return log_bse;
	}
}

int find_larger_power_of_two(int value){
	return (int) pow(2,find_largest_expo_of_two(value));
}

int find_largest_expo_of_two(int value){
	return (int) ceil(log_base(value, 2));
}

int find_smaller_power_of_two(int value){
	return (int) pow(2,find_smallest_expo_of_two(value));
}

int find_smallest_expo_of_two(int value){
	return (int) floor(log_base(value, 2));
}

double log_base(double value, double base){
	return log(value) / log(base);
}
