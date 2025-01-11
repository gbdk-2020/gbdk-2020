// "options.h"

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

#define BANK_NUM_UNSET 0
#define BANK_NUM_MIN   1
#define BANK_NUM_MAX   511

#define OPTION_UNSET 0xFFFF

#define BGR555_MASK 0x7FFFu
#define APPEND_END_COLOR_COUNT (8 * 4); // Add 8 Palettes x 4 Colors each x 2 bytes per color to pal size

#define OPT_MAP_TILE_ORDER_BY_VRAM_ID  false
#define OPT_MAP_TILE_SEQUENTIAL_ORDER  true

void opt_set_map_tile_order(bool newval);
bool opt_get_map_tile_order(void);

void opt_set_tile_dedupe(bool newval);
bool opt_get_tile_dedupe(void);

void opt_set_c_file_output(bool newval);
bool opt_get_c_file_output(void);

void opt_set_precompiled_palette(bool newval);
bool opt_get_precompiled_palette(void);

void opt_set_pal_end_bit(bool newval);
bool opt_get_pal_end_bit(void);

void opt_set_enable_pal_end_color(uint16_t end_color_bgr555);
bool opt_get_enable_pal_end_color(void);
void opt_load_pal_end_color(uint16_t * p_end_color, int * pal_end_color_count);

void opt_set_bank_num(int newval);
int opt_get_bank_num(void);

#endif

