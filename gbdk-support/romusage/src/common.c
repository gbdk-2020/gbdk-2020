// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "common.h"
#include "logging.h"
#include "rom_file.h"

bool banks_display_areas        = false;
bool banks_display_headers      = false;
bool banks_display_minigraph    = false;
bool banks_display_largegraph   = false;
bool option_compact_mode        = false;
bool option_json_output         = false;
bool option_summarized_mode     = false;

// -B
unsigned int option_merged_banks = OPT_MERGED_BANKS_NONE;
// -F
unsigned int option_forced_display_max_bank_ROM = 0;
unsigned int option_forced_display_max_bank_SRAM = 0;

unsigned int option_platform    = OPT_PLAT_GAMEBOY;
bool option_display_asciistyle  = false;
bool option_all_areas_exclusive = false;
bool option_quiet_mode          = false;
bool option_suppress_duplicates = true;
bool option_error_on_warning    = false;
bool option_hide_banners        = false;
int  option_input_source        = OPT_INPUT_SRC_NONE;
int  option_area_sort           = OPT_AREA_SORT_DEFAULT;
int  option_color_mode          = OPT_PRINT_COLOR_OFF;
bool option_percentage_based_color = false;
uint32_t option_area_hide_size  = OPT_AREA_HIDE_SIZE_DEFAULT;
bool exit_error                 = false;

int  banks_hide_count = 0;
char banks_hide_list[BANKS_HIDE_SZ][DEFAULT_STR_LEN];


// Turn on/off display of areas within bank
void banks_output_show_areas(bool do_show) {
    banks_display_areas = do_show;
}

// Turn on/off display of areas within bank
void banks_output_show_headers(bool do_show) {
    banks_display_headers = do_show;
}

// Turn on/off display of mini usage graph per bank
void banks_output_show_minigraph(bool do_show) {
    banks_display_minigraph = do_show;
}

// Turn on/off display of large usage graph per bank
void banks_output_show_largegraph(bool do_show) {
    banks_display_largegraph = do_show;
}

// Turn on/off compact display mode
void set_option_show_compact(bool value) {
    option_compact_mode = value;
}

// Turn on/off JSON output mode
void set_option_show_json(bool value) {
    option_json_output = value;
}

// Turn on/off brief / summarized mode for banked regions
void set_option_summarized(bool value) {
    option_summarized_mode = value;
}

// Turns on merged WRAM_0 + WRAM_1 display
void set_option_merged_banks(unsigned int value) {
    option_merged_banks |= value;
}

// Sets console platform (changes memory map templates)
void set_option_platform(unsigned int value) {
    option_platform = value;
}

// Turn on/off whether to use ascii style
// block characters for graphs
void set_option_display_asciistyle(bool value) {
    option_display_asciistyle = value;
}

// Turn on/off whether all areas are exclusive,
// and whether to warn for any overlap
void set_option_all_areas_exclusive(bool value) {
    option_all_areas_exclusive = value;
}

// Turn on/off quiet mode
void set_option_quiet_mode(bool value) {
    option_quiet_mode = value;
}

// Turn on/off suppression of duplicates
void set_option_suppress_duplicates(bool value) {
    option_suppress_duplicates = value;
}

// Turn on/off setting an error on exit for serious warnings encountered
void set_option_error_on_warning(bool value) {
    option_error_on_warning = value;
}

// Turn on/off banners
void set_option_hide_banners(bool value) {
    option_hide_banners = value;
}

// Input source file format
void set_option_input_source(int value) {
    option_input_source = value;
}

// Area output sort order
void set_option_area_sort(int value) {
    option_area_sort = value;
}

// Color output mode
void set_option_color_mode(int value) {
    option_color_mode = value;
}

// Use Percentage based color
// Turns on color mode to default if not enabled
void set_option_percentage_based_color(bool value) {
    option_percentage_based_color = value;

    if (get_option_color_mode() == OPT_PRINT_COLOR_OFF)
        set_option_color_mode(OPT_PRINT_COLOR_DEFAULT);
}

// Hide areas smaller than size
void set_option_area_hide_size(uint32_t value) {
    option_area_hide_size = value;
}


