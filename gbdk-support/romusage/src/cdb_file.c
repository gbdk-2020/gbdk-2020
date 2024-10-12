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
#include "list.h"
#include "banks.h"
#include "cdb_file.h"


list_type symbol_list;

#define CDB_L_REC_FUNC_START_GLOBAL 'G'
#define CDB_L_REC_FUNC_START_LOCAL  'F'
#define CDB_L_REC_FUNC_END_GLOBAL   "XG"
#define CDB_L_REC_FUNC_END_LOCAL    "XF"

// More compelte CDB coverage is affected by this SDCC bug
// #3662 Adding a function removes const arrays above it from .adb/.cdb output
// https://sourceforge.net/p/sdcc/bugs/3662/


// Example data to parse from a .cdb file:
//
// 0 1 2---------- 3-- 4 5----
// L:G$big_const_4$0_0$0:24039 <-- bank 2
// L:G$big_const_3$0_0$0:1784E <-- Bank 1
//
// S:Fsrcfile_2$func_2_local_scope$0_0$0({2}DF,SV:S),C,0,0
// L:Fsrcfile_2$func_2_local_scope$0$0:14000
// L:XFsrcfile_2$func_2_local_scope$0$0:14003
// F:Fsrcfile_2$func_2_local_scope$0_0$0({2}DF,SV:S),C,0,0,0,0,0
//
// 0 1 2---------- 3-- 4  5-- 6----- 7  8  9 10 11
// S:G$big_const_3$0_0$0({256}DA256d,SC:U),D,0,0     <--- size 256 bytes (SC=signed char)
// S:G$big_const_4$0_0$0({15360}DA15360d,SC:U),D,0,0 <--- size 15360 bytes (SC=signed char)
//
// 0 1--------- 2------------ 3-- 4  5---
// L:Fsrcfile_2$local_const_2$0_0$0:14027
// 0 1--------- 2------------ 3-- 4  5---- 6------- 7  8  9 10 11
// S:Fsrcfile_2$local_const_2$0_0$0({14336}DA14336d,SC:U),D,0,0 <--- size 14336 bytes

// 4.8 Link Address of Symbol
//
// L    Link record type indicator
// G   Symbol has file scope
// F <Filename>    Symbol has file scope.
// L <Function>    Symbol has function scope
//
// functions use L:XG to mark their last line
//   basxto:    L:XG$init_map$0$0:BCA - L:G$init_map$0$0:AFB = should be the length
//     L:G$board_handle_new_piece$0$0:6284
//     L:XG$board_handle_new_piece$0$0:62DD
//
// https://github.com/roybaer/sdcc-wiki/wiki/CDB-File-Format
//
// Address Space field (S: record)
// A   External stack
// B   Internal stack
// C   Code
// D   Code / static segment
// E   Internal ram (lower 128) bytes
// F   External ram
// G   Internal ram
// H   Bit addressable
// I   SFR space
// J   SBIT space
// R   Register space
// Z   Used for function records, or any undefined space code
//
// Types in a Section 4.4 Type Chain Record (S: record)
// DA <n>  Array of n elements
// DF  Function
// DG  Generic pointer
// DC  Code pointer
// DX  External ram pointer
// DD  Internal ram pointer
// DP  Paged pointer
// DI  Upper 128 byte pointer
// SL  long
// SI  int
// SC  char
// SS  short
// SV  void
// SF  float
// ST <name>   Structure of name <name>
// SX  sbit
// SB <n>  Bit field of <n> bits


// Initialize the symbol list
void cdb_init(void) {

    list_init(&symbol_list, sizeof(area_item));
}


// Free the symbol list
void cdb_cleanup(void) {

    list_cleanup(&symbol_list);
}


// Find a matching symbol, if none matches a new one is added and returned
static int symbollist_get_id_by_name(char * symbol_name) {

    area_item * symbols = (area_item *)symbol_list.p_array;
    area_item new_symbol;

    int c;

    // Check for matching symbol name
    for(c=0;c < symbol_list.count; c++) {
        // Return matching symbol index if present
        if (strncmp(symbol_name, symbols[c].name, AREA_MAX_STR) == 0) {
            return c;
        }
    }

    snprintf(new_symbol.name, sizeof(new_symbol.name), "%s", symbol_name);
    new_symbol.start  = AREA_VAL_UNSET;
    new_symbol.end    = AREA_VAL_UNSET;
    new_symbol.length = AREA_VAL_UNSET;
    if (strstr(symbol_name,"HEADER"))
        new_symbol.exclusive = false; // HEADER symbols almost always overlap, ignore them
    else
        new_symbol.exclusive = option_all_areas_exclusive; // Default is false

    list_additem(&symbol_list, &new_symbol);

    return (symbol_list.count - 1);
}


// Process list of symbols and add them to banks
static void cdb_symbollist_add_all_to_banks() {

    area_item * symbols = (area_item *)symbol_list.p_array;
    int c;

    // Only process completed symbols (start and length both set)
    for(c=0;c < symbol_list.count; c++) {

        // Functions need length calculated from start and end
        if ((symbols[c].length == AREA_VAL_UNSET) &&
            (symbols[c].start != AREA_VAL_UNSET) &&
            (symbols[c].end != AREA_VAL_UNSET)) {
            symbols[c].length = symbols[c].end - symbols[c].start + 1;
        }


        if ((symbols[c].start != AREA_VAL_UNSET) &&
            (symbols[c].length != AREA_VAL_UNSET)) {
            symbols[c].end = symbols[c].start + symbols[c].length - 1;
            banks_check(symbols[c]);
        }
    }
}


