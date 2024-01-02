// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _COMMON_H
#define _COMMON_H

// #define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define DEFAULT_STR_LEN 100

#define OPT_AREA_SORT_DEFAULT    0
#define OPT_AREA_SORT_SIZE_DESC  1
#define OPT_AREA_SORT_ADDR_ASC   2
#define OPT_AREA_SORT_HIDE       3

#define OPT_INPUT_SRC_NONE 0
#define OPT_INPUT_SRC_CDB  1
#define OPT_INPUT_SRC_NOI  2
#define OPT_INPUT_SRC_MAP  3
#define OPT_INPUT_SRC_IHX  4
#define OPT_INPUT_SRC_ROM  5

#define OPT_AREA_HIDE_SIZE_DEFAULT 0

#define OPT_PRINT_COLOR_OFF              0
#define OPT_PRINT_COLOR_WHOLE_ROW        1
#define OPT_PRINT_COLOR_WHOLE_ROW_DIMMED 2
#define OPT_PRINT_COLOR_ROW_ENDS         3
#define OPT_PRINT_COLOR_DEFAULT          (OPT_PRINT_COLOR_WHOLE_ROW)


// Each flags should be a unique power of 2
#define OPT_MERGED_BANKS_NONE    0u
#define OPT_MERGED_BANKS_WRAM    1u
#define OPT_MERGED_BANKS_ROM     2u

#define OPT_PLAT_GAMEBOY     0u
#define OPT_PLAT_SMS_GG_GBDK 1u  // GBDK specific layout of sms/gg


extern bool banks_display_areas;
extern bool banks_display_headers;
extern bool banks_display_minigraph;
extern bool banks_display_largegraph;
extern bool option_compact_mode;
extern bool option_json_output;
extern bool option_summarized_mode;

extern unsigned int option_merged_banks;
extern unsigned int option_forced_display_max_bank_ROM;
extern unsigned int option_forced_display_max_bank_SRAM;

extern bool option_all_areas_exclusive;
extern bool option_quiet_mode;
extern bool option_suppress_duplicates;
extern bool option_error_on_warning;
extern unsigned int option_merged_banks;
// Use get_/set_() for these
// extern bool option_hide_banners;
// extern int  option_input_source;
// extern int  option_area_sort;
// extern int  option_color_mode;
extern uint32_t option_area_hide_size;
extern bool exit_error;


void set_option_all_areas_exclusive(bool value);
void set_option_quiet_mode(bool value);
void set_option_suppress_duplicates(bool value);
void set_option_error_on_warning(bool value);
void set_option_hide_banners(bool value);
void set_option_input_source(int value);
void set_option_area_sort(int value);
void set_option_color_mode(int value);
void set_option_percentage_based_color(bool value);
void set_option_area_hide_size(uint32_t value);
void set_option_platform(unsigned int);
void set_option_display_asciistyle(bool value);
void set_option_show_compact(bool value);
void set_option_show_json(bool value);
void set_option_summarized(bool value);

bool option_set_displayed_bank_range(char * arg_str);

void set_option_merged_banks(unsigned int value);

int  get_option_input_source(void);
int  get_option_area_sort(void);
int  get_option_color_mode(void);
bool get_option_percentage_based_color(void);
bool get_option_hide_banners(void);
unsigned int get_option_platform(void);
bool get_option_display_asciistyle(void);
uint32_t get_option_area_hide_size(void);

uint32_t round_up_power_of_2(uint32_t val);

uint32_t min(uint32_t a, uint32_t b);
uint32_t max(uint32_t a, uint32_t b);

#endif // _COMMON_H