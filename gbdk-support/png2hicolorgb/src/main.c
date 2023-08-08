// See LICENSE  file for license details

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>


#include "common.h"
#include "path_ops.h"
#include "options.h"
#include "logging.h"
#include "image_load.h"

#include <hicolour.h>


#define VERSION "version 1.4.1"

static void init(void);
static void cleanup(void);
static void display_help(void);
static int  handle_args(int argc, char * argv[]);
static void set_drag_and_drop_mode_defaults(void);

image_data src_image;
char filename_in[MAX_STR_LEN] = {'\0'};
char opt_filename_out[MAX_STR_LEN] = "";
// bool opt_strip_output_filename_ext = true;

int  show_help_and_exit = false;


static void init(void) {
    // Handle global init
    src_image.p_img_data = NULL;
    hicolor_init();
}


// Registered as an atexit handler
static void cleanup(void) {
    // Handle any cleanup on exit
    // Free the image data
    if (src_image.p_img_data != NULL)
        free(src_image.p_img_data);

}


int main( int argc, char *argv[] )  {

    int ret = EXIT_FAILURE; // Default to failure on exit

    // Register cleanup with exit handler
    atexit(cleanup);

    init();

    #ifdef DRAG_AND_DROP_MODE
        // Non-interactive mode, set some reasonable default
        set_drag_and_drop_mode_defaults();
    #endif

    if (handle_args(argc, argv)) {

        if (show_help_and_exit) {
            ret = EXIT_SUCCESS;
        }
        else {
            // detect file extension
            if (matches_extension(filename_in, (char *)".png")) {

                // Load source image (from first argument)
                if (image_load(&src_image, filename_in, IMG_TYPE_PNG)) {

                    // If output filename is not specified, use source filename
                    if (opt_filename_out[0] == '\0')
                        strcpy(opt_filename_out, filename_in);

                    filename_remove_extension(opt_filename_out);
                    hicolor_process_image(&src_image, opt_filename_out);
                    ret = EXIT_SUCCESS; // Exit with success
                }
            }
        }
    }

    // Override exit code if was set during processing
    if (get_exit_error())
        ret = EXIT_FAILURE;

    // if (ret == EXIT_FAILURE)
    //     log_error("Error loading, converting or writing out the image: %s\n", filename_in);

    #ifdef DRAG_AND_DROP_MODE
        // Wait for input to keep the console window open after processing
        log_standard("\n\nPress Any Key to Continue\n");
        getchar();
    #endif

    return ret;
}


static void display_help(void) {
    log_standard(
        "\n"
        "png2hicolorgb input_image.png [options]\n"
        VERSION": bbbbbr. Based on Glen Cook's Windows GUI \"hicolour.exe\" 1.2\n"
        "Convert an image to Game Boy Hi-Color format\n"
        "\n"
        "Options\n"
        "-h         : Show this help\n"
        "-v*        : Set log level: \"-v\" verbose, \"-vQ\" quiet, \"-vE\" only errors, \"-vD\" debug\n"
        "-o <file>  : Set base output filename (otherwise from input image)\n"
        // "--keepext   : Do not strip extension from output filename\n"
        "--csource  : Export C source format with incbins for data files\n"
        "--bank=N   : Set bank number for C source output where N is decimal bank number 1-511\n"
        "--type=N   : Set conversion type where N is one of below \n"
        "              1: Median Cut - No Dither (*Default*)\n"
        "              2: Median Cut - With Dither\n"
        "              3: Wu Quantiser\n"
        "-p         : Show screen attribute pattern options (no processing)\n"
        "-L=N       : Set Left  screen attribute pattern where N is decimal entry (-p to show patterns)\n"
        "-R=N       : Set Right screen attribute pattern where N is decimal entry (-p to show patterns)\n"
        "--vaddrid  : Map uses vram id (128->255->0->127) instead of (*Default*) sequential tile order (0->255)\n"
        "--nodedupe : Turn off tile pattern deduplication\n"
        "\n"
        "Example 1: \"png2hicolorgb myimage.png\"\n"
        "Example 2: \"png2hicolorgb myimage.png --csource -o=my_output_filename\"\n"
        "* Default settings provide good results. Better quality but slower: \"--type=3 -L=2 -R=2\"\n"
        "\n"
        "Historical credits and info:\n"
        "   Original Concept : Icarus Productions\n"
        "   Original Code : Jeff Frohwein\n"
        "   Full Screen Modification : Anon\n"
        "   Adaptive Code : Glen Cook\n"
        "   Windows Interface : Glen Cook\n"
        "   Additional Windows Programming : Rob Jones\n"
        "   Original Quantiser Code : Benny\n"
        "   Quantiser Conversion : Glen Cook\n"
        "\n"
   );
}


