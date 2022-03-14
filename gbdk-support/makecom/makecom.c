// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2022

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#include "path_ops.h"
#include "common.h"
#include "files.h"
#include "noi_file.h"
#include "bin_to_com.h"

#define EXT_NOI "noi"

char filename_in_bin[MAX_STR_LEN] = "";
char filename_in_noi[MAX_STR_LEN] = "";
char filename_out_com[MAX_STR_LEN] = "";
char filename_banks_base[MAX_STR_LEN] = "";
char filename_overlay[COM_OVERLAY_NAME_LEN] = "";

uint8_t * p_rom_buf_in  = NULL;
size_t    rom_buf_in_len = 0;

static void str_to_upper(char * str);
static void filenames_out_prepare(void);
static void display_help(void);
static int handle_args(int argc, char * argv[]);
void cleanup(void);


static void str_to_upper(char * str) {
    while (*str) {
        *str = toupper(*str);
        str++;
    }
}



// Generate names for the exported bank/overlay files
// based on the output COM filename and path
static void filenames_out_prepare(void) {

    char str_banks_name[MAX_STR_LEN];
    char str_path_only[MAX_STR_LEN];

    snprintf(str_banks_name,  sizeof(str_banks_name), "%s", filename_out_com);

    if (!get_path_without_filename(filename_out_com, str_path_only, sizeof(str_path_only))) {
        printf("makecom: Error: source path %s exceeds max length\n", filename_out_com);
        exit;
    }

    // Convert filename to uppercase and remove extension
    str_to_upper(str_banks_name);
    filename_remove_extension(str_banks_name);

    // Then truncate it to 8 characters with no padding for bank filenames
    int ret = snprintf(filename_banks_base, sizeof(filename_banks_base), "%s%.8s", str_path_only, get_filename_from_path(str_banks_name));
    if (ret < 0) printf("makecom: Warning, banks output path truncated\n");
    // Then with trailing space char padding for the overlay filename patch
    snprintf(filename_overlay, sizeof(filename_overlay), "%-8.8s", get_filename_from_path(str_banks_name));
}


static void display_help(void) {

    fprintf(stdout,
       "makecom image.rom image.noi output.com\n"
       "Use: convert a binary .rom file to .msxdos com format.\n"
       );
}


int handle_args(int argc, char * argv[]) {

    if (argc == 3) {
        // Copy input and output filenames from arguments
        snprintf(filename_in_bin,  sizeof(filename_in_bin),  "%s", argv[1]);
        snprintf(filename_out_com, sizeof(filename_out_com), "%s", argv[2]);
        // Generate .noi file name from input .bin file name
        snprintf(filename_in_noi,  sizeof(filename_in_noi),  "%s", argv[1]);
        filename_replace_extension(filename_in_noi, EXT_NOI, MAX_STR_LEN);
    } else if (argc == 4) {
        // Copy input and output filenames from arguments
        snprintf(filename_in_bin,  sizeof(filename_in_bin),  "%s", argv[1]);
        snprintf(filename_in_noi,  sizeof(filename_in_noi),  "%s", argv[2]);
        snprintf(filename_out_com, sizeof(filename_out_com), "%s", argv[3]);
    } else {
        display_help();
        return false;
    }

    filenames_out_prepare();


    // printf("bin: %s\n",filename_in_bin);
    // printf("noi: %s\n",filename_in_noi);
    // printf("com: %s\n",filename_out_com);

    // printf("banks base: _%s_\n",filename_banks_base);
    // printf("overlay   : _%s_\n",filename_overlay);


    return true;
}


// Registered as atexit() handler
// Free resources during normal shutdown or from a call to exit()
void cleanup(void) {

    banks_cleanup();
    noi_cleanup();

    if (p_rom_buf_in != NULL) {
        free(p_rom_buf_in);
        p_rom_buf_in = NULL;
    }
}


int main( int argc, char *argv[] )  {

    // Exit with failure by default
    int ret = EXIT_FAILURE;

    // Register cleanup with exit handler
    atexit(cleanup);

    if (handle_args(argc, argv)) {

        p_rom_buf_in = file_read_into_buffer(filename_in_bin, &rom_buf_in_len);

        if (p_rom_buf_in)
            if (noi_file_load_symbols(filename_in_noi))
                ret = bin2com();
    }
    cleanup();

    return ret; // Exit with failure by default
}
