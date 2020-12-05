
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

#define MAX_FILE_STR 2048

#define BANK_NUM_UNASSIGNED   0xFFFFU
#define BANK_NUM_AUTO         255
#define BANK_NUM_ROM_MIN      1
#define BANK_NUM_ROM_MAX      255
#define BANK_ROM_TOTAL        256 // Banks 0-255
#define BANK_SIZE_ROM         0x4000U

#define BANK_NUM_ROM_MAX_MBC1  127
#define BANK_NUM_ROM_MAX_MBC2  15
#define BANK_NUM_ROM_MAX_MBC3  127
#define BANK_NUM_ROM_MAX_MBC5  255 // 511 // TODO: support full MBC5 address range (currently 8 bit only)

#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)

#endif // _COMMON_H
