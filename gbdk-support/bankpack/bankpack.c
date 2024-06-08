// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "obj_data.h"
#include "files.h"
#include "options.h"

static void display_help(void);
static int handle_args(int argc, char * argv[]);
static void init(void);
void cleanup(void);


static void display_help(void) {
    fprintf(stdout,
       "bankalloc [options] objfile1 objfile2 etc\n"
       "Use: Read .o files and auto-assign areas with bank=255.\n"
       "     Typically called by Lcc compiler driver before linker.\n"
       "\n"
       "Options\n"
       "-h             : Show this help\n"
       "-lkin=<file>   : Load object files specified in linker file <file>\n"
       "-lkout=<file>  : Write list of object files out to linker file <file>\n"
       "-yt<mbctype>   : Set MBC type per ROM byte 149 in Decimal or Hex (0xNN)\n"
       "                ([see pandocs](https://gbdev.io/pandocs/The_Cartridge_Header.html#0147---cartridge-type))\n"
       "-mbc=N         : Similar to -yt, but sets MBC type directly to N instead\n"
       "                of by intepreting ROM byte 149\n"
       "                mbc1 will exclude banks {0x20,0x40,0x60} max=127, \n"
       "                mbc2 max=15, mbc3 max=127, mbc5 max=255 (not 511!) \n"
       "-min=N         : Min assigned ROM bank is N (default 1)\n"
       "-max=N         : Max assigned ROM bank is N, error if exceeded\n"
       "-ext=<.ext>    : Write files out with <.ext> instead of source extension\n"
       "-path=<path>   : Write files out to <path> (<path> *MUST* already exist)\n"
       "-sym=<prefix>  : Add symbols starting with <prefix> to match + update list\n"
       "                 Default entry is \"___bank_\" (see below)\n"
       "-cartsize      : Print min required cart size as \"autocartsize:<NNN>\"\n"
       "-plat=<plat>   : Select platform specific behavior (default:gb) (gb,sms)\n"
       "-random        : Distribute banks randomly for testing (honors -min/-max)\n"
       "-reserve=<b:n> : Reserve N bytes (hex) in bank B (decimal)\n"
       "                 Ex: -reserve=105:30F reserves 0x30F bytes in bank 105\n"
       "-banktype=<b:t>: Set bank B (decimal) to use type T (CODE or LIT). For sms/gg\n"
       "                 Ex: -banktype=2:LIT sets bank 2 to type LIT\n"
       "-v             : Verbose output, show assignments\n"
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
                if (!option_banks_set_min(atoi(argv[i] + 5))) {
                    printf("BankPack: ERROR: Invalid min bank: %s\n", argv[i] + 5);
                    return false;
                }
            } else if (strstr(argv[i], "-max=") == argv[i]) {
                if (!option_banks_set_max(atoi(argv[i] + 5))) {
                    printf("BankPack: ERROR: Invalid max bank: %s\n", argv[i] + 5);
                    return false;
                }
            } else if (strstr(argv[i], "-ext=") == argv[i]) {
                files_set_out_ext(argv[i] + 5);
            } else if (strstr(argv[i], "-path=") == argv[i]) {
                files_set_out_path(argv[i] + 6);
            } else if (strstr(argv[i], "-mbc=") == argv[i]) {
                option_set_mbc(atoi(argv[i] + 5));
            } else if (strstr(argv[i], "-mapper=") == argv[i]) {
                option_set_nes_mapper(atoi(argv[i] + 8));
            } else if (strstr(argv[i], "-yt") == argv[i]) {
                option_mbc_by_rom_byte_149(strtol(argv[i] + 3, NULL, 0));
            } else if (strstr(argv[i], "-v") == argv[i]) {
                option_set_verbose(true);
            } else if (strstr(argv[i], "-sym=") == argv[i]) {
                symbol_match_add(argv[i] + 5);
            } else if (strstr(argv[i], "-cartsize") == argv[i]) {
                option_set_cartsize(true);
            } else if (strstr(argv[i], "-plat=") == argv[i]) {
                option_set_platform(argv[i] + 6);
            } else if (strstr(argv[i], "-random") == argv[i]) {
                option_set_random_assign(true);
            } else if (strstr(argv[i], "-lkin=") == argv[i]) {
                files_read_linkerfile(argv[i] + strlen("-lkin="));
            } else if (strstr(argv[i], "-lkout=") == argv[i]) {
                files_set_linkerfile_outname(argv[i] + strlen("-lkout="));
            } else if (strstr(argv[i], "-reserve=") == argv[i]) {
                if (!option_bank_reserve_bytes(argv[i])) {
                    fprintf(stdout,"BankPack: ERROR! Malformed argument: %s\n\n", argv[i]);
                    display_help();
                    return false;
                }
            } else if (strstr(argv[i], "-banktype=") == argv[i]) {
                if (!option_bank_set_type(argv[i])) {
                    fprintf(stdout,"BankPack: ERROR! Malformed argument: %s\n\n", argv[i]);
                    display_help();
                    return false;
                }
            } else {
                printf("BankPack: Warning: Ignoring unknown option %s\n", argv[i]);
            }
        } else {
            // Add to list of object files to process
            files_add(argv[i]);
        }
    }

    return true;
}


static int matches_extension(char * filename, char * extension) {
    return (strcmp(filename + (strlen(filename) - strlen(extension)), extension) == 0);
}


static void init(void) {
    srand( time(0) );
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
        if ((option_get_platform() == PLATFORM_GB) && (option_get_mbc_type() == MBC_TYPE_NONE))
            printf("BankPack: ERROR: auto-banking does not work with nonbanked ROMS (no MBC for Game Boy)\n");
        else {
            // Extract areas, sort and assign them to banks
            // then rewrite object files as needed
            files_extract();
            files_rewrite();

            if (option_get_verbose())
                banks_show();
            if (option_get_cartsize())
                fprintf(stdout,"autocartsize:%d\n",option_banks_calc_cart_size());

            cleanup();
            ret = EXIT_SUCCESS;
        }
    }

    return ret; // Exit with failure by default
}