// Default options for Windows Drag and Drop recipient mode
static void set_drag_and_drop_mode_defaults(void) {

    // Set some options here
}


static int handle_args(int argc, char * argv[]) {

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

        if ((strstr(argv[i], "-h") == argv[i]) || (strstr(argv[i], "-?") == argv[i])) {
            display_help();
            show_help_and_exit = true;
            return true;  // Don't parse further input when -h is used
        } else if (strstr(argv[i], "-p") == argv[i]) {
            log_standard(HELP_CONV_PATTERN_STR);
            show_help_and_exit = true;
            return true;  // Don't parse further input when -h is used

        } else if (strstr(argv[i], "-vD") == argv[i]) {
            log_set_level(OUTPUT_LEVEL_DEBUG);
        } else if (strstr(argv[i], "-vE") == argv[i]) {
            log_set_level(OUTPUT_LEVEL_ONLY_ERRORS);
        } else if (strstr(argv[i], "-vQ") == argv[i]) {
            log_set_level(OUTPUT_LEVEL_QUIET);
        } else if (strstr(argv[i], "-v") == argv[i]) {
            log_set_level(OUTPUT_LEVEL_VERBOSE);

        } else if (strstr(argv[i], "--type=") == argv[i]) {
            uint8_t new_type = strtol(argv[i] + strlen("--type="), NULL, 10);
            if ((new_type < CONV_TYPE_MIN) || (new_type > CONV_TYPE_MAX)) {
                log_standard("Error: --type specified with invalid conversion setting: %d\n", new_type);
                display_help();
                show_help_and_exit = true;
                return false; // Abort
            }
            else
                hicolor_set_type(new_type);
        } else if (strstr(argv[i], "-L=") == argv[i]) {
            hicolor_set_convert_left_pattern( strtol(argv[i] + strlen("-L="), NULL, 10));
        } else if (strstr(argv[i], "-R=") == argv[i]) {
            hicolor_set_convert_right_pattern( strtol(argv[i] + strlen("-R="), NULL, 10));

        } else if (strstr(argv[i], "-o") == argv[i]) {
            if (i < (argc -1))
                i++; // Move to next argument if one is available
            else {
                log_standard("Error: -o specified but filename is missing\n");
                show_help_and_exit = true;
                return false; // Abort
            }

            // Require colon and filename to be present
            if (*argv[i] == '-')
                log_standard("Warning: -o specified but filename has dash and looks like an option argument. Usage: -o my_base_output_filename\n");
            snprintf(opt_filename_out, sizeof(opt_filename_out), "%s", argv[i]);

        // } else if (strstr(argv[i], "--keepext") == argv[i]) {
        //     opt_strip_output_filename_ext = false;

        } else if (strstr(argv[i], "--csource") == argv[i]) {
            opt_set_c_file_output(true);

        } else if (strstr(argv[i], "--bank=") == argv[i]) {
            opt_set_bank_num( strtol(argv[i] + strlen("--bank="), NULL, 10) );
            if ((opt_get_bank_num() < BANK_NUM_MIN) || (opt_get_bank_num() > BANK_NUM_MAX)) {
                log_standard("Error: Invalid bank number specified with --bank=%d\n", opt_get_bank_num());
                display_help();
                show_help_and_exit = true;
                return false; // Abort
            }

        } else if (strstr(argv[i], "--vaddrid") == argv[i]) {
            opt_set_map_tile_order(OPT_MAP_TILE_ORDER_BY_VRAM_ID);

        } else if (strstr(argv[i], "--nodedupe") == argv[i]) {
            opt_set_tile_dedupe(false);

        } else if (argv[i][0] == '-') {
            log_error("Unknown argument: %s\n\n", argv[i]);
            display_help();
            return false;
        }

    }

    return true;
}

