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
extern uint16_t bank_limit_rom_min;
extern uint16_t bank_limit_rom_max;
extern uint16_t bank_assigned_rom_max_alltypes;

bool option_verbose       = false;
bool option_cartsize      = false;
bool option_random_assign = false;
int  option_mbc_type = MBC_TYPE_DEFAULT;
int  option_platform = PLATFORM_DEFAULT;


void option_set_verbose(bool is_enabled) {
    option_verbose = is_enabled;
}
bool option_get_verbose(void) {
    return option_verbose;
}


void option_set_cartsize(bool is_enabled) {
    option_cartsize = is_enabled;
}
bool option_get_cartsize(void) {
    return option_cartsize;
}


void option_set_random_assign(bool is_enabled) {
    option_random_assign = is_enabled;
}
bool option_get_random_assign(void) {
    return option_random_assign;
}



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


int option_get_platform(void) {
    return option_platform;
}


void option_set_platform(char * platform_str) {

    if (strcmp(platform_str, PLATFORM_STR_GB) == 0)
        option_platform = PLATFORM_GB;
    else if (strcmp(platform_str, PLATFORM_STR_AP) == 0)
        option_platform = PLATFORM_GB;  // Analogue Pocket uses GB platform
    else if (strcmp(platform_str, PLATFORM_STR_DUCK) == 0)
        option_platform = PLATFORM_GB;  // Megaduck uses GB platform
    else if (strcmp(platform_str, PLATFORM_STR_NES) == 0)
        option_platform = PLATFORM_GB;  // NES uses GB platform
    else if (strcmp(platform_str, PLATFORM_STR_SMS) == 0)
        option_platform = PLATFORM_SMS;
    else if (strcmp(platform_str, PLATFORM_STR_GG) == 0)
        option_platform = PLATFORM_SMS; // GG uses SMS platform
    else if (strcmp(platform_str, PLATFORM_STR_MSXDOS) == 0)
        option_platform = PLATFORM_SMS; // MSXDOS uses SMS platform
    else
        printf("BankPack: Warning: Invalid platform option %s\n", platform_str);
}



int option_get_mbc_type(void) {
    return option_mbc_type;
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
void option_mbc_by_rom_byte_149(int mbc_type_rom_byte) {

    switch (mbc_type_rom_byte) {
        // NO MBC
        case 0x00U: //  0-ROM ONLY
        case 0x08U: //  2-ROM+MBC1+RAM
        case 0x09U: //  8-ROM+RAM
        case 0x0BU: //  B-ROM+MMM01
        case 0x0CU: //  C-ROM+MMM01+SRAM
        case 0x0DU: //  D-ROM+MMM01+SRAM+BATT
            option_set_mbc(MBC_TYPE_NONE);
            break;

        // MBC 1
        case 0x01U: //  1-ROM+MBC1
        case 0x02U: //  2-ROM+MBC1+RAM
        case 0x03U: //  3-ROM+MBC1+RAM+BATT
            option_set_mbc(MBC_TYPE_MBC1);
            break;

        // MBC 2
        case 0x05U: //  5-ROM+MBC2
        case 0x06U: //  6-ROM+MBC2+BATTERY
            option_set_mbc(MBC_TYPE_MBC2);
            break;

        // MBC 3
        case 0x0FU: //  F-ROM+MBC3+TIMER+BATT
        case 0x10U: //  10-ROM+MBC3+TIMER+RAM+BATT
        case 0x11U: //  11-ROM+MBC3
        case 0x12U: //  12-ROM+MBC3+RAM
        case 0x13U: //  13-ROM+MBC3+RAM+BATT
        case 0xFCU: //  FC-GAME BOY CAMERA
            option_set_mbc(MBC_TYPE_MBC3);
            break;

        // MBC 5
        case 0x19U: //  19-ROM+MBC5
        case 0x1AU: //  1A-ROM+MBC5+RAM
        case 0x1BU: //  1B-ROM+MBC5+RAM+BATT
        case 0x1CU: //  1C-ROM+MBC5+RUMBLE
        case 0x1DU: //  1D-ROM+MBC5+RUMBLE+SRAM
        case 0x1EU: //  1E-ROM+MBC5+RUMBLE+SRAM+BATT
            option_set_mbc(MBC_TYPE_MBC5);
            break;

        default:
            printf("BankPack: Warning: unrecognized MBC option -yt=%x\n", mbc_type_rom_byte);
            break;
    }
}


// Set MBC type directly
void option_set_mbc(int mbc_type) {

    uint16_t mbc_bank_limit_rom_max;

    switch (mbc_type) {
        case MBC_TYPE_NONE:
            option_mbc_type = MBC_TYPE_NONE;
            break;
        case MBC_TYPE_MBC1:
            option_mbc_type = mbc_type;
            mbc_bank_limit_rom_max = BANK_NUM_ROM_MAX_MBC1;
            break;
        case MBC_TYPE_MBC2:
            option_mbc_type = mbc_type;
            mbc_bank_limit_rom_max = BANK_NUM_ROM_MAX_MBC2;
            break;
        case MBC_TYPE_MBC3:
            option_mbc_type = mbc_type;
            mbc_bank_limit_rom_max = BANK_NUM_ROM_MAX_MBC3;
            break;
        case MBC_TYPE_MBC5:
            option_mbc_type = mbc_type;
            mbc_bank_limit_rom_max = BANK_NUM_ROM_MAX_MBC5;
            break;
        default:
            printf("BankPack: Warning: unrecognized MBC option -mbc%d!\n", mbc_type);
            break;
    }
    if (mbc_bank_limit_rom_max < bank_limit_rom_max)
        bank_limit_rom_max = mbc_bank_limit_rom_max;
}



// From makebin
//  Byte
//  0148 - ROM size             -yt<N>
//  0    - 256Kbit = 32KByte  =   2 banks
//  1    - 512Kbit = 64KByte  =   4 banks
//  2    - 1Mbit   = 128KByte =   8 banks
//  3    - 2Mbit   = 256KByte =  16 banks
//  4    - 4Mbit   = 512KByte =  32 banks
//  5    - 8Mbit   = 1MByte   =  64 banks
//  6    - 16Mbit  = 2MByte   = 128 banks
//  7    - 16Mbit  = 2MByte   = 256 banks
//  8    - 16Mbit  = 2MByte   = 512 banks
//
//  Not supported by makebin:
//  $52 - 9Mbit = 1.1MByte = 72 banks
//  $53 - 10Mbit = 1.2MByte = 80 banks
//  $54 - 12Mbit = 1.5MByte = 96 banks
uint32_t option_banks_calc_cart_size(void) {

    uint32_t req_banks = 1;

    if ((bank_assigned_rom_max_alltypes + 1) > BANK_ROM_CALC_MAX) {
        printf("BankPack: Warning! Can't calc cart size, too many banks: %d (max %d)\n", (bank_assigned_rom_max_alltypes + 1), BANK_ROM_CALC_MAX);
        return (0);
    }

    // Calculate nearest upper power of 2
    while (req_banks < (bank_assigned_rom_max_alltypes + 1))
        req_banks *= 2;

    return (req_banks);
}


bool option_banks_set_min(uint16_t bank_num) {
    if (bank_num < BANK_NUM_ROM_MIN)
        return false;
    else bank_limit_rom_min = bank_num;

    return true;
}

bool option_banks_set_max(uint16_t bank_num) {
    if (bank_num > BANK_NUM_ROM_MAX)
        return false;
    else bank_limit_rom_max = bank_num;

    return true;
}

