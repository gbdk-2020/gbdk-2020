
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
#include "options.h"
#include "obj_data.h"


static bool bank_check_mbc1_ok(uint32_t bank_num);
static void bank_update_assigned_minmax(uint16_t bank_num);
static void bank_update_all_max(uint16_t bank_num);
static void bank_add_area(bank_item * p_bank, uint16_t bank_num, area_item * p_area);
static void bank_check_area_size(area_item * p_area);
static void banks_assign_area(area_item * p_area);
static int area_item_compare(const void* a, const void* b);
static void areas_sort(void);
static bool symbol_banked_check_rewrite_ok(char *, uint32_t);



list_type banklist;
list_type arealist;
list_type symbollist;
list_type symbol_matchlist;

uint16_t bank_limit_rom_min = BANK_NUM_ROM_DEFAULT;
uint16_t bank_limit_rom_max = BANK_NUM_ROM_MAX;


uint16_t bank_assigned_rom_min = BANK_NUM_ROM_MAX;
uint16_t bank_assigned_rom_max = 0;
uint16_t bank_assigned_rom_max_alltypes = 0;


void obj_data_init(void) {
    int c;
    bank_item newbank;

    list_init(&banklist,      sizeof(bank_item));
    list_init(&arealist,      sizeof(area_item));
    list_init(&symbollist,    sizeof(symbol_item));
    list_init(&symbol_matchlist, sizeof(symbol_match_item));

    // Pre-populate bank list with max number of banks
    // to allow handling fixed-bank (non-autobank) areas
    newbank.size = newbank.free = BANK_SIZE_ROM;
    newbank.reserved = 0;
    newbank.type = BANK_TYPE_UNSET;
    newbank.item_count = 0;
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

    // Only match areas which are banked ("_CODE_" vs "_CODE") and ("_LIT_")
    if (AREA_LINE_RECORDS == sscanf(area_str,"A _CODE _%3d size %4x flags %*4x addr %*4x",
                                    &newarea.bank_num_in, &newarea.size)) {
        newarea.type = BANK_TYPE_CODE;
    }
    else if (AREA_LINE_RECORDS == sscanf(area_str,"A _LIT_%3d size %4x flags %*4x addr %*4x",
                                        &newarea.bank_num_in, &newarea.size)) {
        newarea.type = BANK_TYPE_LIT_EXCLUSIVE;
    }
    else
        return false;

    // Only process areas with (size > 0)
    if (newarea.size > 0) {
        if (newarea.type == BANK_TYPE_LIT_EXCLUSIVE)
            sprintf(newarea.name, "_LIT_");  // Hardwired to _LIT_ for now
        else
            sprintf(newarea.name, "_CODE_"); // Hardwired to _CODE_ for now
        newarea.file_id = file_id;
        newarea.bank_num_out = BANK_NUM_UNASSIGNED;
        list_additem(&arealist, &newarea);
        return true;
    } else
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


// Track Min/Max assigned banks used
static void bank_update_assigned_minmax(uint16_t bank_num) {

    if (bank_num > bank_assigned_rom_max)
        bank_assigned_rom_max = bank_num;

    if (bank_num < bank_assigned_rom_min)
        bank_assigned_rom_min = bank_num;

    bank_update_all_max(bank_num);
}


// Tracks Max bank for *all* banks used, including fixed banks outside max limits
static void bank_update_all_max(uint16_t bank_num) {

    if (bank_num > bank_assigned_rom_max_alltypes)
        bank_assigned_rom_max_alltypes = bank_num;
}


// Add an area to a bank.
// It should only error out when there isn't enough
// room with a fixed-bank area (non-autobank)
static void bank_add_area(bank_item * p_bank, uint16_t bank_num, area_item * p_area) {

    // Make sure there is room and then update free space
    if (p_area->size > p_bank->free) {

        // Trying to add an auto-bank area to a full bank should be prevented by previous tests, but just in case
        if (p_area->bank_num_in == BANK_NUM_AUTO) {
            printf("BankPack: ERROR! Auto-banked Area %s, bank %d, size %d won't fit in assigned bank %d (%d bytes free, %d bytes reserved)\n",
                    p_area->name, p_area->bank_num_in, p_area->size, bank_num, p_bank->free, p_bank->reserved);
            exit(EXIT_FAILURE);
        } else {
            // Only warn for fixed bank areas. Don't exit and add the area anyway
            printf("BankPack: Warning: Fixed-bank Area %s, bank %d, size %d won't fit in assigned bank %d (%d bytes free, %d bytes reserved)\n",
                    p_area->name, p_area->bank_num_in, p_area->size, bank_num, p_bank->free, p_bank->reserved);
        }

        // Force bank space to zero, subtracting at this point would overflow
        p_bank->free = 0;
    } else
        p_bank->free -= p_area->size;

    // Copy bank type from area: some platforms (sms) don't allow mixing of area types in the same bank
    // Assign outbound bank number for the area
    p_bank->type = p_area->type;
    p_bank->item_count++;
    p_area->bank_num_out = bank_num;
    bank_update_assigned_minmax(bank_num);
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
                p_area->name, p_area->bank_num_in, p_area->size, BANK_SIZE_ROM, file_get_name_in_by_id(p_area->file_id));
        exit(EXIT_FAILURE);
    }
}


