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
#include "banks.h"
#include "map_file.h"

// Example data to parse from a .map file (excluding unwanted lines):
/*
_CODE                  00000200    00006A62 =       27234. bytes (REL,CON)
_CODE                  00000200    00006A62 =       27234. bytes (REL,CON)
_HOME                  00006C62    000000ED =         237. bytes (REL,CON)
_BASE                  00006D4F    000002A3 =         675. bytes (REL,CON)
_GSINIT                00006FF2    000001F9 =         505. bytes (REL,CON)
_GSINITTAIL            000071EB    00000001 =           1. bytes (REL,CON)
_DATA                  0000C0A0    00001684 =        5764. bytes (REL,CON)
_BSS                   0000D724    00000041 =          65. bytes (REL,CON)
_HEAP                  0000D765    00000000 =           0. bytes (REL,CON)
_HRAM10                00000000    00000001 =           1. bytes (ABS,CON)
*/

#define MAX_SPLIT_WORDS 6
#define RGBDS_BANK_SPLIT_WORDS 3
#define RGBDS_SECT_NAME_SPLIT_WORDS 3
#define RGBDS_SECT_INFO_SPLIT_WORDS 5
#define GBDK_AREA_SPLIT_WORDS 6
#define BANK_NUM_UNSET 0xFFFFFFFF



static int str_split(char * str_check, char * p_words[], const char * split_criteria) {

    char cols;
    char * p_str;

    cols = 0;
    p_str = strtok (str_check,split_criteria);
    while (p_str != NULL)
    {
        p_words[cols++] = p_str;
        p_str = strtok(NULL, split_criteria);
        if (cols >= MAX_SPLIT_WORDS) break;
    }

    return (cols);
}


static void add_area_gbdk(char * p_words[]) {

    area_item area;

    if ((strtol(p_words[2], NULL, 16) > 0) &&  // Exclude empty areas
        !(strstr(p_words[0], "SFR")) &&        // Exclude SFR areas (not actually located at addresses in area listing)
        !(strstr(p_words[0], "HRAM"))          // Exclude HRAM area
        )
    {
        snprintf(area.name, sizeof(area.name), "%s", p_words[0]); // [0] Area Name
        area.start = strtol(p_words[1], NULL, 16);         // [1] Area Hex Address Start
        area.end   = area.start + strtol(p_words[2], NULL, 16) - 1; // Start + [3] Hex Size - 1 = Area End
        if (strstr(area.name,"HEADER"))
            area.exclusive = false; // HEADER areas almost always overlap, ignore them
        else
            area.exclusive = option_all_areas_exclusive; // Default is false
        banks_check(area);
    }
}


static void add_area_rgbds(char * p_words[], int current_bank, const char * str_area_name) {

    area_item area;

    snprintf(area.name, sizeof(area.name), "%s", str_area_name);        // Area Name
    area.start = strtol(p_words[1], NULL, 16) | (current_bank << 16);   // [1] Area Hex Address Start
    area.end   = strtol(p_words[2], NULL, 16) | (current_bank << 16);   // [2] Area Hex Address End
    area.exclusive = option_all_areas_exclusive; // Default is false
    banks_check(area);
}


static int get_bank_num_rgbds(char * p_words[]) {

    return strtol(p_words[2], NULL, 10);
}


int map_file_process_areas(char * filename_in) {

    uint32_t cur_bank_rgbds;
    char * p_words[MAX_SPLIT_WORDS];
    char strline_in[MAX_STR_LEN] = "";
    FILE * map_file = fopen(filename_in, "r");

    set_option_input_source(OPT_INPUT_SRC_MAP);

    cur_bank_rgbds = BANK_NUM_UNSET;

    if (map_file) {

        // Read one line at a time into \0 terminated string
        while ( fgets(strline_in, sizeof(strline_in), map_file) != NULL) {

            // RGBDS Bank Numbers: Bank lines precede Section lines, use them to set bank num
            if (strstr(strline_in, " bank #")) {
                if (str_split(strline_in,p_words," #:\r\n") == RGBDS_BANK_SPLIT_WORDS)
                    cur_bank_rgbds = get_bank_num_rgbds(p_words);
            }

            // RGBDS Sections: Only parse lines that have Section (Area) summary info
            else if (strstr(strline_in, "  SECTION: ") || strstr(strline_in, "\tSECTION: ")) {
                if (cur_bank_rgbds != BANK_NUM_UNSET) {
                    // Try to strip quote bracketed name out first
                    int name_split_count = str_split(strline_in, p_words,"\"");
                    if (name_split_count > 0) {
                        // Save section name from array before splitting again (if not blank)
                        const char * str_area_name = (name_split_count == RGBDS_SECT_NAME_SPLIT_WORDS) ? p_words[1] : "";
                        // Then split up the remaining section info from first string in split array
                        if (str_split(p_words[0], p_words," :$()[]\n\t\"") == RGBDS_SECT_INFO_SPLIT_WORDS)
                            add_area_rgbds(p_words, cur_bank_rgbds, str_area_name);
                    }
                }
            }

            // GBDK Areas: Only parse lines that start with '_' character (Area summary lines)
            else if (strline_in[0] == '_') {
                if (str_split(strline_in, p_words, " =.") == GBDK_AREA_SPLIT_WORDS)
                    add_area_gbdk(p_words);
            }

        } // end: while still lines to process

        fclose(map_file);

    } // end: if valid file
    else return (false);

   return true;
}

