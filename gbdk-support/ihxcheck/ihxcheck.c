// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "ihx_file.h"
#include "areas.h"

#define MAX_STR_LEN     4096

void display_help(void);
int handle_args(int argc, char * argv[]);

char filename_in[MAX_STR_LEN] = {'\0'};


void display_help(void) {
    fprintf(stdout,
           "ihx_check input_file.ihx [options]\n"
           "\n"
           "Options\n"
           "-h : Show this help\n"
           "-e : Treat warnings as errors\n"
           "\n"
           "Use: Read a .ihx and warn about overlapped areas.\n"
           "Example: \"ihx_check build/MyProject.ihx\"\n"
           );
}


int handle_args(int argc, char * argv[]) {

    int i;

    if( argc < 2 ) {
        display_help();
        return false;
    }

    // Copy input filename (if not preceded with option dash)
    if (argv[1][0] != '-')
        snprintf(filename_in, sizeof(filename_in), "%s", argv[1]);

    // Start at first optional argument, argc is zero based
    for (i = 1; i <= (argc -1); i++ ) {

        if (strstr(argv[i], "-h") == argv[i]) {
            display_help();
            return false;  // Don't parse input when -h is used

        } else if (strstr(argv[i], "-e") == argv[i]) {
            set_option_warnings_as_errors(true);
        }

    }

    return true;
}


int matches_extension(char * filename, char * extension) {
    return (strcmp(filename + (strlen(filename) - strlen(extension)), extension) == 0);
}


int main( int argc, char *argv[] )  {

    int ret = EXIT_FAILURE; // Exit with failure by default

    if (handle_args(argc, argv)) {

        // Must at least have extension
        if (strlen(filename_in) >=5) {
            // detect file extension
            if (matches_extension(filename_in, (char *)".ihx")) {
                ret = ihx_file_process_areas(filename_in);
            }
        }
    }

    return ret; // Exit with failure by default
}