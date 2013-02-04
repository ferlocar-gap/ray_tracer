/* memory_handler.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Handles dynamic memory allocations.
 */

// Headers
#include <stdio.h>
#include <stdlib.h>
#include "error_handler.h"

// Methods

/*
 * Assigns the requested number of bytes from the heap memory. If there is a
 * memory error, it prints the corresponding error message and exits the
 * program. You can send a pointer to a routine in order to handle the error.
 *
 * n: Number of bytes requested
 * error_routine: Pointer to the routine that will handle the error (if any).
 */
void* get_memory(unsigned long n, void (*error_routine)())
{
	void* mem_pointer = malloc(n);
	if(!mem_pointer)
	{
		print_error(MEMORY_ALLOCATION_ERROR);
		if(error_routine)
			error_routine();
		else
			exit(MEMORY_ALLOCATION_ERROR);
	}
	return mem_pointer;
}
