// logging.c

#include <stdarg.h>
#include <stdio.h>

#include "logging.h"

static enum output_level output_level = OUTPUT_LEVEL_DEFAULT;


#define VA_LIST_PRINT() \
    va_list args; \
    va_start (args, format); \
    vprintf (format, args); \
    va_end (args); \
    fflush(stdout);    // If logging a lot of output to a file turn off the excessive flushing


void set_log_level(enum output_level new_output_level) {
    output_level = new_output_level;
}

int log_(enum output_level level, const char *fmt, ...) {
    if (output_level > level) {
        return 0; // Pretend everything went smoothly. (It did, kind of!)
    }

    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(stderr, fmt, ap);
    va_end(ap);
    return ret;
}
