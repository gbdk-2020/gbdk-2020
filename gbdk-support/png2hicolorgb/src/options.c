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
int  opt_bank_num                      = BANK_NUM_UNSET;

void opt_set_map_tile_order(bool newval) { opt_map_use_sequential_tile_index = newval; }
bool opt_get_map_tile_order(void)        { return opt_map_use_sequential_tile_index; }

void opt_set_tile_dedupe(bool newval) { opt_tile_dedupe = newval; }
bool opt_get_tile_dedupe(void)        { return opt_tile_dedupe; }

void opt_set_c_file_output(bool newval) { opt_c_file_output = newval; }
bool opt_get_c_file_output(void)        { return opt_c_file_output; }

void opt_set_bank_num(int newval) { opt_bank_num = newval; }
int opt_get_bank_num(void)        { return opt_bank_num; }

