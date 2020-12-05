
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
#include "list.h"
#include "files.h"
#include "obj_data.h"


static bool bank_check_mbc1_ok(uint32_t bank_num);
static void bank_update_assigned_minmax(uint16_t bank_num);
static void bank_add_area(bank_item * p_bank, uint16_t bank_num, area_item * p_area);
static void bank_check_area_size(area_item * p_area);
static void banks_assign_area(area_item * p_area);
static int area_item_compare(const void* a, const void* b);
static void areas_sort(void);
static bool symbol_banked_check_rewrite_ok(char *, uint32_t);

extern bool g_option_verbose;


list_type banklist;
list_type arealist;
list_type symbollist;
list_type symbol_matchlist;

uint16_t bank_limit_rom_min = BANK_NUM_ROM_MIN;
uint16_t bank_limit_rom_max = BANK_NUM_ROM_MAX;

uint16_t bank_assign_rom_min = BANK_NUM_ROM_MAX;
uint16_t bank_assign_rom_max = 0;

int g_mbc_type = MBC_TYPE_DEFAULT;


int banks_get_mbc_type(void) {
    return g_mbc_type;
}


// Set MBC type by interpreting from byte 149
//
//  For lcc linker option: -Wl-ytN where N is one of the numbers below
//  (from makebin.c in SDCC)
//
//  ROM Byte 0147: Cartridge type:
//  0-ROM ONLY            12-ROM+MBC3+RAM
//  1-ROM+MBC1            13-ROM+MBC3+RAM+BATT
//  2-ROM+MBC1+RAM        19-ROM+MBC5
//  3-ROM+MBC1+RAM+BATT   1A-ROM+MBC5+RAM
//  5-ROM+MBC2            1B-ROM+MBC5+RAM+BATT
//  6-ROM+MBC2+BATTERY    1C-ROM+MBC5+RUMBLE
//  8-ROM+RAM             1D-ROM+MBC5+RUMBLE+SRAM
//  9-ROM+RAM+BATTERY     1E-ROM+MBC5+RUMBLE+SRAM+BATT
//  B-ROM+MMM01           1F-Pocket Camera
//  C-ROM+MMM01+SRAM      FD-Bandai TAMA5
//  D-ROM+MMM01+SRAM+BATT FE - Hudson HuC-3
//  F-ROM+MBC3+TIMER+BATT FF - Hudson HuC-1
//  10-ROM+MBC3+TIMER+RAM+BATT
//  11-ROM+MBC3
void banks_set_mbc_by_rom_byte_149(int mbc_type_rom_byte) {

    switch (mbc_type_rom_byte) {
        // NO MBC
        case 0x00U: //  0-ROM ONLY
        case 0x08U: //  2-ROM+MBC1+RAM
        case 0x09U: //  8-ROM+RAM
        case 0x0BU: //  B-ROM+MMM01
        case 0x0CU: //  C-ROM+MMM01+SRAM
        case 0x0DU: //  D-ROM+MMM01+SRAM+BATT
            banks_set_mbc(MBC_TYPE_NONE);
            break;

        // MBC 1
        case 0x01U: //  1-ROM+MBC1
        case 0x02U: //  2-ROM+MBC1+RAM
        case 0x03U: //  3-ROM+MBC1+RAM+BATT
            banks_set_mbc(MBC_TYPE_MBC1);
            break;

        // MBC 2
        case 0x05U: //  5-ROM+MBC2
        case 0x06U: //  6-ROM+MBC2+BATTERY
            banks_set_mbc(MBC_TYPE_MBC2);
            break;

        // MBC 3
        case 0x0FU: //  F-ROM+MBC3+TIMER+BATT
        case 0x10U: //  10-ROM+MBC3+TIMER+RAM+BATT
        case 0x11U: //  11-ROM+MBC3
        case 0x12U: //  12-ROM+MBC3+RAM
        case 0x13U: //  13-ROM+MBC3+RAM+BATT
            banks_set_mbc(MBC_TYPE_MBC3);
            break;

        // MBC 5
        case 0x19U: //  19-ROM+MBC5
        case 0x1AU: //  1A-ROM+MBC5+RAM
        case 0x1BU: //  1B-ROM+MBC5+RAM+BATT
        case 0x1CU: //  1C-ROM+MBC5+RUMBLE
        case 0x1DU: //  1D-ROM+MBC5+RUMBLE+SRAM
        case 0x1EU: //  1E-ROM+MBC5+RUMBLE+SRAM+BATT
            banks_set_mbc(MBC_TYPE_MBC5);
            break;

        default:
            printf("BankPack: Warning: unrecognized MBC option -yt=%x\n", mbc_type_rom_byte);
            break;
    }
}


