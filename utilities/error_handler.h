#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <libconfig.h>

// Error codes
#define MEMORY_ALLOCATION_ERROR 1
#define OPEN_FILE_ERROR 2
#define MISSING_CONFIG_PROPERTY_ERROR 3
#define UNDEFINED_TYPE_ERROR 4
#define MISSING_CONFIGURATION_FILE_ERROR 5
#define MISSING_CONFIGURATION_ATTR_ERROR 6

void print_error(int error_code);
void* throw_config_error(config_setting_t *setting, char *attr_path, char *attr_type);

#endif
