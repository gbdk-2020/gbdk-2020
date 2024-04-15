// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2022


#define ARG_BANK_RESERVE_SIZE_MAX_LEN 256u   //  -reserve=255:4000
#define ARG_BANK_RESERVE_SIZE_REC_COUNT_MATCH 3u
#define ARG_BANK_RESERVE_SIZE_MAX_SPLIT_WORDS (ARG_BANK_RESERVE_SIZE_REC_COUNT_MATCH + 1u)

#define ARG_BANK_SET_TYPE_MAX_LEN 256u   //  -banktype=255:LIT
#define ARG_BANK_SET_TYPE_REC_COUNT_MATCH 3u
#define ARG_BANK_SET_TYPE_MAX_SPLIT_WORDS (ARG_BANK_RESERVE_SIZE_REC_COUNT_MATCH + 1u)

#define PLATFORM_GB                 0
#define PLATFORM_SMS                1
#define PLATFORM_DEFAULT            PLATFORM_GB

#define PLATFORM_STR_GB             "gb"
#define PLATFORM_STR_AP             "ap"     // Uses PLATFORM_GB
#define PLATFORM_STR_DUCK           "duck"   // Uses PLATFORM_GB
#define PLATFORM_STR_NES            "nes"    // Uses PLATFORM_GB
#define PLATFORM_STR_SMS            "sms"
#define PLATFORM_STR_GG             "gg"     // Uses PLATFORM_SMS
#define PLATFORM_STR_MSXDOS         "msxdos" // Uses PLATFORM_SMS


void option_set_verbose(bool is_enabled);
bool option_get_verbose(void);

void option_set_cartsize(bool is_enabled);
bool option_get_cartsize(void);

void option_set_random_assign(bool is_enabled);
bool option_get_random_assign(void);

int  option_bank_reserve_bytes(char * arg_str);
int option_bank_set_type(char * arg_str);

void option_set_platform(char * platform_str);
int  option_get_platform(void);

int  option_get_mbc_type(void);
void option_set_mbc(int);
void option_mbc_by_rom_byte_149(int);

void option_set_nes_mapper(int mapper_type);

uint32_t option_banks_calc_cart_size(void);

bool option_banks_set_min(uint16_t bank_num);
bool option_banks_set_max(uint16_t bank_num);

void option_banks_set_random(bool is_random);

