// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "gbcompress.h"
#include "rlecompress.h"
#include "files.h"
#include "files_c_source.h"

#include "zx0/zx0_libsalvador.h"

#define MAX_STR_LEN     4096

#define COMPRESSION_TYPE_GB        0
#define COMPRESSION_TYPE_RLE_BLOCK 1
#define COMPRESSION_TYPE_ZX0       2
#define COMPRESSION_TYPE_DEFAULT   COMPRESSION_TYPE_GB

char filename_in[MAX_STR_LEN] = {'\0'};
char filename_out[MAX_STR_LEN] = {'\0'};

uint8_t * p_buf_in  = NULL;
uint8_t * p_buf_out = NULL;

#define VAR_IS_CONST_YES true
#define VAR_IS_CONST_NO  false

#define OPT_HELP        "-h"
#define OPT_DECOMPRESS  "-d"
#define OPT_VERBOSE     "-v"
#define OPT_CIN         "--cin"
#define OPT_COUT        "--cout"
#define OPT_VARNAME     "--varname="
#define OPT_BANKREFNAME "--bankrefname="
#define OPT_ALG         "--alg="
#define OPT_BANK        "--bank="


bool opt_mode_compress    = true;
bool opt_verbose          = false;
int  opt_compression_type = COMPRESSION_TYPE_DEFAULT;
bool opt_c_source_input   = false;
bool opt_c_source_output  = false;
char opt_c_source_output_var_name[MAX_STR_LEN] = "var_name";
char opt_c_source_output_bankref_name[MAX_STR_LEN] = {'\0'};
uint16_t opt_bank_num     = BANK_NUM_ROM_UNSET;

static void display_help(void);
static int handle_args(int argc, char * argv[]);
static int compress(void);
static int decompress(void);
void cleanup(void);


static void display_help(void) {
    fprintf(stdout,
       "gbcompress [options] infile outfile\n"
       "Use: compress a binary file and write it out.\n"
       "\n"
       "Options\n"
       "-h       : Show this help screen\n"
       "-d       : Decompress (default is compress)\n"
       "-v       : Verbose output\n"
       "--cin    : Read input as .c source format (8 bit char ONLY, uses first array found)\n"
       "--cout   : Write output in .c / .h source format (8 bit char ONLY) \n"
       "--varname=<NAME>     : specify variable name for c source output\n"
       "--bankrefname=<NAME> : specify bankref name for c source output\n"
       "--alg=<type>     : specify compression type: 'zx0', 'rle', 'gb' (default)\n"
       "--bank=<num>     : Add Bank Ref: %d - %d (default is none, with --cout only)\n"
       "Example: \"gbcompress binaryfile.bin compressed.bin\"\n"
       "Example: \"gbcompress -d compressedfile.bin decompressed.bin\"\n"
       "Example: \"gbcompress --alg=rle binaryfile.bin compressed.bin\"\n"
       "\n"
       "The default compression (gb) is the type used by gbtd/gbmb\n"
       "The rle compression is Amiga IFF style\n",
       BANK_NUM_ROM_MIN, BANK_NUM_ROM_MAX
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
            if (strstr(argv[i], OPT_HELP) == argv[i]) {
                display_help();
                return false;  // Don't parse input when -h is used
            } else if (strstr(argv[i], OPT_VERBOSE) == argv[i]) {
                opt_verbose = true;
            } else if (strstr(argv[i], OPT_CIN) == argv[i]) {
                opt_c_source_input = true;
            } else if (strstr(argv[i], OPT_COUT) == argv[i]) {
                opt_c_source_output = true;
            } else if (strstr(argv[i], OPT_VARNAME) == argv[i]) {
                snprintf(opt_c_source_output_var_name, sizeof(opt_c_source_output_var_name), "%s", argv[i] + strlen(OPT_VARNAME));
            } else if (strstr(argv[i], OPT_BANKREFNAME) == argv[i]) {
                snprintf(opt_c_source_output_bankref_name, sizeof(opt_c_source_output_bankref_name), "%s", argv[i] + strlen(OPT_BANKREFNAME));
            } else if (strstr(argv[i], OPT_ALG"gb") == argv[i]) {
                opt_compression_type = COMPRESSION_TYPE_GB;
            } else if (strstr(argv[i], OPT_ALG"rle") == argv[i]) {
                opt_compression_type = COMPRESSION_TYPE_RLE_BLOCK;
            } else if (strstr(argv[i], OPT_ALG"zx0") == argv[i]) {
                opt_compression_type = COMPRESSION_TYPE_ZX0;
            } else if (strstr(argv[i], OPT_DECOMPRESS) == argv[i]) {
                opt_mode_compress = false;
            } else if (strstr(argv[i], OPT_BANK) == argv[i]) {
                opt_bank_num = atoi(argv[i] + strlen(OPT_BANK));
                if ((opt_bank_num < BANK_NUM_ROM_MIN) || (opt_bank_num > BANK_NUM_ROM_MAX)) {
                    printf("gbcompress: Warning: Bank Num %d outside of range %d - %d\n", opt_bank_num, BANK_NUM_ROM_MIN, BANK_NUM_ROM_MAX);
                    return false;
                }
            } else
                printf("gbcompress: Warning: Ignoring unknown option %s\n", argv[i]);
        }
    }

    // If custom bankref name not specified, copy from varname
    if (opt_c_source_output_bankref_name[0] == '\0') {
        snprintf(opt_c_source_output_bankref_name, sizeof(opt_c_source_output_bankref_name), "%s", opt_c_source_output_var_name);
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

    uint32_t  buf_size_in = 0;
    uint32_t  buf_size_out = 0;
    uint32_t  out_len = 0;
    bool      result = false;

    if (opt_c_source_input)
        p_buf_in =  file_read_c_input_into_buffer(filename_in, &buf_size_in);
    else
        p_buf_in =  file_read_into_buffer(filename_in, &buf_size_in);

    // Allocate buffer output buffer same size as input
    // It can grow more in gbdecompress_buf()
    if (opt_compression_type == COMPRESSION_TYPE_ZX0)
        buf_size_out = salvador_get_max_compressed_size(buf_size_in);
    else
        buf_size_out = buf_size_in;
    p_buf_out = malloc(buf_size_out);

    if (!p_buf_out) return EXIT_FAILURE;


    if ((p_buf_in) && (buf_size_in > 0)) {

        if (opt_compression_type == COMPRESSION_TYPE_GB)
            out_len = gbcompress_buf(p_buf_in, buf_size_in, &p_buf_out, buf_size_out);
        else if (opt_compression_type == COMPRESSION_TYPE_RLE_BLOCK)
            out_len = rlecompress_buf(p_buf_in, buf_size_in, &p_buf_out, buf_size_out);
        else if (opt_compression_type == COMPRESSION_TYPE_ZX0) {
           // FLG_IS_INVERTED Flag, no offset, no dictionary, no progress callback, no stats
           memset(p_buf_out, 0, buf_size_out);
           out_len = salvador_compress(p_buf_in, p_buf_out, buf_size_in, buf_size_out, FLG_IS_INVERTED, 0, 0, NULL, NULL);
        } else
            return EXIT_FAILURE;

        if (out_len > 0) {

            if (opt_c_source_output) {
                c_source_set_sizes(out_len, buf_size_in); // compressed, decompressed
                result = file_write_c_output_from_buffer(filename_out, p_buf_out, out_len,
                                                        opt_c_source_output_var_name, opt_c_source_output_bankref_name,
                                                        VAR_IS_CONST_YES, opt_bank_num);
            }
            else
                result = file_write_from_buffer(filename_out, p_buf_out, out_len);

            if (result) {
                if (opt_verbose)
                    printf("Compressed: %d bytes -> %d bytes (%%%.2f)\n", buf_size_in, out_len, ((double)out_len / (double)buf_size_in) * 100);
                return EXIT_SUCCESS;
            }
        }
    }

    return EXIT_FAILURE;
}


