// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2022


#define ARG_BANK_RESERVE_SIZE_MAX_LEN 256u   //  -reserve=255:4000
#define ARG_BANK_RESERVE_SIZE_REC_COUNT_MATCH 3u
#define ARG_BANK_RESERVE_SIZE_MAX_SPLIT_WORDS (ARG_BANK_RESERVE_SIZE_REC_COUNT_MATCH + 1u)


int option_bank_reserve_bytes(char * arg_str);

