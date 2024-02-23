// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2022

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "banks_color.h"


#ifdef _WIN32
  #ifndef _WIN32
     #define __WIN32__
  #endif
#endif

// For GetConsoleMode()/etc..
#ifdef __WIN32__
    #include <windows.h>
#endif

#ifdef __WIN32__
// Enables Windows virtual terminal sequences for handling VT escape codes
// MS recommends this over SetConsoleTextAttribute()
bool colors_try_windows_enable_virtual_term_for_vt_codes(void) {

    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {

        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {

            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(hOut, dwMode)) {
                return true; // success
            }
        }
    }
    return false; // failure
}
#endif


// Default colots
color_pal_t bank_colors = {
    .default_color = PRINT_COLOR_DEFAULT,
    .rom     = PRINT_COLOR_ROM_DEFAULT,
    .vram    = PRINT_COLOR_VRAM_DEFAULT,
    .sram    = PRINT_COLOR_SRAM_DEFAULT,
    .wram    = PRINT_COLOR_WRAM_DEFAULT,
    .hram    = PRINT_COLOR_HRAM_DEFAULT,
};


static uint8_t bank_get_color(bank_item * p_bank) {

    uint8_t color_esc_code = bank_colors.default_color;

    if (get_option_percentage_based_color()) {

        int perc_free = bank_calc_percent_free(p_bank);

        // Color ramp from Default -> Green -> Yellow -> Red
        if      (perc_free == 0)  color_esc_code = VT_COLOR_RED_LIGHT;
        else if (perc_free <= 10) color_esc_code = VT_COLOR_YELLOW_LIGHT;
        else if (perc_free <= 25) color_esc_code = VT_COLOR_GREEN_LIGHT;
        // implied: else if (> 25) PRINT_COLOR_DEFAULT
    }
    else {
        switch (p_bank->bank_mem_type) {
            case BANK_MEM_TYPE_ROM:  color_esc_code = bank_colors.rom;  break;
            case BANK_MEM_TYPE_VRAM: color_esc_code = bank_colors.vram; break;
            case BANK_MEM_TYPE_SRAM: color_esc_code = bank_colors.sram; break;
            case BANK_MEM_TYPE_WRAM: color_esc_code = bank_colors.wram; break;
            case BANK_MEM_TYPE_HRAM: color_esc_code = bank_colors.hram; break;
            default: break;
        }
    }

    return color_esc_code;
}


// Write out the actual color/etc escape codes
static void write_out_color_code(uint8_t esc_num) {

    #ifdef __WIN32__

        // Instead of Dim/Dim Off VT100 codes Windows virtual terminal uses
        // Bright/Bright Off with basically inverted meaning for escape code 22.
        // So flip them:
        //   Dim ON  (2)  -> Bright OFF (22)
        //   Dim OFF (22) -> Bright ON  (1)
        if      (esc_num == VT_ATTR_DIM)       esc_num = WINCON_ATTR_BRIGHT_RESET;
        else if (esc_num == VT_ATTR_DIM_RESET) esc_num = WINCON_ATTR_BRIGHT;

        printf("\x1b[%dm", esc_num);
    #else // VT100 compatible
        printf("\x1b[%dm", esc_num);
    #endif
}


// Writes out a color escape code based on current printing region and bank type
void bank_render_color(bank_item * p_bank, int mode) {

    if (get_option_color_mode() != OPT_PRINT_COLOR_OFF) {

        switch(mode) {
            // Start row colorizing
            case PRINT_REGION_ROW_START:
                write_out_color_code(bank_get_color(p_bank));
                break;

            // Based on mode, either set to dim-color, turn off coloring, or continue coloring (do nothing)
            case PRINT_REGION_ROW_MIDDLE_START:
                if (get_option_color_mode() == OPT_PRINT_COLOR_WHOLE_ROW_DIMMED) write_out_color_code(VT_ATTR_DIM);
                else if (get_option_color_mode() == OPT_PRINT_COLOR_ROW_ENDS)    write_out_color_code(VT_ATTR_ALL_RESET);
                // implied: else ( == OPT_PRINT_COLOR_WHOLE_ROW) : Don't change attributes in middle
                break;

            // Based on mode, either turn off dim-color, turn coloring back on, or continue coloring (do nothing)
            case PRINT_REGION_ROW_MIDDLE_END:
                if (get_option_color_mode() == OPT_PRINT_COLOR_WHOLE_ROW_DIMMED) write_out_color_code(VT_ATTR_DIM_RESET);
                else if (get_option_color_mode() == OPT_PRINT_COLOR_ROW_ENDS)    write_out_color_code(bank_get_color(p_bank));
                // implied: else ( == OPT_PRINT_COLOR_WHOLE_ROW) : Don't change attributes in middle
                break;

            // Start end colorizing
            case PRINT_REGION_ROW_END:
                write_out_color_code(VT_ATTR_ALL_RESET);
                break;
        }
    }
}



#define MAX_SPLIT_WORDS 8
#define ARG_COLORS_CUSTOM_PAL 7u

// -sP : Custom Color Palette. Each colon separated entry is decimal VT100 color code
//       -sP:DEFAULT:ROM:VRAM:SRAM:WRAM:HRAM
//
// Custom color scheme for output
bool set_option_custom_bank_colors(char * arg_str) {

    char cols;
    char * p_str;
    char * p_words[MAX_SPLIT_WORDS];
    area_item area;

    // Split string into words separated by - and : chars
    cols = 0;
    p_str = strtok(arg_str,"-:");
    while (p_str != NULL)
    {
        p_words[cols++] = p_str;
        p_str = strtok(NULL, "-:");
        if (cols >= MAX_SPLIT_WORDS) break;
    }

    if (cols == ARG_COLORS_CUSTOM_PAL) {
        bank_colors.default_color = strtol(p_words[1], NULL, 10);
        bank_colors.rom     = strtol(p_words[2], NULL, 10);
        bank_colors.vram    = strtol(p_words[3], NULL, 10);
        bank_colors.sram    = strtol(p_words[4], NULL, 10);
        bank_colors.wram    = strtol(p_words[5], NULL, 10);
        bank_colors.hram    = strtol(p_words[6], NULL, 10);

        return true;
    } else
        return false; // Signal failure
}
