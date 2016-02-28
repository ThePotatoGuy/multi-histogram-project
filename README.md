# Histogram Project for COMP 137

Histogram program that bins data according to min/max of a bin's bounds
(Refer to actual histograms to understand how histograms work)
 
This program can bin given double/float data in parallel (and serial)
 
When loading from file, please use the following format for the file:
* \<number of lines of data\>
* \<data 1\>
* \<data 2\>
* ...
* \<data n\>

# USAGE:
```
histo_program.out [-h] [-v] [-p N] (-R N B or FILENAME B)

Required arguments:
-R N B		Randomly generate data of size N and apply the histogram 
			data binning using bin size B.
			(CANNOT be used with (FILENAME B))
FILENAME B	Load data to sort from a file using bin size B.
			(CANNOT be used with (-R N B))

Optional arguments:
-h			Display help message and exit
-p N		Use parallel binning process with N number of threads
			(Default mode is serial)
-v			Be verbose (must be first flag to activate)
			(only affects parallel mode)
```