// Set MBC type directly
void banks_set_mbc(int mbc_type) {
    switch (mbc_type) {
        case MBC_TYPE_NONE:
            g_mbc_type = MBC_TYPE_NONE;
            break;
        case MBC_TYPE_MBC1:
            g_mbc_type = mbc_type;
            bank_limit_rom_max = BANK_NUM_ROM_MAX_MBC1;
            break;
        case MBC_TYPE_MBC2:
            g_mbc_type = mbc_type;
            bank_limit_rom_max = BANK_NUM_ROM_MAX_MBC2;
            break;
        case MBC_TYPE_MBC3:
            g_mbc_type = mbc_type;
            bank_limit_rom_max = BANK_NUM_ROM_MAX_MBC3;
            break;
        case MBC_TYPE_MBC5:
            g_mbc_type = mbc_type;
            bank_limit_rom_max = BANK_NUM_ROM_MAX_MBC5;
            break;
        default:
            printf("BankPack: Warning: unrecognized MBC option -mbc%d!\n", mbc_type);
            break;
    }
}


bool banks_set_min(uint16_t bank_num) {
    if (bank_num < BANK_NUM_ROM_MIN)
        return false;
    else bank_limit_rom_min = bank_num;

    return true;
}

bool banks_set_max(uint16_t bank_num) {
    if (bank_num > BANK_NUM_ROM_MAX)
        return false;
    else bank_limit_rom_max = bank_num;

    return true;
}


void obj_data_init(void) {
    int c;
    bank_item newbank;

    list_init(&banklist,      sizeof(bank_item));
    list_init(&arealist,      sizeof(area_item));
    list_init(&symbollist,    sizeof(symbol_item));
    list_init(&symbol_matchlist, sizeof(symbol_match_item));

    // Pre-populate bank list with max number of of banks
    // to allow handling fixed-bank (non-autobank) areas
    newbank.size = newbank.free = BANK_SIZE_ROM;
    for (c=0; c < BANK_ROM_TOTAL; c++)
        list_additem(&banklist, &newbank);

    // Add default symbol match and replace
    symbol_match_add("___bank_");
}


void obj_data_cleanup(void) {
    list_cleanup(&banklist);
    list_cleanup(&arealist);
    list_cleanup(&symbollist);
    list_cleanup(&symbol_matchlist);
}


// Add a symbol to the match and replace list
void symbol_match_add(char * symbol_str) {

    symbol_match_item newmatch;

    if (snprintf(newmatch.name, sizeof(newmatch.name), "%s", symbol_str) > sizeof(newmatch.name))
        printf("BankPack: Warning: truncated symbol match string to:%s\n",newmatch.name);

    list_additem(&symbol_matchlist, &newmatch);
}



// Add an area into the pool of areas to assign (if it's banked CODE)
int areas_add(char * area_str, uint32_t file_id) {

    area_item newarea;

    // Only match areas which are banked ("_CODE_" vs "_CODE")
     if (AREA_LINE_RECORDS == sscanf(area_str,"A _CODE_%3d size %4x flags %*4x addr %*4x",
                                    &newarea.bank_num_in, &newarea.size)) {

        // Only process areas with (size > 0)
        if (newarea.size > 0) {
            sprintf(newarea.name, "_CODE_"); // Hardwired to _CODE_ for now
            newarea.file_id = file_id;
            newarea.bank_num_out = BANK_NUM_UNASSIGNED;
            list_additem(&arealist, &newarea);
            return true;
        }
    }
    return false;
}