// Display a error message about bank mixing
static void bank_report_mixed_area_error(bank_item * p_bank, uint16_t bank_num, area_item * p_area) {

    printf("BankPack: ERROR! Bank %d already assigned different area type.\n"
           "  Can't mix _CODE_ and _LIT_ areas in the same bank for this platform.\n"
           "  Rejecting: Area %s, bank %d, file:%s\n",
           p_area->bank_num_in, p_area->name, bank_num, file_get_name_in_by_id(p_area->file_id));

    if (option_get_verbose())
        banks_show();
}


// Checks whether mixing area types in the same bank should be rejected
static bool bank_check_mixed_area_types_ok(bank_item * p_bank, uint16_t bank_num, area_item * p_area) {

    // Don't allow mixing of _CODE_ and _LIT_ for sms/gg ports
    // If one type has already been assigned to the bank, lock others out
    if ((option_get_platform() == PLATFORM_SMS) &&
        (p_bank->type != BANK_TYPE_UNSET) &&
        (p_area->type != p_bank->type))
        return false;
    else
        return true;
}


// Verify that a bank is available for an area
// Checks: Size, MBC Availability, Mixed area restrictions
static bool bank_check_ok_for_area(uint16_t bank_num, area_item * p_area, bank_item * banks) {

    // Check for MBC bank restrictions
    if ((option_get_mbc_type() != MBC_TYPE_MBC1) || (bank_check_mbc1_ok(bank_num))) {
        // Check for allowed area mixing if needed
        if (bank_check_mixed_area_types_ok(&banks[bank_num], bank_num, p_area)) {
            // Make sure there is enough space for the area
            if (p_area->size <= banks[bank_num].free) {
                return true;
            }
        }
    }

    return false;
}


// Assign areas randomly and distributed as sparsely as possible
static bool banks_assign_area_random(area_item * p_area, bank_item * banks) {

    uint16_t bank_num;
    uint16_t item_count_min = BANK_ITEM_COUNT_MAX;
    uint16_t list_of_banks[BANK_ROM_TOTAL];
    uint16_t list_count = 0;

    // Find lowest number of areas per bank among banks which have sufficient space for the area
    for (bank_num = bank_limit_rom_min; bank_num <= bank_limit_rom_max; bank_num++)
        if ((banks[bank_num].item_count < item_count_min) && bank_check_ok_for_area(bank_num, p_area, banks))
            item_count_min = banks[bank_num].item_count;

    // Now make a list of suitable banks below that threshold
    for (bank_num = bank_limit_rom_min; bank_num <= bank_limit_rom_max; bank_num++)
        if ((banks[bank_num].item_count <= item_count_min) && bank_check_ok_for_area(bank_num, p_area, banks))
            list_of_banks[list_count++] = bank_num;

    if (list_count > 0) {
        // Choose a random bank from the selected banks and add the area to it
        bank_num = list_of_banks[ rand() % list_count ];
        bank_add_area(&banks[bank_num], bank_num, p_area);

        return true;
    } else
        return false; // Fail if no banks were available
}


