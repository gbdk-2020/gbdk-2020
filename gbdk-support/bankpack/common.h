
#ifndef _COMMON_H
#define _COMMON_H

enum {
    MBC_TYPE_NONE = 0,
    MBC_TYPE_MBC1 = 1,
    MBC_TYPE_MBC2 = 2,
    MBC_TYPE_MBC3 = 3,
    /* MBC4 doesn't exist */
    MBC_TYPE_MBC5 = 5,

    MBC_TYPE_MIN = MBC_TYPE_MBC1,
    MBC_TYPE_MAX = MBC_TYPE_MBC5,
    MBC_TYPE_DEFAULT =  MBC_TYPE_NONE
};

enum {
    NES_MAPPER_TYPE_NONE = 0,
    NES_MAPPER_TYPE_UXROM = 2,
    NES_MAPPER_TYPE_MAPPER30 = 30,

    NES_MAPPER_TYPE_MIN = NES_MAPPER_TYPE_NONE,
    NES_MAPPER_TYPE_MAX = NES_MAPPER_TYPE_MAPPER30,
    NES_MAPPER_TYPE_DEFAULT = NES_MAPPER_TYPE_MAPPER30
};

#define MAX_FILE_STR 2048

#define BANK_NUM_UNASSIGNED   0xFFFFU
#define BANK_NUM_AUTO         255
#define BANK_NUM_ROM_MIN      0
#define BANK_NUM_ROM_MAX      255
#define BANK_NUM_ROM_DEFAULT  1
#define BANK_ROM_TOTAL        256 // Banks 0-255
#define BANK_ROM_CALC_MAX     512 // Banks 0-512 (>256 not supported for auto-banking right now)
#define BANK_SIZE_ROM         0x4000U
#define BANK_SIZE_ROM_MAX     (BANK_SIZE_ROM)

#define BANK_ITEM_COUNT_MAX 0xFFFF

#define BANK_NUM_ROM_MAX_MBC1  127
#define BANK_NUM_ROM_MAX_MBC2  15
#define BANK_NUM_ROM_MAX_MBC3  127
#define BANK_NUM_ROM_MAX_MBC5  255 // 511 // TODO: support full MBC5 address range (currently 8 bit only)

#define BANK_NUM_ROM_MAX_UXROM     127  // Oversize UNROM supports up to 4MB, but iNES 1.0 only supports up to 2MB. Historical UxROM boards only supported 256kB at most
#define BANK_NUM_ROM_MAX_MAPPER30  31   // Mapper30 only supports up to 512kB due to other register bits being used for CHR-RAM / 1-screen mirroring control.

#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)

#endif // _COMMON_H