static int decompress() {

    uint32_t  buf_size_in = 0;
    uint32_t  buf_size_out = 0;
    uint32_t  out_len = 0;
    bool      result = false;

    if (opt_c_source_input)
        p_buf_in =  file_read_c_input_into_buffer(filename_in, &buf_size_in);
    else
        p_buf_in =  file_read_into_buffer(filename_in, &buf_size_in);

    // Allocate buffer output buffer 3x size of input
    // It can grow more in gbdecompress_buf()
    if (opt_compression_type == COMPRESSION_TYPE_ZX0)
        buf_size_out = salvador_get_max_decompressed_size(p_buf_in, buf_size_in, 0);  // No flags
    else
        buf_size_out = buf_size_in * 3;

    p_buf_out = malloc(buf_size_out);

    if ((p_buf_in) && (p_buf_out) && (buf_size_in > 0)) {

        if (opt_compression_type == COMPRESSION_TYPE_GB)
            out_len = gbdecompress_buf(p_buf_in, buf_size_in, &p_buf_out, buf_size_out);
        else if (opt_compression_type == COMPRESSION_TYPE_RLE_BLOCK)
            out_len = rledecompress_buf(p_buf_in, buf_size_in, &p_buf_out, buf_size_out);
        else if (opt_compression_type == COMPRESSION_TYPE_ZX0) {
            memset(p_buf_out, 0, buf_size_out);            
            out_len = salvador_decompress(p_buf_in, p_buf_out, buf_size_in, buf_size_out, 0, FLG_IS_INVERTED); // No dictionary, FLG_IS_INVERTED Flag
        } else
            return EXIT_FAILURE;

        if (out_len > 0) {

            if (opt_c_source_output) {
                c_source_set_sizes(buf_size_in, out_len); // compressed, decompressed
                result = file_write_c_output_from_buffer(filename_out, p_buf_out, out_len,
                                                         opt_c_source_output_var_name, opt_c_source_output_bankref_name,
                                                         VAR_IS_CONST_YES, opt_bank_num);
            }
            else {
                result = file_write_from_buffer(filename_out, p_buf_out, out_len);
            }

            if (result) {
                if (opt_verbose)
                    printf("Decompressed: %d bytes -> %d bytes (compression was %%%.2f)\n", buf_size_in, out_len, ((double)buf_size_in / (double)out_len) * 100);
                return EXIT_SUCCESS;
            }
        } else if (out_len == 0) {
            printf("gbcompress: ERROR: Failed to decompress any bytes (from %d input). Data may not be compressed\n", buf_size_in);
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
