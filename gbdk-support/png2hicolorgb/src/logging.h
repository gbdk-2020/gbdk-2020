// logging.h

#ifndef _LOGGING_H
#define _LOGGING_H

enum output_levels {
    OUTPUT_LEVEL_DEBUG,
    OUTPUT_LEVEL_VERBOSE,
    OUTPUT_LEVEL_DEFAULT,
    OUTPUT_LEVEL_ONLY_ERRORS,
    OUTPUT_LEVEL_QUIET
};

#define log_progress log_verbose

void log_set_level(int new_output_level);
void log_debug(const char * format, ...);
void log_verbose(const char * format, ...);
void log_standard(const char * format, ...);
void log_error(const char * format, ...);

#endif