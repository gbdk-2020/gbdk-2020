// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "logging.h"
#include "banks.h"
#include "bank_templates.h"

// Bank info from pandocs
//  0000-3FFF   16KB ROM Bank 00            (in cartridge, fixed at bank 00)
//  4000-7FFF   16KB ROM Bank 01..NN        (in cartridge, switchable bank number)
//  8000-9FFF   8KB Video RAM (VRAM)        (switchable bank 0-1 in CGB Mode)
//  A000-BFFF   8KB External RAM            (in cartridge, switchable bank, if any)
//  C000-CFFF   4KB Work RAM Bank 0 (WRAM)
//  D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
//  E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)


// // Previous unified bank declaration
// const bank_item bank_templates_default[] = {
//     {"ROM_0",   0x0000, 0x3FFF, BANKED_NO,  0x7FFF, 0,0,0, BANK_MEM_TYPE_ROM,  BANK_STARTNUM_0},
//     {"ROM_",    0x4000, 0x7FFF, BANKED_YES, 0x7FFF, 0,0,0, BANK_MEM_TYPE_ROM,  BANK_STARTNUM_1},
//     {"VRAM_",   0x8000, 0x9FFF, BANKED_YES, 0x9FFF, 0,0,0, BANK_MEM_TYPE_VRAM, BANK_STARTNUM_0},
//     {"SRAM_",   0xA000, 0xBFFF, BANKED_YES, 0xBFFF, 0,0,0, BANK_MEM_TYPE_SRAM, BANK_STARTNUM_0},
//     {"WRAM_LO", 0xC000, 0xCFFF, BANKED_NO,  0xDFFF, 0,0,0, BANK_MEM_TYPE_WRAM, BANK_STARTNUM_0},
//     {"WRAM_HI_",0xD000, 0xDFFF, BANKED_YES, 0xDFFF, 0,0,0, BANK_MEM_TYPE_WRAM, BANK_STARTNUM_1},
//     {"HRAM",    0xFF80, 0xFFFE, BANKED_NO,  0xFFFE, 0,0,0, BANK_MEM_TYPE_HRAM, BANK_STARTNUM_0},
// };


// ===== Game Boy =====

// ROM
const bank_item ROM_0 =          {"ROM_0",   0x0000, 0x3FFF, BANKED_NO,  0x7FFF, 0,0,0, BANK_MEM_TYPE_ROM,  BANK_STARTNUM_0, BANK_MERGED_NO, HIDDEN_NO};
const bank_item ROM_X_banked =   {"ROM_",    0x4000, 0x7FFF, BANKED_YES, 0x7FFF, 0,0,0, BANK_MEM_TYPE_ROM,  BANK_STARTNUM_1, BANK_MERGED_NO, HIDDEN_NO};
// Merged version
const bank_item ROM_nonbanked =  {"ROM",     0x0000, 0x7FFF, BANKED_YES, 0x7FFF, 0,0,0, BANK_MEM_TYPE_ROM,  BANK_STARTNUM_0, BANK_MERGED_YES, HIDDEN_NO};

// VRAM
const bank_item VRAM =           {"VRAM_",   0x8000, 0x9FFF, BANKED_YES, 0x9FFF, 0,0,0, BANK_MEM_TYPE_VRAM, BANK_STARTNUM_0, BANK_MERGED_NO, HIDDEN_NO};

// SRAM
const bank_item SRAM =           {"SRAM_",   0xA000, 0xBFFF, BANKED_YES, 0xBFFF, 0,0,0, BANK_MEM_TYPE_SRAM, BANK_STARTNUM_0, BANK_MERGED_NO, HIDDEN_NO};

// WRAM
const bank_item WRAM_0 =         {"WRAM_LO", 0xC000, 0xCFFF, BANKED_NO,  0xDFFF, 0,0,0, BANK_MEM_TYPE_WRAM, BANK_STARTNUM_0, BANK_MERGED_NO, HIDDEN_NO};
const bank_item WRAM_X_banked =  {"WRAM_HI_",0xD000, 0xDFFF, BANKED_YES, 0xDFFF, 0,0,0, BANK_MEM_TYPE_WRAM, BANK_STARTNUM_1, BANK_MERGED_NO, HIDDEN_NO};
// Merged version
const bank_item WRAM_nonbanked = {"WRAM",    0xC000, 0xDFFF, BANKED_YES, 0xDFFF, 0,0,0, BANK_MEM_TYPE_WRAM, BANK_STARTNUM_0, BANK_MERGED_YES, HIDDEN_NO};

