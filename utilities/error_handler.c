/* error_handler.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Prints and handles errors that occur on the application.
 */

// Headers
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "error_handler.h"

// Error messages
#define SUCCESS_MSG "Success. No Error.\n"
#define MEMORY_ALLOCATION_MSG "USER ERROR: Memory allocation.\n"
#define OPEN_FILE_MSG "USER ERROR: Opening error.\n"
#define MISSING_CONFIG_PROPERTY_MSG "USER ERROR: Missing a property in the configuration file.\n"
#define UNDEFINED_TYPE_MSG "USER ERROR: Undefined object type in the configuration file.\n"
#define MISSSING_CONFIGURATION_FILE_MSG "USER ERROR: You must provide a configuration file named \"scene.cfg\" at the root of the project.\n"
#define MISSSING_CONFIGURATION_ATTR_MSG "USER ERROR: Missing a configuration attribute in \"scene.cfg\".\n"
#define MISSING_VERTEX_MSG "USER ERROR: All polygons must have at least 3 vertex.\n"

char *ERROR_MESSAGES[] =
{
	SUCCESS_MSG,
	MEMORY_ALLOCATION_MSG,
	OPEN_FILE_MSG,
	MISSING_CONFIG_PROPERTY_MSG,
	UNDEFINED_TYPE_MSG,
	MISSSING_CONFIGURATION_FILE_MSG,
	MISSSING_CONFIGURATION_ATTR_MSG,
	MISSING_VERTEX_MSG
};

// Methods

/*
 * Prints the error message associated with the given error code
 *
 * error_code: Unique error code associated to the error message you want to print
 */
void print_error(int error_code)
{
	printf("%s", ERROR_MESSAGES[error_code]);
}

/*
 * Exits the program with a 'missing configuration attribute' error coode, and
 * prints an error indicating the name and the type of the missing attribute,
 * and the line of the configuration file where the error occured.
 *
 * setting: Setting in which the attribute was not found.
 * attr_path: Path to the attribute that was not found.
 * attr_type: Type of the attribute that was not found.
 */
void* throw_config_error(config_setting_t *setting, char *attr_path, char *attr_type)
{
    print_error(MISSING_CONFIGURATION_ATTR_ERROR);
    printf("Missing attribute '%s' of type '%s', line %d", attr_path, attr_type, setting->line);
    exit(MISSING_CONFIGURATION_ATTR_ERROR);
}
