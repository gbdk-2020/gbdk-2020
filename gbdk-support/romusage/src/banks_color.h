// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2022


#ifndef _BANKS_COLOR_H
#define _BANKS_COLOR_H

#include "banks.h"


// VT100 color codes
// https://misc.flogisoft.com/bash/tip_colors_and_formatting

// Windows Console Virtual Terminal Sequences
// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

#define VT_ATTR_ALL_RESET       0
#define VT_ATTR_DIM             2
#define VT_ATTR_DIM_RESET      22

// 0   Default     Returns all attributes to the default state prior to modification
// 1   Bold/Bright     Applies brightness/intensity flag to foreground color
// 22  No bold/bright  Removes brightness/intensity flag from foreground color
#define WINCON_ATTR_BRIGHT         1
#define WINCON_ATTR_BRIGHT_RESET  22

#define VT_COLOR_GREY_LIGHT    37
#define VT_COLOR_RED_LIGHT     91
#define VT_COLOR_GREEN_LIGHT   92
#define VT_COLOR_YELLOW_LIGHT  93
#define VT_COLOR_BLUE_LIGHT    94
#define VT_COLOR_MAGENTA_LIGHT 95
#define VT_COLOR_CYAN_LIGHT    96


#define VT_COLOR_GREY_DARK     90
#define VT_COLOR_RED_DARK      31
#define VT_COLOR_GREEN_DARK    32
#define VT_COLOR_YELLOW_DARK   33
#define VT_COLOR_BLUE_DARK     34
#define VT_COLOR_MAGENTA_DARK  35
#define VT_COLOR_CYAN_DARK     36

// Note: The preset Default color just uses the "reset to no attributes" code
//       which should revert to the user's default terminal font color.
//       -> Using VT_COLOR_GREY_LIGHT is also an option
#define PRINT_COLOR_DEFAULT      (VT_ATTR_ALL_RESET)
#define PRINT_COLOR_ROM_DEFAULT  (VT_COLOR_GREEN_LIGHT)
#define PRINT_COLOR_VRAM_DEFAULT (PRINT_COLOR_DEFAULT)
#define PRINT_COLOR_WRAM_DEFAULT (VT_COLOR_MAGENTA_LIGHT)
#define PRINT_COLOR_SRAM_DEFAULT (VT_COLOR_YELLOW_LIGHT)
#define PRINT_COLOR_HRAM_DEFAULT (VT_COLOR_CYAN_LIGHT)

typedef enum {
    PRINT_REGION_ROW_START,
    PRINT_REGION_ROW_MIDDLE_START,
    PRINT_REGION_ROW_MIDDLE_END,
    PRINT_REGION_ROW_END,
} bank_print_regions;

typedef struct color_pal_t {
    uint8_t default_color;
    uint8_t rom;
    uint8_t vram;
    uint8_t sram;
    uint8_t wram;
    uint8_t hram;
} color_pal_t;

bool colors_try_windows_enable_virtual_term_for_vt_codes(void);

void bank_render_color(bank_item * p_bank, int mode);

bool set_option_custom_bank_colors(char * arg_str);

#endif // _BANKS_PRINT_H