// HRAM
const bank_item HRAM =           {"HRAM",    0xFF80, 0xFFFE, BANKED_NO,  0xFFFE, 0,0,0, BANK_MEM_TYPE_HRAM, BANK_STARTNUM_0, BANK_MERGED_NO, HIDDEN_NO};


// ===== Game Gear =====

// https://www.smspower.org/Development/MemoryMap

// _CODE_<N> is at base address 0x4000 (code and assets)
// _LIT_<N> is at base address 0x8000 (assets)
// _DATA_N is also at base address 0x8000 (RAM)

const bank_item smsgg_ROM_0 =          {"ROM_0",   0x0000, 0x3FFF, BANKED_NO,  0x7FFF, 0,0,0, BANK_MEM_TYPE_ROM,  BANK_STARTNUM_0, BANK_MERGED_NO, HIDDEN_NO};
const bank_item smsgg_ROM_X_banked =   {"ROM_",    0x4000, 0x7FFF, BANKED_YES, 0x7FFF, 0,0,0, BANK_MEM_TYPE_ROM,  BANK_STARTNUM_1, BANK_MERGED_NO, HIDDEN_NO};
// Merged version
const bank_item smsgg_ROM_nonbanked =  {"ROM",     0x0000, 0x7FFF, BANKED_YES, 0x7FFF, 0,0,0, BANK_MEM_TYPE_ROM,  BANK_STARTNUM_0, BANK_MERGED_YES, HIDDEN_NO};

const bank_item smsgg_LIT_X_banked =   {"LIT_",    0x8000, 0xBFFF, BANKED_YES, 0xBFFF, 0,0,0, BANK_MEM_TYPE_ROM,  BANK_STARTNUM_1, BANK_MERGED_NO, HIDDEN_NO};
// Data can also be in the 0x8000 region.. requires some special handling in banks_check()
const bank_item smsgg_DATA_X_banked =  {"DATA_",   0x8000, 0xBFFF, BANKED_YES, 0xBFFF, 0,0,0, BANK_MEM_TYPE_SRAM,  BANK_STARTNUM_1, BANK_MERGED_NO, HIDDEN_NO};
const bank_item smsgg_RAM_nonbanked =  {"RAM",     0xC000, 0xDFFF, BANKED_YES, 0xDFFF, 0,0,0, BANK_MEM_TYPE_WRAM, BANK_STARTNUM_0, BANK_MERGED_YES, HIDDEN_NO};


static int bank_template_add(int idx, bank_item * p_bank_templates, const bank_item  * p_bank) {

    if (idx >= BANK_TEMPLATES_MAX) {
        log_error("Error: exceeded max bank template\n");
        exit(EXIT_FAILURE);
    }

    p_bank_templates[idx++] = *p_bank;
    return idx;
}


// TODO: FIXME: With -B and -cWRAM and -cROM
//        - Calc total size is wrong... maybe 2x?
//        - Graph renders wrong (maybe due to total size wrong)

int bank_templates_load(bank_item * p_bank_templates) {

    int idx = 0;

    if (get_option_platform() == OPT_PLAT_SMS_GG_GBDK) {
        if (option_merged_banks & OPT_MERGED_BANKS_ROM) {
            idx = bank_template_add(idx, p_bank_templates, &smsgg_ROM_nonbanked);
        } else {
            idx = bank_template_add(idx, p_bank_templates, &smsgg_ROM_0);
            idx = bank_template_add(idx, p_bank_templates, &smsgg_ROM_X_banked);
        }
        idx = bank_template_add(idx, p_bank_templates, &smsgg_LIT_X_banked);
        idx = bank_template_add(idx, p_bank_templates, &smsgg_DATA_X_banked);
        idx = bank_template_add(idx, p_bank_templates, &smsgg_RAM_nonbanked);

    }
    else { // implied: if (get_option_platform() == OPT_PLAT_GAMEBOY) {

        if (option_merged_banks & OPT_MERGED_BANKS_ROM) {
            idx = bank_template_add(idx, p_bank_templates, &ROM_nonbanked);
        } else {
            idx = bank_template_add(idx, p_bank_templates, &ROM_0);
            idx = bank_template_add(idx, p_bank_templates, &ROM_X_banked);
        }

        idx = bank_template_add(idx, p_bank_templates, &VRAM);
        idx = bank_template_add(idx, p_bank_templates, &SRAM);

        if (option_merged_banks & OPT_MERGED_BANKS_WRAM) {
            idx = bank_template_add(idx, p_bank_templates, &WRAM_nonbanked);
        } else {
            idx = bank_template_add(idx, p_bank_templates, &WRAM_0);
            idx = bank_template_add(idx, p_bank_templates, &WRAM_X_banked);
        }

        idx = bank_template_add(idx, p_bank_templates, &HRAM);
    }

    return idx;
}