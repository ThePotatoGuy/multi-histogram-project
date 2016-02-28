/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * These functions help parallel methods do their stuff
 */

#ifndef P_HELP_H
#define P_HELP_H

/*	FUNCTIONS	======================================================*/

/**
 *	@returns the index the given thread will stop at
 */
int calculate_end_index(int thread_id, int thread_count, unsigned long size);

/**
 * @returns the index the given thread will start at
 */
int calculate_start_index(int thread_id, int thread_count, unsigned long size);

/**
 * Calculates how many threads the given thread needs to spawn for 
 * computing a tree sum
 * 
 * @returns the number of threads the given thread needs to spawn
 */
int calculate_thread_spawn_size(int divisor, int thread_id, int thread_count);

/**
 * @returns the nearest power of two to the given value
 * that is greater than the given value
 */
int find_larger_power_of_two(int value);

/**
 * @returns the log_2 of the value, rounded up to the nearest integer
 */
int find_largest_expo_of_two(int value);

/**
 * @returns the nearest power of two to the given value that is
 * smaller than the given value
 */
int find_smaller_power_of_two(int value);

/**
 * @returns the log_2 of the value, rounded down to the nearest integer
 */
int find_smallest_expo_of_two(int value);

/**
 * @returns the log base of the given value and base
 */
double log_base(double value, double base);

#endif