// Add an area into the pool of areas to assign (if it's banked CODE)
int symbols_add(char * symbol_str, uint32_t file_id) {

    symbol_item newsymbol;

     if (SYMBOL_LINE_RECORDS == sscanf(symbol_str,"S %" TOSTR(OBJ_NAME_MAX_STR_LEN) "s Def00%4x",
                                       newsymbol.name, &newsymbol.bank_num_in)) {

        // Symbols that start with b_ store the bank num for the matching symbol without 'b'
        newsymbol.is_banked_def          = (newsymbol.name[0] == 'b');
        newsymbol.file_id                = file_id;
        newsymbol.found_matching_symbol  = false;

        // Don't add banked symbols if they're not set to the autobank bank #
        if ((newsymbol.is_banked_def) && (newsymbol.bank_num_in != BANK_NUM_AUTO))
            return false;

        list_additem(&symbollist, &newsymbol);
        return true;
    }
    return false;
}


// Track Min/Max assigned banks  used
static void bank_update_assigned_minmax(uint16_t bank_num) {
    if (bank_num > bank_assign_rom_max)
        bank_assign_rom_max = bank_num;

    if (bank_num < bank_assign_rom_min)
        bank_assign_rom_min = bank_num;
}


// Add an area to a bank.
// It should only error out when there isn't enough
// room with a fixed-bank area (non-autobank)
static void bank_add_area(bank_item * p_bank, uint16_t bank_num, area_item * p_area) {
    // Make sure there is room, then update free space and assign outbound bank number
    if (p_area->size <= p_bank->free) {
            p_bank->free -= p_area->size;
            p_area->bank_num_out = bank_num;
            bank_update_assigned_minmax(bank_num);
    } else {
        printf("BankPack: ERROR! Area %s, bank %d, size %d is too large for assigned bank %d (free %d)\n",
                p_area->name, p_area->bank_num_in, p_area->size, bank_num, p_bank->free);
        exit(EXIT_FAILURE);
    }
}


// Check to see if the specified bank is allowed with MBC1
static bool bank_check_mbc1_ok(uint32_t bank_num) {

    if ((bank_num == 0x20U) || (bank_num == 0x40U) || (bank_num == 0x60U))
        return false;
    else
        return true;
}


// Checks to make sure area size is not larger than an entire bank
static void bank_check_area_size(area_item * p_area) {
    if (p_area->size > BANK_SIZE_ROM) {
        printf("BankPack: ERROR! Area %s, bank %d, size %d is too large for bank size %d (file %s)\n",
                p_area->name, p_area->bank_num_in, p_area->size, BANK_SIZE_ROM, file_get_name_by_id(p_area->file_id));
        exit(EXIT_FAILURE);
    }
}


// Find an bank for a given area using First Fit Decreasing (FFD)
// All possible banks (0-255) were pre-created and initialized [in obj_data_init()],
// so there is no need to add when using a fresh bank
static void banks_assign_area(area_item * p_area) {

    uint16_t c;
    bank_item * banks = (bank_item *)banklist.p_array;

    bank_check_area_size(p_area);

    // Try to assign fixed bank areas to their expected bank.
    // (ignore the area if it's outside the processing range)
    if (p_area->bank_num_in != BANK_NUM_AUTO) {
        if ((p_area->bank_num_in >= bank_limit_rom_min) &&
            (p_area->bank_num_in <= bank_limit_rom_max)) {

            if ((g_mbc_type == MBC_TYPE_MBC1) && (!bank_check_mbc1_ok(p_area->bank_num_in)))
                printf("BankPack: Warning: Area in fixed bank assigned to MBC1 excluded bank:%d\n", p_area->bank_num_in);

            bank_add_area(&banks[p_area->bank_num_in], p_area->bank_num_in, p_area);
        }
        return;
    }
    else if (p_area->bank_num_in == BANK_NUM_AUTO) {
        // Try to assign area to first allowed bank with enough free space
        // Bank array index maps directly to bank numbers, so [2] will be _CODE_2
        for (c = bank_limit_rom_min; c <= bank_limit_rom_max ; c++) {
            // Check for MBC bank restrictions
            if ((g_mbc_type != MBC_TYPE_MBC1) || (bank_check_mbc1_ok(c))) {
                // If there is enough space in current bank, assign it and reduce free space
                if (p_area->size <= banks[c].free) {
                    bank_add_area(&banks[c], c, p_area);
                    return;
                }
            }
        }
    }

    if (g_option_verbose)
        banks_show();
    printf("BankPack: ERROR! Failed to assign bank for Area %s, bank %d, size %d. Out of banks!\n",
            p_area->name, p_area->bank_num_in, p_area->size);
    exit(EXIT_FAILURE);
}


