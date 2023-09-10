// "options.h"

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h>
#include <stdbool.h>

#define BANK_NUM_UNSET 0
#define BANK_NUM_MIN   1
#define BANK_NUM_MAX   511

#define OPTION_UNSET 0xFFFF

#define OPT_MAP_TILE_ORDER_BY_VRAM_ID  false
#define OPT_MAP_TILE_SEQUENTIAL_ORDER  true

void opt_set_map_tile_order(bool newval);
bool opt_get_map_tile_order(void);

void opt_set_tile_dedupe(bool newval);
bool opt_get_tile_dedupe(void);

void opt_set_c_file_output(bool newval);
bool opt_get_c_file_output(void);

void opt_set_bank_num(int newval);
int opt_get_bank_num(void);

#endif

