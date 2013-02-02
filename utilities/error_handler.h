#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

// Error codes
#define MEMORY_ALLOCATION_ERROR 1
#define OPEN_FILE_ERROR 2
#define MISSING_CONFIG_PROPERTY_ERROR 3
#define UNDEFINED_TYPE_ERROR 4

void* print_error(int error_code);

#endif