// qsort compare rule function for sorting areas
static int area_item_compare(const void* a, const void* b) {

    // sort by bank [asc] (fixed vs auto-bank), then by size [desc]
    if (((area_item *)a)->bank_num_in != ((area_item *)b)->bank_num_in)
        return  (((area_item *)a)->bank_num_in > ((area_item *)b)->bank_num_in);
    else
        return (((area_item *)a)->size < ((area_item *)b)->size);
}

static void areas_sort(void) {
    // Sort banks by name
    qsort (arealist.p_array, arealist.count, sizeof(area_item), area_item_compare);
}


// Assigns all areas -> banks, and bank numbers -> files
// Fixed bank areas are placed first, then auto-banks fill the rest in
// Only call after all areas have been collected from object files
int obj_data_process(list_type * p_filelist) {
    uint32_t c, s;
    area_item   * areas   = (area_item *)arealist.p_array;
    symbol_item * symbols = (symbol_item *)symbollist.p_array;
    file_item   * files   = (file_item *)(p_filelist->p_array);

    areas_sort();

    // Assign areas to banks
    for (c = 0; c < arealist.count; c++) {
        banks_assign_area(&(areas[c]));

        // If areas was auto-banked then set bank number in associated file
        if ((areas[c].bank_num_in == BANK_NUM_AUTO) &&
            (areas[c].bank_num_out != BANK_NUM_UNASSIGNED)) {

            if (files[ areas[c].file_id ].bank_num != BANK_NUM_UNASSIGNED) {
                printf("BankPack: ERROR! Can't assign bank number to a file more than once! Already %d, new is %d\n",
                        files[ areas[c].file_id ].bank_num, areas[c].bank_num_out);
                exit(EXIT_FAILURE);
            }

            files[ areas[c].file_id ].bank_num = areas[c].bank_num_out;
            files[ areas[c].file_id ].rewrite_needed = true;
        }
    }

    // Check all symbols for matches to banked entries, flag if match found
    // TODO: ineffecient to loop over symbols for all files
    for (c = 0; c < symbollist.count; c++) {
        if (symbols[c].is_banked_def) {
            for (s = 0; s < symbollist.count; s++) {
                if (symbols[c].file_id == symbols[s].file_id) {
                    // offset +1 bast the "b" char at start of banekd symbol entry name
                    if (strcmp(symbols[c].name + 1, symbols[s].name) == 0) {
                        symbols[c].found_matching_symbol = true;
                        break;
                    } else if (s == symbollist.count -1)
                        printf("  -> NO MATCH FOUND%s\n", symbols[c].name);
                }
            }
        }
    }

}