// Assign areas linearly, trying to fill up lowest banks first
static bool banks_assign_area_linear(area_item * p_area, bank_item * banks) {

    uint16_t bank_num;

    // Try to assign area to first allowed bank with enough free space
    // Bank array index maps directly to bank numbers, so [2] will be _CODE_2
    for (bank_num = bank_limit_rom_min; bank_num <= bank_limit_rom_max; bank_num++) {
        if (bank_check_ok_for_area(bank_num, p_area, banks)) {
            bank_add_area(&banks[bank_num], bank_num, p_area);
            return true;
        }
    }
    return false; // Fail if no banks were available
}


// Find a bank for a given area using First Fit Decreasing (FFD)
// All possible banks (0-255) were pre-created and initialized [in obj_data_init()],
// so there is no need to add when using a fresh bank
static void banks_assign_area(area_item * p_area) {

    uint16_t bank_num;
    bank_item * banks = (bank_item *)banklist.p_array;
    bool result;

    bank_check_area_size(p_area);

    // Try to assign fixed bank areas to their expected bank.
    // (ignore the area if it's outside the processing range)
    if (p_area->bank_num_in != BANK_NUM_AUTO) {

        bank_num = p_area->bank_num_in;

        // Update max bank var that tracks regardless of limits
        // For auto banks this will get updated via bank_add_area()
        bank_update_all_max(bank_num);

        if ((bank_num >= bank_limit_rom_min) &&
            (bank_num <= bank_limit_rom_max)) {

            if ((option_get_mbc_type() == MBC_TYPE_MBC1) && (!bank_check_mbc1_ok(bank_num)))
                printf("BankPack: Warning: Area in fixed bank assigned to MBC1 excluded bank: %d, file: %s\n", bank_num, file_get_name_in_by_id(p_area->file_id));

            if (!bank_check_mixed_area_types_ok(&banks[bank_num], bank_num, p_area)) {
                bank_report_mixed_area_error(&banks[bank_num], bank_num, p_area);
                exit(EXIT_FAILURE);
            }

            bank_add_area(&banks[bank_num], bank_num, p_area);
        }
        //else
        //    printf("BankPack: Notice: Ignoring Area in fixed bank %d outside specified range %d - %d, file: %s\n", bank_num, bank_limit_rom_min, bank_limit_rom_max, file_get_name_in_by_id(p_area->file_id));

        return;
    }
    else if (p_area->bank_num_in == BANK_NUM_AUTO) {

        if (option_get_random_assign())
            result = banks_assign_area_random(p_area, banks);
        else
            result = banks_assign_area_linear(p_area, banks);

        if (result) // Success
            return;
    }

    if (option_get_verbose())
        banks_show();
    printf("BankPack: ERROR! Failed to assign bank for Area %s, bank %d, size %d. Out of banks!\n",
            p_area->name, p_area->bank_num_in, p_area->size);
    exit(EXIT_FAILURE);
}


#define QSORT_A_FIRST -1
#define QSORT_A_SAME   0
#define QSORT_A_AFTER  1

// qsort compare rule function for sorting areas
static int area_item_compare(const void* a, const void* b) {

    // sort by bank [asc] (fixed vs auto-bank), then by size [desc]
    if (((area_item *)a)->bank_num_in != ((area_item *)b)->bank_num_in)
        return  (((area_item *)a)->bank_num_in < ((area_item *)b)->bank_num_in) ? QSORT_A_FIRST : QSORT_A_AFTER;
    else if (((area_item *)a)->size != ((area_item *)b)->size)
        return (((area_item *)a)->size > ((area_item *)b)->size) ? QSORT_A_FIRST : QSORT_A_AFTER;
    else
        return QSORT_A_SAME;
}

