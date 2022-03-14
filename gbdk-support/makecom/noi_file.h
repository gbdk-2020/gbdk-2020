// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _NOI_FILE_H
#define _NOI_FILE_H

#include "list.h"

#define MAX_STR_LEN     4096
#define MAX_SPLIT_WORDS 4

#define NOI_REC_COUNT_MATCH 3 // "DEF ...symbol_name... ...symbol_value...

#define ERR_NO_SYMBOLS_LEFT -1
#define SYM_VAL_UNSET   0xFFFFFFFF

#define NOI_REC_START_LEN 4 // length of "DEF "
#define NOI_REC_S_L_CHK   2 // length of "s_" or "l_"
#define NOI_REC_START   's'
#define NOI_REC_LENGTH  'l'

typedef struct symbol_item {
    char     name[SYM_MAX_STR_LEN];
    uint32_t addr_start;
    uint32_t length;
    uint16_t bank_num;
    uint32_t src_rom_addr;
} symbol_item;

extern list_type symbol_list;
extern uint32_t overlay_count_addr;
extern uint32_t overlay_name_addr;


int noi_file_load_symbols(char * filename_in);
void noi_init(void);
void noi_cleanup(void);

#endif // _NOI_FILE_H