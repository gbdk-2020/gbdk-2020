//
// options.c
//
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <options.h>

 // Use sequential tile order number (0->255) for map index instead of VRAM tile index (128->255->0->127)
bool opt_map_use_sequential_tile_index = true;
bool opt_tile_dedupe                   = true;
bool opt_c_file_output                 = false;
bool opt_c_precompiled                 = false;
int  opt_bank_num                      = BANK_NUM_UNSET;
bool opt_pal_end_bit                   = false;
bool opt_enable_pal_end_col            = false;
uint16_t pal_end_color                 = 0x0000u; // Black in BGR555

void opt_set_map_tile_order(bool newval) { opt_map_use_sequential_tile_index = newval; }
bool opt_get_map_tile_order(void)        { return opt_map_use_sequential_tile_index; }

void opt_set_tile_dedupe(bool newval) { opt_tile_dedupe = newval; }
bool opt_get_tile_dedupe(void)        { return opt_tile_dedupe; }

void opt_set_c_file_output(bool newval) { opt_c_file_output = newval; }
bool opt_get_c_file_output(void)        { return opt_c_file_output; }

void opt_set_bank_num(int newval) { opt_bank_num = newval; }
int opt_get_bank_num(void)        { return opt_bank_num; }

void opt_set_precompiled_palette(bool newval) { opt_c_precompiled = newval; }
bool opt_get_precompiled_palette(void)        { return opt_c_precompiled; }

void opt_set_pal_end_bit(bool newval) { opt_pal_end_bit = newval; }
bool opt_get_pal_end_bit(void)        { return opt_pal_end_bit; }

void opt_set_enable_pal_end_color(uint16_t end_color_bgr555) {

    opt_enable_pal_end_col = true;
    // Make sure unused high bit is not set for compat with pal_end_bit
    pal_end_color = (end_color_bgr555 & BGR555_MASK);
}

bool opt_get_enable_pal_end_color(void) {

    return opt_enable_pal_end_col;
}

void opt_load_pal_end_color(uint16_t * p_end_color, int * pal_end_color_count) {

    *p_end_color = pal_end_color;

    if (opt_enable_pal_end_col) {
        *pal_end_color_count = APPEND_END_COLOR_COUNT; // Add 8 Palettes x 4 Colors to palette data (2 bytes each)
    } else {
        *pal_end_color_count = 0; // Not enabled, so add zero bytes to pal size
    }
}
