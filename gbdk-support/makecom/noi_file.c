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
#include "noi_file.h"
#include "bin_to_com.h"

// Example data to parse from a .noi file:
/*
DEF s__CODE 0x200
DEF l__BASE 0x2A3
DEF __shadow_OAM_OFF 0x455
DEF .mode 0x456
*/

const char * known_section_names[] =
        {"_CODE","_HOME","_BASE","_CODE_0","_INITIALIZER","_LIT","_GSINIT","_GSFINAL"};


list_type symbol_list;
uint32_t overlay_count_addr = SYM_VAL_UNSET;
uint32_t overlay_name_addr = SYM_VAL_UNSET;


// Initialize the symbol list
void noi_init(void) {

    list_init(&symbol_list, sizeof(symbol_item));
}


// Free the symbol list
void noi_cleanup(void) {

    list_cleanup(&symbol_list);
}


// Find a matching symbol, if none matches a new one is added and returned
static int symbollist_get_id_by_name(char * symbol_name) {

    symbol_item * symbols = (symbol_item *)symbol_list.p_array;

    // Check for matching symbol name
    for(int c = 0;c < symbol_list.count; c++) {
        // Return matching symbol index if present
        if (strcmp(symbol_name, symbols[c].name) == 0) {
            return c;
        }
    }

    // no match was found, add symbol
    symbol_item new_symbol = {.name = "", .addr_start = SYM_VAL_UNSET, .length = SYM_VAL_UNSET, .bank_num = 0x00, .src_rom_addr = SYM_VAL_UNSET};
    snprintf(new_symbol.name, sizeof(new_symbol.name), "%s", symbol_name);
    list_additem(&symbol_list, &new_symbol);

    return (symbol_list.count - 1);
}


// Add the start address or length for a s_ or l_ symbol record
static void noi_symbollist_add(char rec_type, char * name, char * value) {

    // Only add symbols if they're in the known section name list
    for (int c = 0; c < ARRAY_LEN(known_section_names); c++) {

        if ((strcmp(name, known_section_names[c]) == 0) ||
            (strncmp(name, "_CODE_", (sizeof("_CODE_") - 1)) == 0)) {

            symbol_item * symbols = (symbol_item *)symbol_list.p_array;

            // Check to see if there is an existing record to update
            // If one isn't found a new record will have been created automatically
            int symbol_id = symbollist_get_id_by_name(name);
            if (symbol_id != ERR_NO_SYMBOLS_LEFT) {

                // Handle whether it's a start-of-address or a length record for the given symbol
                if (rec_type == NOI_REC_START) {
                    symbols[symbol_id].addr_start   = strtol(value, NULL, 16);
                    symbols[symbol_id].bank_num     = BANK_GET_NUM(symbols[symbol_id].addr_start);
                    symbols[symbol_id].src_rom_addr = BANK_GET_ROM_ADDR(symbols[symbol_id].addr_start);
                }
                else if (rec_type == NOI_REC_LENGTH) {
                    symbols[symbol_id].length = strtol(value, NULL, 16);
                }
            }

            return;
        }
    }
}


// Load symbols from a .noi file and create paired records for start & length entries
// Plus, look for and store for overlay symbol information
int noi_file_load_symbols(char * filename_in) {

    char cols;
    char * p_str;
    char * p_words[MAX_SPLIT_WORDS];
    char strline_in[MAX_STR_LEN] = "";
    FILE * noi_file = fopen(filename_in, "r");
    symbol_item symbol;
    int symbol_id;

    noi_init();

    if (noi_file) {

        // Read one line at a time into \0 terminated string
        while ( fgets(strline_in, sizeof(strline_in), noi_file) != NULL) {

            // Require minimum length to match
            if (strlen(strline_in) >= NOI_REC_START_LEN) {

                // Split string into words separated by spaces
                cols = 0;
                p_str = strtok(strline_in," ");
                while (p_str != NULL)
                {
                    p_words[cols++] = p_str;
                    // Only split on underscore for the second match
                    p_str = strtok(NULL, " ");
                    if (cols >= MAX_SPLIT_WORDS) break;
                }

                // [0] = "DEF"
                // [1] = symbol name
                // [2] = symbol value
                if (cols >= NOI_REC_COUNT_MATCH) {

                    // If it matches either _s_egment or _l_ength records (first two chars)
                    // then add a record for it with the first two chars truncated
                    if ((strncmp(p_words[1], "l_", NOI_REC_S_L_CHK) == 0) ||
                        (strncmp(p_words[1], "s_", NOI_REC_S_L_CHK) == 0)) {
                        noi_symbollist_add(p_words[1][0], p_words[1] + NOI_REC_S_L_CHK, p_words[2]);
                    }
                    // Otherwise check for records of interest and store them
                    else if (strcmp(p_words[1], "___overlay_count") == 0) {
                        overlay_count_addr = strtol(p_words[2], NULL, 16);
                    }
                    else if (strcmp(p_words[1], "___overlay_name") == 0) {
                        overlay_name_addr = strtol(p_words[2], NULL, 16);
                    }
                }
            } // end: valid min chars to process line

        } // end: while still lines to process

        fclose(noi_file);

    } // end: if valid file
    else return (false);

   return true;
}
