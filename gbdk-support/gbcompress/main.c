// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "gbcompress.h"
#include "files.h"

#define MAX_STR_LEN     4096

char filename_in[MAX_STR_LEN] = {'\0'};
char filename_out[MAX_STR_LEN] = {'\0'};

uint8_t * p_buf_in  = NULL;
uint8_t * p_buf_out = NULL;

bool opt_mode_compress = true;
bool opt_verbose       = false;


static void display_help(void);
static int handle_args(int argc, char * argv[]);
static int compress(void);
static int decompress(void);
void cleanup(void);


static void display_help(void) {
    fprintf(stdout,
       "gbcompress [options] infile outfile\n"
       "Use: Gbcompress a binary file and write it out.\n"
       "\n"
       "Options\n"
       "-h : Show this help screen\n"
       "-d : Decompress (default is compress)\n"
       "-v : Verbose output\n"
       "Example: \"gbcompress binaryfile.bin compressed.bin\"\n"
       "Example: \"gbcompress -d compressedfile.bin decompressed.bin\"\n"
       );
}


int handle_args(int argc, char * argv[]) {

    int i = 1; // start at first arg

    if( argc < 3 ) {
        display_help();
        return false;
    }

    // Start at first optional argument
    // Last two arguments *must* be input/output files
    for (i = 1; i < (argc - 2); i++ ) {

        if (argv[i][0] == '-') {
            if (strstr(argv[i], "-h")) {
                display_help();
                return false;  // Don't parse input when -h is used
            } else if (strstr(argv[i], "-v")) {
                opt_verbose = true;
            } else if (strstr(argv[i], "-d")) {
                opt_mode_compress = false;
            } else
                printf("BankPack: Warning: Ignoring unknown option %s\n", argv[i]);
        }
    }

    // Copy input and output filenames from last two arguments
    // if not preceded with option dash
    if (argv[i][0] != '-') {
        snprintf(filename_in, sizeof(filename_in), "%s", argv[i++]);

        if (argv[i][0] != '-') {
            snprintf(filename_out, sizeof(filename_out), "%s", argv[i++]);
            return true;
        }
    }


    return false;
}


void cleanup(void) {
    if (p_buf_in != NULL) {
        free(p_buf_in);
        p_buf_in = NULL;
    }
    if (p_buf_out != NULL) {
        free(p_buf_out);
        p_buf_out = NULL;
    }
}


static int compress() {

    uint32_t  buf_size = 0;
    uint32_t  out_len = 0;

    p_buf_in =  file_read_into_buffer(filename_in, &buf_size);

    // Allocate buffer of same size for rle output 
    p_buf_out = malloc(buf_size);

    if ((p_buf_out) && (buf_size > 0)) {
        out_len = gbcompress_buf(p_buf_in, buf_size, 
                                 p_buf_out, buf_size);
        if (out_len > 0)
            if (file_write_from_buffer(filename_out, p_buf_out, out_len)) {
                if (opt_verbose)
                    printf("Compressed: %d bytes -> %d bytes (%%%.2f)\n", buf_size, out_len, ((double)out_len / (double)buf_size) * 100);
                return EXIT_SUCCESS;
            }
    }

    return EXIT_FAILURE;    
}


static int decompress() {

    uint32_t  buf_size_in = 0;
    uint32_t  buf_size_out = 0;
    uint32_t  out_len = 0;

    p_buf_in =  file_read_into_buffer(filename_in, &buf_size_in);

    // Allocate buffer output buffer 3x size of input
    // It can grow more in gbdecompress_buf()
    buf_size_out = buf_size_in * 3;
    p_buf_out = malloc(buf_size_out);

    if ((p_buf_out) && (buf_size_in > 0)) {
        out_len = gbdecompress_buf(p_buf_in, buf_size_in,
                                 &p_buf_out, buf_size_out);
        if (out_len > 0)
            if (file_write_from_buffer(filename_out, p_buf_out, out_len)) {
                if (opt_verbose)
                    printf("Decompressed: %d bytes -> %d bytes (compression was %%%.2f)\n", buf_size_in, out_len, ((double)buf_size_in / (double)out_len) * 100);
                return EXIT_SUCCESS;
            }
    }

    return EXIT_FAILURE;
}


int main( int argc, char *argv[] )  {

    // Exit with failure by default
    int ret = EXIT_FAILURE;

    // Register cleanup with exit handler
    atexit(cleanup);

    if (handle_args(argc, argv)) {

        if (opt_mode_compress)
            ret = compress();
        else
            ret = decompress();
    }
    cleanup();

    return ret; // Exit with failure by default
}
