// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include "obj_data.h"
#include "files.h"

bool g_option_verbose = false;
bool g_option_cartsize = false;

static void display_help(void);
static int handle_args(int argc, char * argv[]);
static void option_set_verbose(bool is_enabled);
static void init(void);
void cleanup(void);


static void display_help(void) {
    fprintf(stdout,
       "bankalloc [options] objfile1 objfile2 etc\n"
       "Use: Read .o files and auto-assign areas with bank=255.\n"
       "     Typically called by Lcc compiler driver before linker.\n"
       "\n"
       "Options\n"
       "-h           : Show this help\n"
       "-yt<mbctype> : Set MBC type per ROM byte 149 in Decimal or Hex (0xNN) (see pandocs)\n"
       "-mbc=N       : Similar to -yt, but sets MBC type directly to N instead\n"
       "               of by intepreting ROM byte 149\n"
       "               mbc1 will exclude banks {0x20,0x40,0x60} max=127, \n"
       "               mbc2 max=15, mbc3 max=127, mbc5 max=255 (not 511!) \n"
       "-min=N       : Min assigned ROM bank is N (default 1)\n"
       "-max=N       : Max assigned ROM bank is N, error if exceeded\n"
       "-ext=<.ext>  : Write files out with <.ext> instead of source extension\n"
       "-path=<path> : Write files out to <path> (<path> *MUST* already exist)\n"
       "-sym=<prefix>: Add symbols starting with <prefix> to match + update list.\n"
       "               Default entry is \"___bank_\" (see below)\n"
       "-cartsize    : Print min required cart size as \"autocartsize:<NNN>\"\n"
       "-plat=<plat> : Select platform specific behavior (default:gb) (gb,sms)\n"
       "-v           : Verbose output, show assignments\n"
       "\n"
       "Example: \"bankpack -ext=.rel -path=some/newpath/ file1.o file2.o\"\n"
       "Unless -ext or -path specify otherwise, input files are overwritten.\n"
       "\n"
       "Default MBC type is not set. It *must* be specified by -mbc= or -yt!\n"
       "\n"
       "The following will have FF and 255 replaced with the assigned bank:\n"
       "A _CODE_255 size <size> flags <flags> addr <address>\n"
       "S b_<function name> Def0000FF\n"
       "S ___bank_<const name> Def0000FF\n"
       "    (Above can be made by: const void __at(255) __bank_<const name>;\n"
       );
}


static int handle_args(int argc, char * argv[]) {

    int i;

    if( argc < 2 ) {
        display_help();
        return false;
    }

    // Start at first optional argument, argc is zero based
    for (i = 1; i <= (argc -1); i++ ) {

        if (argv[i][0] == '-') {
            if (strstr(argv[i], "-h") == argv[i]) {
                display_help();
                return false;  // Don't parse input when -h is used
            } else if (strstr(argv[i], "-min=") == argv[i]) {
                if (!banks_set_min(atoi(argv[i] + 5))) {
                    printf("BankPack: ERROR: Invalid min bank: %s\n", argv[i] + 5);
                    return false;
                }
            } else if (strstr(argv[i], "-max=") == argv[i]) {
                if (!banks_set_max(atoi(argv[i] + 5))) {
                    printf("BankPack: ERROR: Invalid max bank: %s\n", argv[i] + 5);
                    return false;
                }
            } else if (strstr(argv[i], "-ext=") == argv[i]) {
                files_set_out_ext(argv[i] + 5);
            } else if (strstr(argv[i], "-path=") == argv[i]) {
                files_set_out_path(argv[i] + 6);
            } else if (strstr(argv[i], "-mbc=") == argv[i]) {
                banks_set_mbc(atoi(argv[i] + 5));
            } else if (strstr(argv[i], "-yt") == argv[i]) {
                banks_set_mbc_by_rom_byte_149(strtol(argv[i] + 3, NULL, 0));
            } else if (strstr(argv[i], "-v") == argv[i]) {
                option_set_verbose(true);
            } else if (strstr(argv[i], "-sym=") == argv[i]) {
                symbol_match_add(argv[i] + 5);
            } else if (strstr(argv[i], "-cartsize") == argv[i]) {
                g_option_cartsize = true;
            } else if (strstr(argv[i], "-plat=") == argv[i]) {
                banks_set_platform(argv[i] + 6);
            } else
                printf("BankPack: Warning: Ignoring unknown option %s\n", argv[i]);
        } else {
            // Add to list of object files to process
            files_add(argv[i]);
        }
    }

    return true;
}


static void option_set_verbose(bool is_enabled) {
    g_option_verbose = is_enabled;
}


static int matches_extension(char * filename, char * extension) {
    return (strcmp(filename + (strlen(filename) - strlen(extension)), extension) == 0);
}


static void init(void) {
    files_init();
    obj_data_init();
}


void cleanup(void) {
    files_cleanup();
    obj_data_cleanup();
}


int main( int argc, char *argv[] )  {

    // Exit with failure by default
    int ret = EXIT_FAILURE;

    // Register cleanup with exit handler
    atexit(cleanup);

    init();

    if (handle_args(argc, argv)) {

        // Require MBC for Game Boy
        // SMS doesn't require an MBC setting
        if ((banks_get_platform() == PLATFORM_GB) && (banks_get_mbc_type() == MBC_TYPE_NONE))
            printf("BankPack: ERROR: auto-banking does not work with unbanked ROMS (no MBC for Game Boy)\n");
        else {
            // Extract areas, sort and assign them to banks
            // then rewrite object files as needed
            files_extract();
            files_rewrite();

            if (g_option_verbose)
                banks_show();
            if (g_option_cartsize)
                fprintf(stdout,"autocartsize:%d\n",banks_calc_cart_size());

            cleanup();
            ret = EXIT_SUCCESS;
        }
    }

    return ret; // Exit with failure by default
}