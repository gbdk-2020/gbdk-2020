// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "logging.h"
#include "banks.h"
#include "banks_color.h"
#include "map_file.h"
#include "noi_file.h"
#include "ihx_file.h"
#include "cdb_file.h"
#include "rom_file.h"

#define VERSION "version 1.3.0"

enum {
    HELP_FULL = 0,
    HELP_BRIEF
};

void static display_cdb_warning(void);
void static display_help(int mode);
int handle_args(int argc, char * argv[]);
static bool matches_extension(char *, char *);
static void init(void);
void cleanup(void);


char filename_in[MAX_STR_LEN] = {'\0'};
int  show_help_and_exit = false;


static void display_cdb_warning() {
    printf("\n"
           "   ************************ NOTICE ************************ \n"
           "    .cdb output ONLY counts (most) data from C sources.     \n"
           "   It cannot count functions and data from ASM and LIBs.    \n"
           "   Bank totals may be incorrect/missing. (-nB to hide this) \n"
           "   ************************ NOTICE ************************ \n");
}

static void display_help(int mode) {
    fprintf(stdout,
           "romusage input_file.[map|noi|ihx|cdb|.gb[c]|.pocket|.duck|.gg|.sms] [options]\n"
           VERSION", by bbbbbr\n"
           "\n"
           "Options\n"
           "-h  : Show this help\n"
           "-p:SMS_GG : Set platform to GBDK SMS/Game Gear (changes memory map templates)\n"
           "\n"
           "-a  : Show Areas in each Bank. Optional sort by, address:\"-aA\" or size:\"-aS\" \n"
           "-g  : Show a small usage graph per bank (-gA for ascii style)\n"
           "-G  : Show a large usage graph per bank (-GA for ascii style)\n"
           "-B  : Brief (summarized) output for banked regions. Auto scales max bank\n"
           "      shows [Region]_[Max Used Bank] / [auto-sized Max Bank Num]\n"
           "-F  : Force Max ROM and SRAM bank num for -B. (0 based) -F:ROM:SRAM (ex: -F:255:15)\n"
           "\n"
           "-m  : Manually specify an Area -m:NAME:HEXADDR:HEXLENGTH\n"
           "-e  : Manually specify an Area that should not overlap -e:NAME:HEXADDR:HEXLENGTH\n"
           "-b  : Set hex bytes treated as Empty in ROM files (.gb/etc) -b:HEXVAL[...] (default FF)\n"
           "-E  : All areas are exclusive (except HEADERs), warn for any overlaps\n"
           "-q  : Quiet, no output except warnings and errors\n"
           "-Q  : Suppress output of warnings and errors\n"
           "-R  : Return error code for Area warnings and errors\n"
           "\n"
           "-sR : [Rainbow] Color output (-sRe for Row Ends, -sRd for Center Dimmed, -sRp %% based)\n"
           "-sP : Custom Color Palette. Colon separated entries are decimal VT100 color codes\n"
           "      -sP:DEFAULT:ROM:VRAM:SRAM:WRAM:HRAM (section based color only)\n"
           "-sC : Show Compact Output, hide non-essential columns\n"
           "-sH : Show HEADER Areas (normally hidden)\n"
           "-smROM  : Show Merged ROM_0  and ROM_1  output (i.e. bare 32K ROM)\n"
           "-smWRAM : Show Merged WRAM_0 and WRAM_1 output (i.e DMG/MGB not CGB)\n"
           "          -sm* compatible with banked ROM_x or WRAM_x when used with -B\n"
           "-sJ   : Show JSON output. Some options not applicable. When used, -Q recommended\n"
           "-nB   : Hide warning banner (for .cdb output)\n"
           "-nA   : Hide areas (shown by default in .cdb output)\n"
           "-z    : Hide areas smaller than SIZE -z:DECSIZE\n"
           "-nMEM : Hide banks matching case sensitive substring (ex hide all RAM: -nMEM:RAM)\n"
           "\n");

    if (mode == HELP_FULL) {
        fprintf(stdout,
           "Use: Read a .map, .noi, .cdb or .ihx file to display area sizes\n"
           "Example 1: \"romusage build/MyProject.map\"\n"
           "Example 2: \"romusage build/MyProject.noi -a -e:STACK:DEFF:100 -e:SHADOW_OAM:C000:A0\"\n"
           "Example 3: \"romusage build/MyProject.ihx -g\"\n"
           "Example 4: \"romusage build/MyProject.map -q -R\"\n"
           "Example 5: \"romusage build/MyProject.noi -sR -sP:90:32:90:35:33:36\"\n"
           "Example 6: \"romusage build/MyProject.map -sRp -g -B -F:255:15 -smROM -smWRAM\"\n"
           "Example 7: \"romusage build/MyProject.gb  -g -b:FF:00\"\n"
           "\n"
           "Notes:\n"
           "  * GBDK / RGBDS map file format detection is automatic.\n"
           "  * Estimates are as close as possible, but may not be complete.\n"
           "    Unless specified with -m/-e they *do not* factor regions lacking\n"
           "    complete ranges in the Map/Noi/Ihx file, for example Shadow OAM and Stack.\n"
           "  * IHX files can only detect overlaps, not detect memory region overflows.\n"
           "  * CDB file output ONLY counts (most) data from C sources.\n"
           "    It cannot count functions and data from ASM and LIBs,\n"
           "    so bank totals may be incorrect/missing.\n"
           "  * GB/GBC/ROM files are just guessing, no promises.\n"
           );
    }
}


