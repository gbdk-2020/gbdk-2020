// logging.c

#include <stdarg.h>
#include <stdio.h>

#include "logging.h"

int output_level = OUTPUT_LEVEL_DEFAULT;


#define VA_LIST_PRINT() \
    va_list args; \
    va_start (args, format); \
    vprintf (format, args); \
    va_end (args); \
    fflush(stdout);    // If logging a lot of output to a file turn off the excessive flushing


void log_set_level(int new_output_level) {
    output_level = new_output_level;
}

void log_debug(const char * format, ...){

    if (output_level > OUTPUT_LEVEL_DEBUG) return;
    VA_LIST_PRINT();
}

void log_verbose(const char * format, ...){

    if (output_level > OUTPUT_LEVEL_VERBOSE) return;
    VA_LIST_PRINT();
}

void log_standard(const char * format, ...){

    // Only print if quiet mode and error_only are NOT enabled
    if ((output_level == OUTPUT_LEVEL_QUIET) ||
        (output_level == OUTPUT_LEVEL_ONLY_ERRORS)) return;
    VA_LIST_PRINT();
}

void log_error(const char * format, ...){

    // Only print if quiet mode is NOT enabled
    if (output_level == OUTPUT_LEVEL_QUIET) return;
    VA_LIST_PRINT();
}