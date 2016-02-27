/**
 * @author Andre Allan Ponce
 * andreponce@null.net
 * 
 * This header contains enums for return codes. 
 * All functions that return codes use one of the given sets of codes
 */

#ifndef RETURN_CODE_H
#define RETURN_CODE_H

/*
 * The basic return code with only 3 levels
 * 
 * The true meaning of each code is described in a method's comments.
 */
typedef enum{
	SUCCESS = 0,
	FAIL = 1,
	ERROR = -1
}RETURN_CODE;

#endif
