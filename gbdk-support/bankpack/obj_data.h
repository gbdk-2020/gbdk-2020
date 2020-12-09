// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _AREAS_H
#define _AREAS_H

#include "list.h"

#define OBJ_NAME_MAX_STR_LEN 255
#define AREA_LINE_RECORDS      2 // Bank number, Size
#define SYMBOL_LINE_RECORDS    2 // Name, DefVal


typedef struct bank_item {
    uint32_t size;
    uint32_t free;
} bank_item;


typedef struct area_item {
    uint32_t file_id;
    char     name[OBJ_NAME_MAX_STR_LEN];
    uint32_t size;         // uint32_t to avoid mingw sscanf() buffer overflow
    uint32_t bank_num_in;  // uint32_t to avoid mingw sscanf() buffer overflow
    uint32_t bank_num_out; // uint32_t to avoid mingw sscanf() buffer overflow
} area_item;

typedef struct symbol_item {
    uint32_t file_id;
    char     name[OBJ_NAME_MAX_STR_LEN];
    bool     is_banked_def;
    uint32_t bank_num_in;   // uint32_t to avoid mingw sscanf() buffer overflow
    bool     found_matching_symbol;
} symbol_item;

typedef struct symbol_match_item {
    char     name[OBJ_NAME_MAX_STR_LEN];
} symbol_match_item;

int banks_get_mbc_type(void);
void banks_set_mbc(int);
void banks_set_mbc_by_rom_byte_149(int);

uint32_t banks_calc_cart_size(void);

bool banks_set_min(uint16_t bank_num);
bool banks_set_max(uint16_t bank_num);

void obj_data_init(void);
void obj_data_cleanup(void);

int areas_add(char * area_str, uint32_t file_id);
int symbols_add(char * area_str, uint32_t file_id);
void symbol_match_add(char *);

int obj_data_process(list_type *);

bool area_modify_and_write_to_file(char * strline_in, FILE * out_file, uint16_t bank_num);
bool symbol_modify_and_write_to_file(char * strline_in, FILE * out_file, uint16_t bank_num, uint32_t file_id);

int banks_show(void);


#endif // _AREAS_H