// Default options for Windows Drag and Drop recipient mode
void set_drag_and_drop_mode_defaults(void) {

    set_option_color_mode(OPT_PRINT_COLOR_ROW_ENDS);
    banks_output_show_minigraph(true);
    set_option_summarized(true);
}


int handle_args(int argc, char * argv[]) {

    int i;

    if( argc < 2 ) {
        display_help(HELP_FULL);
        return false;
    }

    // Start at first optional argument, argc is zero based
    for (i = 0; i <= (argc -1); i++ ) {

        if (strstr(argv[i], "-h") == argv[i]) {
            display_help(HELP_FULL);
            show_help_and_exit = true;
            return true;  // Don't parse further input when -h is used
        } else if (strstr(argv[i], "-a") == argv[i]) {
            banks_output_show_areas(true);
            if      (argv[i][2] == 'S') set_option_area_sort(OPT_AREA_SORT_SIZE_DESC);
            else if (argv[i][2] == 'A') set_option_area_sort(OPT_AREA_SORT_ADDR_ASC);

        } else if (strstr(argv[i], "-sR") == argv[i]) {
            switch (argv[i][ + strlen("-sR")]) {
                case 'p': set_option_percentage_based_color(true); break; // Turns on default color mode if not set
                case 'e': set_option_color_mode(OPT_PRINT_COLOR_ROW_ENDS); break;
                case 'd': set_option_color_mode(OPT_PRINT_COLOR_WHOLE_ROW_DIMMED); break;
                case 'w': set_option_color_mode(OPT_PRINT_COLOR_WHOLE_ROW); break;
                default:  set_option_color_mode(OPT_PRINT_COLOR_DEFAULT); break;
            }
        } else if (strstr(argv[i], "-sP") == argv[i]) {
            if (!set_option_custom_bank_colors(argv[i])) {
                log_error("Malformed -sP custom color palette: %s\n\n", argv[i]);
                // display_help();
                return false;
            }
        } else if (strstr(argv[i], "-sH") == argv[i]) {
            banks_output_show_headers(true);

        } else if (strstr(argv[i], "-sC") == argv[i]) {
            set_option_show_compact(true);

        } else if (strstr(argv[i], "-sJ") == argv[i]) {
            set_option_show_json(true);

        } else if (strstr(argv[i], "-nB") == argv[i]) {
            set_option_hide_banners(true);

        } else if (strstr(argv[i], "-nA")) {
            set_option_area_sort(OPT_AREA_SORT_HIDE);

        } else if (strstr(argv[i], "-p:SMS_GG") == argv[i]) {
            set_option_platform(OPT_PLAT_SMS_GG_GBDK);

        } else if (strstr(argv[i], "-g") == argv[i]) {
            banks_output_show_minigraph(true);
            if (argv[i][2] == 'A') set_option_display_asciistyle(true);
        } else if (strstr(argv[i], "-G") == argv[i]) {
            banks_output_show_largegraph(true);
            if (argv[i][2] == 'A') set_option_display_asciistyle(true);
        } else if (strstr(argv[i], "-E") == argv[i]) {
            set_option_all_areas_exclusive(true);

        } else if (strstr(argv[i], "-B") == argv[i]) {
            set_option_summarized(true);
        } else if (strstr(argv[i], "-F") == argv[i]) {
            if (!set_option_displayed_bank_range(argv[i])) {
                log_error("Malformed -F forced display max bank range\n\n");
                // display_help();
                return false;
            }

        } else if (strstr(argv[i], "-b") == argv[i]) {
            if (!set_option_binary_rom_empty_values(argv[i] + strlen("-b"))) {
                log_error("Malformed or no entries found -b set hex values treated as empty for ROM files: %s\n\n", argv[i]);
                return false;
            }

        } else if (strstr(argv[i], "-smWRAM") == argv[i]) {
            set_option_merged_banks(OPT_MERGED_BANKS_WRAM);
        } else if (strstr(argv[i], "-smROM") == argv[i]) {
            set_option_merged_banks(OPT_MERGED_BANKS_ROM);

        } else if (strstr(argv[i], "-q") == argv[i]) {
            set_option_quiet_mode(true);
        } else if (strstr(argv[i], "-Q") == argv[i]) {
            log_set_level(OUTPUT_LEVEL_QUIET);
        } else if (strstr(argv[i], "-R") == argv[i]) {
            set_option_error_on_warning(true);

        } else if (strstr(argv[i], "-z:") == argv[i]) {
            set_option_area_hide_size( strtol(argv[i] + 3, NULL, 10));

        } else if (strstr(argv[i], "-nMEM:") == argv[i]) {
            if (!set_option_banks_hide_add(argv[i] + strlen("-nMEM:"))) {
                log_error("Adding memory region to hide failed: %s\n\n", argv[i]);
                // display_help();
                return false;
            }

        } else if ((strstr(argv[i], "-m") == argv[i]) ||
                   (strstr(argv[i], "-e") == argv[i])) {
            if (!area_manual_queue(argv[i])) {
                log_error("Malformed -m or -e manual area argument: %s\n\n", argv[i]);
                // display_help();
                return false;
            }

        } else if (argv[i][0] == '-') {
            log_error("Error: Unknown argument: %s\n\n", argv[i]);
            display_help(HELP_BRIEF);
            return false;
        }

        // Copy input filename (if not preceded with option dash)
        else if (argv[i][0] != '-') {
            snprintf(filename_in, sizeof(filename_in), "%s", argv[i]);
        }
    }

    return true;
}


