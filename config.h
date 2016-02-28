/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * The config file defines constants
 * Mostly string messages 
 */

#ifndef CONFIG_H
#define CONFIG_H

/* read only flag for fopen */
#define READ_ONLY "r"

/* the base number of threads is 2 */
#define BASE_THD 2

/* flags for cmd input */
#define RAND_FLAG "-R"
#define HELP_FLAG "-h"
#define PARA_FLAG "-p"
#define VERB_FLAG "-v"

/* The help message, in python-like style */
#define HELP_MESSAGE "usage: histo_program.out [-h] [-v] [-p N] (-R N B or FILENAME B)\n\nApply histogram data sorting to given (or random) data\n\nRequired arguments:\n -R N B\t\t Randomly generate data of size N and apply the histogram data sorting to the data using bin size B.\n\t\t(CANNOT be used with (FILENAME B))\n FILENAME B\t load data to sort from a file using bin size B. (CANNOT be used with (-R N B))\n\nOptional arguments:\n -h \t\t show this help message and exit\n -p N\t\t Use parallel binning process with N number of threads.\n\t\t(Default mode is serial)\n -v \t\t Be verbose (must be first flag to activate)\n\t\t(only affects parallel mode)\n"

/* Thread status messages for verbose mode */
#define THREAD_SP_MSG "Thread %lu is spawning: %d\n"
#define THREAD_CR_MSG "In thread %lu: creating thread %lu\n"

/* vector status message */
#define VEC_MSG "Creating vector of size %lu...\n"

/* mode names for status message */
#define METH_SER "Serial"
#define METH_PAR "Parallel"

/* histogram status messages */
#define H_MM_MSG "Finding min and max values...\n"
#define H_BM_MSG "Calculting upper bounds for bins...\n"
#define H_BD_MSG "Binning data in %s...\n"

/* bin printing and formatting strings */
#define BINS_MESSAGE "%10s|%10s|%10s\n"
#define BINS_MSG_BIN "Bin number"
#define BINS_MSG_COT "Count"
#define BINS_MSG_MAX "Upper bound"
#define BINS_DATA_MSG "%9lu |%9lu |%9lf\n"

/* cmd argument error messages */
#define BAD_ARGS_MESSAGE "Missing number arguments to %s\n"
#define BAD_ARG_MESSAGE "Missing number argument N to %s\n"
#define BAD_BIN_MESSAGE "Missing number argument B to %s\n"
#define BAD_NUM_MESSAGE "Number argument to %s is NaN\n"
#define BAD_NIN_MESSAGE "Bin number argument to %s is NaN\n"
#define BAD_ARGS "Missing arguments\n"

/* general error messages */
#define ERROR_FILENAME "File %s not found\n"
#define ERROR_UNKNOWN "Unknown error occured\n"
#define ERROR_NO_DATA "No data was found\n"
#define ERROR_THREAD_CR "ERROR; return code from pthread_create() is %d\n"
#define ERROR_THREAD_JN "ERROR; return code from pthread_join() is %d\n"
#define ERROR_TOO_MANY_THREADS "ERROR: Max thread:data ratio is 1:1. Given: %lu:%lu\n"
#define ERROR_SINGLE_THREAD "ERROR: parallel mode requires more than 1 thread\n"

#endif
