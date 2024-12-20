// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _BANKS_H
#define _BANKS_H

#include "list.h"

#define WRAM_X_MAX_BANKS        7

#define MAX_ADDR_UNBANKED       0x0000FFFFU
#define BANK_ADDR_ROM_UPPER_ST  0x00004000U  // Upper ROM Bank address bit
#define BANK_ADDR_ROM_UPPER_END 0x00007FFFU  // Upper ROM Bank address bit
#define BANK_ADDR_VADDR_MASK    0xFFFF0000U  // Virtual addressing mask for where .noi files/etc store bank number
#define BANK_NUM_ROM1_VADDR     (1u << 16)
#define BANK_NUM_ROM0           (0u)
#define BANK_NUM_ROM1           (1u)

// Used for ignoring address range check/clipping when copying
// areas from multiple banks into a single collapsed bank
#define ADDR_NO_CLIP_MIN        0x00000000
#define ADDR_NO_CLIP_MAX        0xFFFFFFFF

#define AREA_COPY_ADDRESS_TRANSLATE_YES true
#define AREA_COPY_ADDRESS_TRANSLATE_NO  false

#define ARRAY_LEN(A)         (sizeof(A) / sizeof(A[0]))
#define WITHOUT_BANK(addr)   ((addr) & MAX_ADDR_UNBANKED)
#define BANK_GET_NUM(addr)   (((addr) & BANK_ADDR_VADDR_MASK) >> 16)
#define BANK_ONLY(addr)      ((addr) & BANK_ADDR_VADDR_MASK)
#define RANGE_SIZE(MIN, MAX) (MAX - MIN + 1)
#define UNBANKED_END(start, end)  ((end - start) + WITHOUT_BANK(start))

#define AREA_MAX_STR DEFAULT_STR_LEN

#define BANK_MAX_STR DEFAULT_STR_LEN
#define BANKED_NO     0
#define BANKED_YES    1

#define BANK_STARTNUM_0 0
#define BANK_STARTNUM_1 1

#define BANK_MERGED_NO     false
#define BANK_MERGED_YES    true

#define HIDDEN_NO          false
#define HIDDEN_YES         true

#define MINIGRAPH_SIZE (2 * 14) // Number of characters wide (inside edge brackets)
#define LARGEGRAPH_BYTES_PER_CHAR 16

typedef enum {
    BANK_MEM_TYPE_ROM,
    BANK_MEM_TYPE_VRAM,
    BANK_MEM_TYPE_SRAM,
    BANK_MEM_TYPE_WRAM,
    BANK_MEM_TYPE_HRAM
} bank_mem_types;


typedef struct area_item {
    char     name[AREA_MAX_STR];
    uint32_t start;
    uint32_t end;
    uint32_t start_unbanked;
    uint32_t end_unbanked;
    uint32_t length;
    bool     exclusive;
} area_item;


typedef struct bank_item {
    // Mostly fixed values
    char     name[BANK_MAX_STR];
    uint32_t start;
    uint32_t end;        // May get modified for summarized output
    int      is_banked;
    uint32_t overflow_end;

    // Updateable values
    uint32_t size_total;
    uint32_t size_used;
    int      bank_num;

    int      bank_mem_type;
    int      base_bank_num;
    bool     is_merged_bank;
    // End of templating vars
    bool     hidden;

    // TODO: track overflow bytes and report them in graph
    list_type area_list;
} bank_item;

void area_manual_apply_queued(void);
bool area_manual_queue(char * arg_str);

int bank_calc_percent_free(bank_item * p_bank);
int bank_calc_percent_used(bank_item * p_bank);

uint32_t bank_areas_calc_used(bank_item *, uint32_t, uint32_t);

void banks_output_show_areas(bool do_show);
void banks_output_show_headers(bool do_show);
void banks_output_show_minigraph(bool do_show);
void banks_output_show_largegraph(bool do_show);

void banks_init(void);
void banks_cleanup(void);
void banks_init_templates(void);

void set_exit_error(void);
bool get_exit_error(void);

void banks_check(area_item area);
void banklist_finalize_and_show(void);

void bank_areas_split_to_buckets(bank_item * p_bank, uint32_t range_start, uint32_t range_size, uint32_t range_buckets, uint32_t * p_buckes);


#endif // _BANKS_H