/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * The config file defines specific usage modes and constants
 */

#ifndef CONFIG_H
#define CONFIG_H

#define READ_ONLY "-r"

#define RAND_FLAG "-R"
#define HELP_FLAG "-h"

#define HELP_MESSAGE "usage: histo_program.out [-h] [-R N B] [FILENAME B]\n\nApply histogram data sorting to given (or random) data\n\nOptional arguments:\n -h \t\t show this help message and exit\n -R N\t\t Randomly generate data of size N and apply the histogram data sorting to the data using bin size B. (CANNOT be used with [FILENAME B])\n FILENAME \t load data to sort from a file using bin size B. (CANNOT be used with [-R N B]\n"

#define BAD_ARG_MESSAGE "Missing number argument N to %s\n"
#define BAD_BIN_MESSAGE "Missing number argument B to %s\n"
#define BAD_NUM_MESSAGE "Number argument to %s is NaN\n"
#define BAD_NIN_MESSAGE "Bin number argument to %s is NaN\n"

#define ERROR_FILENAME "File %s not found\n"

#endif
