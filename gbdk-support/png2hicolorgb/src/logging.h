// logging.h

#ifndef _LOGGING_H
#define _LOGGING_H

#include "common.h"

enum output_level {
    OUTPUT_LEVEL_DEBUG,
    OUTPUT_LEVEL_VERBOSE,
    OUTPUT_LEVEL_DEFAULT,
    OUTPUT_LEVEL_ONLY_ERRORS,
};

void set_log_level(enum output_level new_output_level);
int log_(enum output_level level, char const *fmt, ...) FMT(printf, 2, 3);
#define DBG(...) log_(OUTPUT_LEVEL_DEBUG, __VA_ARGS__)
#define VERBOSE(...) log_(OUTPUT_LEVEL_VERBOSE, __VA_ARGS__)
#define LOG(...) log_(OUTPUT_LEVEL_DEFAULT, __VA_ARGS__)
#define ERR(...) log_(OUTPUT_LEVEL_ONLY_ERRORS, __VA_ARGS__)

#endif