// Display file/area/bank assignment
// Should be called after obj_data_process()
int banks_show(void) {

    uint32_t c;
    uint32_t a;

    printf("\n=== Banks assigned: %d -> %d (allowed range %d -> %d) ===\n",
            bank_assign_rom_min, bank_assign_rom_max,
            bank_limit_rom_min,  bank_limit_rom_max);

    bank_item * banks = (bank_item *)banklist.p_array;
    area_item * areas = (area_item *)arealist.p_array;
    for (c = 0; c < banklist.count; c++) {
        if (banks[c].free != BANK_SIZE_ROM) {
            printf("Bank %d: size=%5d, free=%5d\n", c, banks[c].size, banks[c].free);
            for (a = 0; a < arealist.count; a++) {
                if (areas[a].bank_num_out == c) {
                    printf(" +- Area: name=%8s, size=%5d, bank_in=%3d, bank_out=%3d, file=%s\n",
                        areas[a].name,
                        areas[a].size,
                        areas[a].bank_num_in,
                        areas[a].bank_num_out,
                        file_get_name_by_id(areas[a].file_id));
                }
            }
        }
    }

    printf("\n");
}


// Accepts an input string line and writes it
// out with an **updated bank num** to a file
// * Adds trailing \n if missing
bool area_modify_and_write_to_file(char * strline_in, FILE * out_file, uint16_t bank_num) {

    // Only rewrite area bank number for unset banked CODE
    if (strline_in[0] == 'A') {
        // For lines: A _CODE_255 ...
        if (strstr(strline_in, "A _CODE_255")) {
            fprintf(out_file, "A _CODE_%d %s", bank_num, strstr(strline_in, "size"));
            // Add trailing \n if missing (may be, due to using strtok() to split the string)
            if (strline_in[(strlen(strline_in)-1)] != '\n')
                fprintf(out_file, "\n");
            return true;
        }
    }
    return false;
}


// Check to make sure a banked symbol has a matching symbol in the same file
// This prevents mistakenly rewriting symbol b_<something> entries from asm files
bool symbol_banked_check_rewrite_ok(char * symbol_name, uint32_t file_id) {

    uint32_t c;
    symbol_item * symbols = (symbol_item *)symbollist.p_array;

    for (c = 0; c < symbollist.count; c++) {
        if ((symbols[c].file_id == file_id) &&
            (symbols[c].is_banked_def) &&
            (symbols[c].found_matching_symbol)) {
            // Make sure b_<name> matches
            if (strcmp(symbols[c].name + 2, symbol_name) == 0)
                return true;
        }
    }
    return false;
}


// Accepts an input string line
// and writes it out with an updated bank to a file
// * Adds trailing \n if missing
bool symbol_modify_and_write_to_file(char * strline_in, FILE * out_file, uint16_t bank_num, uint32_t file_id) {

    uint32_t c;
    char strmatch[OBJ_NAME_MAX_STR_LEN];
    char symbol_name[OBJ_NAME_MAX_STR_LEN];
    symbol_match_item * sym_match = (symbol_match_item *)symbol_matchlist.p_array;

    // Only rewrite banked symbol entries (include end of line chars)
    if (strline_in[0] == 'S') {
        // For lines: S b_<symbol name>... Def0000FF
        if (sscanf(strline_in,"S b_%" TOSTR(OBJ_NAME_MAX_STR_LEN) "s Def0000FF", symbol_name)) {
            if (symbol_banked_check_rewrite_ok(symbol_name, file_id))  {
                fprintf(out_file, "S b_%s Def0000%02x", symbol_name, bank_num);
                if (strline_in[(strlen(strline_in)-1)] != '\n')
                    fprintf(out_file, "\n");
                return true;
            }
        } // For lines: S ___bank_<trailing symbol name>... Def0000FF
        else {
            for (c = 0; c < symbol_matchlist.count; c++) {
                if (snprintf(strmatch, sizeof(strmatch), "S %s%%" TOSTR(OBJ_NAME_MAX_STR_LEN) "s Def0000FF", sym_match[c].name) > sizeof(strmatch))
                    printf("BankPack: Warning: truncated symbol match string to:%s\n",strmatch);

                if (sscanf(strline_in, strmatch, symbol_name)) {
                    fprintf(out_file, "S %s%s Def0000%02x", sym_match[c].name, symbol_name, bank_num);
                    if (strline_in[(strlen(strline_in)-1)] != '\n')
                        fprintf(out_file, "\n");
                    return true;
                }
            }
        }
    }
    return false;
}
