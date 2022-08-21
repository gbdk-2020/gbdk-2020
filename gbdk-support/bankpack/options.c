// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2022

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
#include "options.h"

extern list_type banklist;

// Format: -reserve=DECIMAL_BANKNUM:HEX_SIZE
// Reserve space in banks manually from command line arguments
// Should be called *after* obj_data_init() has initialized banks
int option_bank_reserve_bytes(char * arg_str) {

    bank_item * banks = (bank_item *)banklist.p_array;

    char cols;
    char * p_str;
    char * p_words[ARG_BANK_RESERVE_SIZE_MAX_SPLIT_WORDS];
    char arg_str_copy[ARG_BANK_RESERVE_SIZE_MAX_LEN]; // copy arg since strtok modifies strings it operates on
    snprintf(arg_str_copy, sizeof(arg_str_copy), "%s", arg_str);

    // Split string into words using "-:=" as delimiters
    cols = 0;
    p_str = strtok(arg_str_copy,"-:=");
    while (p_str != NULL)
    {
        p_words[cols++] = p_str;
        p_str = strtok(NULL, "-:=");
        if (cols >= ARG_BANK_RESERVE_SIZE_MAX_SPLIT_WORDS) break;
    }

    if (cols == ARG_BANK_RESERVE_SIZE_REC_COUNT_MATCH) {

        uint32_t reserve_bank = strtol(p_words[1], NULL, 10); // [1] Decimal Bank Number
        uint32_t reserve_size = strtol(p_words[2], NULL, 16); // [2] Hex Reserve Size

        if ((reserve_bank < BANK_NUM_ROM_MIN) || (reserve_bank > BANK_NUM_ROM_MAX)) {
            printf("BankPack: ERROR! Bank number %d for %s is invalid (min: %d, max: %d)\n", reserve_bank, arg_str, BANK_NUM_ROM_MIN, BANK_NUM_ROM_MAX);
            exit(EXIT_FAILURE);
        }

        if (reserve_size > BANK_SIZE_ROM_MAX) {
            printf("BankPack: ERROR! Size value %x for %s is too large (max:%x)\n", reserve_size, arg_str, BANK_SIZE_ROM_MAX);
            exit(EXIT_FAILURE);
        }

        if (reserve_size > banks[reserve_bank].free) {
            printf("BankPack: ERROR! Size of %x bytes for %s is larger than free space (%x bytes) in bank %d\n", reserve_size, arg_str, banks[reserve_bank].free, reserve_bank);
            exit(EXIT_FAILURE);
        }

        // printf("Bankpack: Reserving %x bytes for bank %d (%s)\n", reserve_size, reserve_bank, arg_str);

        // Params were valid, deduct the space from the bank
        banks[reserve_bank].free -= reserve_size;
        banks[reserve_bank].reserved += reserve_size;
        return true;
    } else
        return false; // Signal failure

}