// Adds start/end address from a Linker Record
// Requires either separate calls for Start and End, or one call for
// start and a separate cdb_add_record_symbol() call to set length
static void cdb_add_record_linker(char * type, char * name, char * address) {

    area_item * symbols = (area_item *)symbol_list.p_array;

    // Retrieve existing symbol or create a new one
    int symbol_id = symbollist_get_id_by_name(name);

    if (symbol_id != ERR_NO_AREAS_LEFT) {

        // Check Linker record for start-address or end-address
        // Bank number is in the address bits, no need to modify it
        if ((strncmp(type, CDB_L_REC_FUNC_END_GLOBAL, 4) == 0) ||
            (strncmp(type, CDB_L_REC_FUNC_END_LOCAL, 4) == 0)) {
            symbols[symbol_id].end = strtol(address, NULL, 16);     // End address
        }
        else if ((type[0] == CDB_L_REC_FUNC_START_GLOBAL) ||
                 (type[0] == CDB_L_REC_FUNC_START_LOCAL)) {
            symbols[symbol_id].start = strtol(address, NULL, 16);   // Start address
        }
        // else
        // printf("Rejected L record %s, %s, %s\n", type, name, address);
    }
}


// Adds length from a symbol record
// To get a complete entry requires a start address call to cdb_add_record_linker()
static void cdb_add_record_symbol(char * addr_space, char * name, char * length, char * dcl_type) {

    area_item * symbols = (area_item *)symbol_list.p_array;

    // Only allow certain address spaces
    if ((addr_space[0] == 'C') || // Address Space: Code
        (addr_space[0] == 'D') || // Address Space: Code / static segment
        (addr_space[0] == 'E') || // Address Space: Internal RAM (lower 128) bytes
        (addr_space[0] == 'F') || // Address Space: External RAM
        (addr_space[0] == 'G')) { // Address Space: Internal RAM

        // Exclude zero length entries
        // Don't let function ~entry points override function bodies(<DCLType> = DF, function which are two bytes in size)
        if ((strtol(length, NULL, 10) > 0) &&
            (!strstr(dcl_type, "DF")))
        {
            // Retrieve existing symbol or create a new one
            int symbol_id = symbollist_get_id_by_name(name); // [2] Area Name
            if (symbol_id != ERR_NO_AREAS_LEFT) {
                    symbols[symbol_id].length = strtol(length, NULL, 10); // [5] Symbol decimal length
            }
        }
    }
    // else
    // printf("Rejected S record %s, %s, %s, %s\n", addr_space, name, length, dcl_type);
}


int cdb_file_process_symbols(char * filename_in) {

    char cols;
    char * p_str;
    char * p_words[CDB_MAX_SPLIT_WORDS];
    char strline_in[CDB_MAX_STR_LEN] = "";
    FILE * cdb_file = fopen(filename_in, "r");
    area_item symbol;
    int symbol_id;

    // CDB defaults to showing areas
    banks_output_show_areas(true);

    // CDB defaults to size descending, but don't override explicit options
    if (get_option_area_sort() == OPT_AREA_SORT_DEFAULT)
        set_option_area_sort(OPT_AREA_SORT_SIZE_DESC);

    set_option_input_source(OPT_INPUT_SRC_CDB);

    if (cdb_file) {

        // Read one line at a time into \0 terminated string
        while ( fgets(strline_in, sizeof(strline_in), cdb_file) != NULL) {

            // Require minimum length to match
            if (strlen(strline_in) >= CDB_REC_START_LEN) {

                // Match either _S_egment or _L_ength records
                if ( (strncmp(strline_in, "L:", CDB_REC_START_LEN) == 0) ||
                     (strncmp(strline_in, "S:", CDB_REC_START_LEN) == 0)) {

                    // Split string into words separated by spaces
                    cols = 0;
                    p_str = strtok(strline_in,":$({}),");
                    while (p_str != NULL)
                    {
                        p_words[cols++] = p_str;
                        p_str = strtok(NULL, ":$({}),");
                        if (cols >= CDB_MAX_SPLIT_WORDS) break;
                    }

                    // Linker record (start or end address)
                    if ((p_words[0][0] == CDB_REC_L) &&
                        (cols == CDB_REC_L_COUNT_MATCH)) {
                        // [1] Start/End, [2] Area Name1, [5] Address
                        cdb_add_record_linker(p_words[1], p_words[2], p_words[5]);
                    }
                    // Symbol record (length)
                    if ((p_words[0][0] == CDB_REC_S) &&
                        (cols == CDB_REC_S_COUNT_MATCH)) {
                        // [9] address space, [2] Area Name, [5] Symbol decimal length, [6] DCLType
                        cdb_add_record_symbol(p_words[9], p_words[2], p_words[5], p_words[6]);
                    }

                } // end: if valid start of line
            } // end: valid min chars to process line
        } // end: while still lines to process

        fclose(cdb_file);

        // Process all the symbols
        cdb_symbollist_add_all_to_banks();

    } // end: if valid file
    else {
        log_error("Error: Failed to open input file %s\n", filename_in);
        return false;
    }

   return true;
}