static void areas_sort(void) {
    // Sort banks by name
    qsort (arealist.p_array, arealist.count, sizeof(area_item), area_item_compare);
}


// Assigns all areas -> banks, and bank numbers -> files
// Fixed bank areas are placed first, then auto-banks fill the rest in
// Only call after all areas have been collected from object files
void obj_data_process(list_type * p_filelist) {
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
void banks_show(void) {

    uint32_t c;
    uint32_t a;

    printf("\n=== Banks assigned: %d -> %d (allowed range %d -> %d). Max including fixed: %d) ===\n",
            bank_assigned_rom_min, bank_assigned_rom_max,
            bank_limit_rom_min,  bank_limit_rom_max,
            bank_assigned_rom_max_alltypes);

    bank_item * banks = (bank_item *)banklist.p_array;
    area_item * areas = (area_item *)arealist.p_array;
    for (c = 0; c < banklist.count; c++) {
        if (banks[c].free != BANK_SIZE_ROM) {
            printf("Bank %d: size=%5d, free=%5d, reserved=%5d\n", c, banks[c].size, banks[c].free, banks[c].reserved);
            for (a = 0; a < arealist.count; a++) {
                if (areas[a].bank_num_out == c) {
                    printf(" +- Area: name=%8s, size=%5d, bank_in=%3d, bank_out=%3d, file=%s -> %s\n",
                        areas[a].name,
                        areas[a].size,
                        areas[a].bank_num_in,
                        areas[a].bank_num_out,
                        file_get_name_in_by_id(areas[a].file_id),
                        file_get_name_out_by_id(areas[a].file_id));
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
        else if (strstr(strline_in, "A _LIT_255")) {
            fprintf(out_file, "A _LIT_%d %s", bank_num, strstr(strline_in, "size"));
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
    uint32_t bank_in = 0;
    char strmatch[OBJ_NAME_MAX_STR_LEN];
    char symbol_name[OBJ_NAME_MAX_STR_LEN];
    symbol_match_item * sym_match = (symbol_match_item *)symbol_matchlist.p_array;

    // Only rewrite banked symbol entries
    if (strline_in[0] == 'S') {
        // For lines: S b_<symbol name>... Def0000FF
        if (SYMBOL_REWRITE_RECORDS == sscanf(strline_in,"S b_%" TOSTR(OBJ_NAME_MAX_STR_LEN) "s Def%06x", symbol_name, &bank_in)) {
            if (bank_in == BANK_NUM_AUTO) {
                if (symbol_banked_check_rewrite_ok(symbol_name, file_id))  {
                    fprintf(out_file, "S b_%s Def0000%02x", symbol_name, bank_num);
                    if (strline_in[(strlen(strline_in)-1)] != '\n')
                        fprintf(out_file, "\n");
                    return true;
                }
            }
        } // For lines: S ___bank_<trailing symbol name>... Def0000FF
        else {
            for (c = 0; c < symbol_matchlist.count; c++) {
                // Prepare a sscanf match test string for the current symbol name
                if (snprintf(strmatch, sizeof(strmatch), "S %s%%" TOSTR(OBJ_NAME_MAX_STR_LEN) "s Def%%06x", sym_match[c].name) > sizeof(strmatch))
                    printf("BankPack: Warning: truncated symbol match string to:%s\n",strmatch);

                if (SYMBOL_REWRITE_RECORDS == sscanf(strline_in, strmatch, symbol_name, &bank_in)) {
                    if (bank_in == BANK_NUM_AUTO) {
                        fprintf(out_file, "S %s%s Def0000%02x", sym_match[c].name, symbol_name, bank_num);
                        if (strline_in[(strlen(strline_in)-1)] != '\n')
                            fprintf(out_file, "\n");
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