// -sP : Custom Color Palette. Each colon separated entry is decimal VT100 color code
//       -sP:DEFAULT:ROM:VRAM:SRAM:WRAM:HRAM
//
// Custom color scheme for output
bool set_option_displayed_bank_range(char * arg_str) {

    #define MAX_SPLIT_WORDS 4
    #define EXPECTED_COLS   3

    char cols;
    char * p_str;
    char * p_words[MAX_SPLIT_WORDS];

    // Split string into words separated by - and : chars
    cols = 0;
    p_str = strtok(arg_str,"-:");
    while (p_str != NULL)
    {
        p_words[cols++] = p_str;
        p_str = strtok(NULL, "-:");
        if (cols >= MAX_SPLIT_WORDS) break;
    }

    if (cols == EXPECTED_COLS) {
        option_forced_display_max_bank_ROM = strtol(p_words[1], NULL, 10);
        option_forced_display_max_bank_SRAM = strtol(p_words[2], NULL, 10);
        // printf("2:%s\n", p_words[2]);
        return true;
    } else
        return false; // Signal failure
}



// Input source file format
int get_option_input_source(void) {
    return option_input_source;
}

// Area output sort order
int get_option_area_sort(void) {
    return option_area_sort;
}

// Color output mode
int get_option_color_mode(void) {
    return option_color_mode;
}

// Use Percentage based color
bool get_option_percentage_based_color(void) {
    return option_percentage_based_color;
}

// Turn on/off banners
bool get_option_hide_banners(void) {
    return option_hide_banners;
}

// Hide areas smaller than size
uint32_t  get_option_area_hide_size(void) {
    return option_area_hide_size;
}

// Current platform (used for changing memory map templates)
unsigned int get_option_platform(void) {
    return option_platform;
}

// Turn on/off whether to use ascii style
// block characters for graphs
bool get_option_display_asciistyle(void) {
    return option_display_asciistyle;
}


// Add a substring for hiding banks
bool set_option_banks_hide_add(char * str_bank_hide_substring) {

    if (banks_hide_count < BANKS_HIDE_SZ) {
        snprintf(banks_hide_list[banks_hide_count], (DEFAULT_STR_LEN - 1), "%s", str_bank_hide_substring);
        banks_hide_count++;
        return true;
    } else
        log_error("Error: no bank hide string slots available\n");


    return false;
}


// Set hex bytes treated as Empty in ROM files (.gb/etc) -b:HEXVAL:HEXVAL...
//       -b:FF
// Value passed in has "-e" stripped off the front
bool set_option_binary_rom_empty_values(char * arg_str) {

    #define MAX_ROMFILE_ENTRIES     256
    #define MIN_EXPECTED_ENTRIES   1

    char entries_found;
    char * p_str;
    char * p_words[MAX_ROMFILE_ENTRIES];

    // Clear existing defaults
    romfile_empty_value_table_clear();

    // Split string into words separated by : chars
    entries_found = 0;
    p_str = strtok(arg_str,":");
    while (p_str != NULL)
    {
        p_words[entries_found++] = p_str;
        p_str = strtok(NULL, "-:");
        if (entries_found >= MAX_ROMFILE_ENTRIES) break;
    }

    for (int c = 0; c < entries_found; c++)
        romfile_empty_value_table_add_entry( (uint8_t)strtol(p_words[c], NULL, 16) );

    if (entries_found >= MIN_EXPECTED_ENTRIES)
        return true;
    else
        return false; // Signal failure
}


void set_exit_error(void) {
    exit_error = true;
}

bool get_exit_error(void) {
    return exit_error;
}


uint32_t round_up_power_of_2(uint32_t val) {

    val--;
    val |= val >> 1;
    val |= val >> 2;
    val |= val >> 4;
    val |= val >> 8;
    val |= val >> 16;
    val++;

    return val;
}


uint32_t min(uint32_t a, uint32_t b) {
    return (a < b) ? a : b;
}


uint32_t max(uint32_t a, uint32_t b) {
    return (a > b) ? a : b;
}