// Case insensitive
static bool matches_extension(char * filename, char * extension) {

    if (strlen(filename) >= strlen(extension)) {
        char * str_ext = filename + (strlen(filename) - strlen(extension));

        return (strncasecmp(str_ext, extension, strlen(extension)) == 0);
    }
    else
        return false;
}


static void init(void) {
    cdb_init();
    noi_init();
    banks_init();
    romfile_init_defaults();
}


// Register as an exit handler
void cleanup(void) {
    cdb_cleanup();
    noi_cleanup();
    banks_cleanup();
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

        banks_init_templates();
        area_manual_apply_queued();

        if (show_help_and_exit) {
            ret = EXIT_SUCCESS;
        }
        else {
            // detect file extension
            if (matches_extension(filename_in, (char *)".noi")) {
                if (noi_file_process_areas(filename_in)) {
                    banklist_finalize_and_show();
                    ret = EXIT_SUCCESS; // Exit with success
                }
            } else if (matches_extension(filename_in, (char *)".map")) {
                if (map_file_process_areas(filename_in)) {
                    banklist_finalize_and_show();
                    ret = EXIT_SUCCESS; // Exit with success
                }
            } else if (matches_extension(filename_in, (char *)".ihx")) {
                if (ihx_file_process_areas(filename_in)) {
                    banklist_finalize_and_show();
                    ret = EXIT_SUCCESS; // Exit with success
                }
            } else if (matches_extension(filename_in, (char *)".gb"    ) ||
                       matches_extension(filename_in, (char *)".gbc"   ) ||
                       matches_extension(filename_in, (char *)".sms"   ) ||
                       matches_extension(filename_in, (char *)".gg"   ) ||
                       matches_extension(filename_in, (char *)".pocket") ||
                       matches_extension(filename_in, (char *)".duck") ) {
                // printf("ROM FILE\n");
                if (rom_file_process(filename_in)) {
                    banklist_finalize_and_show();
                    ret = EXIT_SUCCESS; // Exit with success
                }
            } else if (matches_extension(filename_in, (char *)".cdb")) {
                if (cdb_file_process_symbols(filename_in)) {
                    if (!get_option_hide_banners()) display_cdb_warning();

                    banklist_finalize_and_show();

                    if (!get_option_hide_banners()) display_cdb_warning();
                    ret = EXIT_SUCCESS; // Exit with success
                }
            } else {
                log_error("Error: Incompatible file extension\n");
            }
        }
    }

    // if (ret == EXIT_FAILURE)
    //     printf("Problem with filename or unable to open file! %s\n", filename_in);

    // Override exit code if was set during processing
    if (get_exit_error())
        ret = EXIT_FAILURE;

    #ifdef DRAG_AND_DROP_MODE
        // Wait for input to keep the console window open after processing
        printf("\n\nPress Any Key to Continue\n");
        getchar();
    #endif

    return ret;
}
