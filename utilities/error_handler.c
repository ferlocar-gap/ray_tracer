/* error_handler.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Prints and handles errors that occur on the application.
 */

// Headers
#include <stdio.h>
#include <stdlib.h>

// Error messages
#define SUCCESS_MSG "Success. No Error.\n"
#define MEMORY_ALLOCATION_MSG "USER ERROR: Memory allocation.\n"
#define OPEN_FILE_MSG "USER ERROR: Opening error.\n"
#define MISSING_CONFIG_PROPERTY_MSG "USER ERROR: Missing a property in the configuration file.\n"
#define UNDEFINED_TYPE_MSG "USER ERROR: Undefined object type in the configuration file.\n"

char *ERROR_MESSAGES[] =
{
	SUCCESS_MSG,
	MEMORY_ALLOCATION_MSG,
	OPEN_FILE_MSG,
	MISSING_CONFIG_PROPERTY_MSG,
	UNDEFINED_TYPE_MSG
};

// Methods

/*
 * Prints the error message associated with the given error code
 *
 * error_code: Unique error code associated to the error message you want to print
 */

void* print_error(int error_code)
{
	printf("%s", ERROR_MESSAGES[error_code]);
}

