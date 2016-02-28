/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * The config file defines specific usage modes and constants
 */

#ifndef CONFIG_H
#define CONFIG_H

#define READ_ONLY "r"

#define BASE_THD 2

#define RAND_FLAG "-R"
#define HELP_FLAG "-h"

#define HELP_MESSAGE "usage: histo_program.out [-h] [-R N B] [FILENAME B]\n\nApply histogram data sorting to given (or random) data\n\nOptional arguments:\n -h \t\t show this help message and exit\n -R N B\t\t Randomly generate data of size N and apply the histogram data sorting to the data using bin size B.\n\t\t(CANNOT be used with [FILENAME B])\n FILENAME B\t load data to sort from a file using bin size B. (CANNOT be used with [-R N B]\n"

#define THREAD_SP_MSG "Thread %lu is spawning: %d\n"
#define THREAD_CR_MSG "In thread %lu: creating thread %lu\n"

#define VEC_MSG "Creating vector of size %lu...\n"

#define H_MM_MSG "Finding min and max values...\n"
#define H_BM_MSG "Calculting upper bounds for bins...\n"
#define H_BD_MSG "Binning data...\n"

#define BINS_MESSAGE "%10s|%10s|%10s\n"
#define BINS_MSG_BIN "Bin number"
#define BINS_MSG_COT "Count"
#define BINS_MSG_MAX "Upper bound"

#define BAD_ARGS_MESSAGE "Missing number arguments to %s\n"
#define BAD_ARG_MESSAGE "Missing number argument N to %s\n"
#define BAD_BIN_MESSAGE "Missing number argument B to %s\n"
#define BAD_NUM_MESSAGE "Number argument to %s is NaN\n"
#define BAD_NIN_MESSAGE "Bin number argument to %s is NaN\n"

#define ERROR_FILENAME "File %s not found\n"
#define ERROR_UNKNOWN "Unknown error occured\n"
#define ERROR_NO_DATA "No data was found\n"
#define ERROR_THREAD_CR "ERROR; return code from pthread_create() is %d\n"
#define ERROR_THREAD_JN "ERROR; return code from pthread_join() is %d\n"

#endif
