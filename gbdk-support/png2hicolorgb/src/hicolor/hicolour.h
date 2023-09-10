#ifndef __hicolour_h__
#define __hicolour_h__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <defines.h>

#include <image_info.h>
#include <logging.h>

// For displaying available conversion patterns to the user
#define HELP_CONV_PATTERN_STR \
    "Available conversion palette/attribute pattern widths for -cL:N and -cR:N\n" \
    "\n" \
    "- Default for Left and Right is 3\n" \
    "- Fixed pattern widths are faster but less optimal than adaptive\n" \
    "- Adaptive pattern width speeds are 1: fastest, 2: medium, 3: slowest (slower = higher quality)\n" \
    "\n" \
    "  0: Adaptive-1   1: Adaptive-2   2: Adaptive-3   3: 3-2-3-2      4: 2-3-2-3 \n" \
    "  5: 2-2-3-3      6: 2-3-3-2      7: 3-2-2-3      8: 3-3-2-2      9: 4-2-2-2 \n" \
    " 10: 2-2-2-4     11: 2-2-4-2     12: 2-4-2-2     13: 1-1-2-6     14: 1-1-3-5 \n" \
    " 15: 1-1-4-4     16: 1-1-5-3     17: 1-1-6-2     18: 1-2-1-6     19: 1-2-2-5 \n" \
    " 20: 1-2-3-4     21: 1-2-4-3     22: 1-2-5-2     23: 1-2-6-1     24: 1-3-1-5 \n" \
    " 25: 1-3-2-4     26: 1-3-3-3     27: 1-3-4-2     28: 1-3-5-1     29: 1-4-1-4 \n" \
    " 30: 1-4-2-3     31: 1-4-3-2     32: 1-4-4-1     33: 1-5-1-3     34: 1-5-2-2 \n" \
    " 35: 1-5-3-1     36: 1-6-1-2     37: 1-6-2-1     38: 2-1-1-6     39: 2-1-2-5 \n" \
    " 40: 2-1-3-4     41: 2,1,4,3     42: 2-1-5-2     43: 2-1-6-1     44: 2-2-1-5 \n" \
    " 45: 2-2-5-1     46: 2-3-1-4     47: 2-3-4-1     48: 2-4-1-3     49: 2-4-3-1 \n" \
    " 50: 2-5-1-2     51: 2-5-2-1     52: 2-6-1-1     53: 3-1-1-5     54: 3-1-2-4 \n" \
    " 55: 3-1-3-3     56: 3-1-4-2     57: 3-1-5-1     58: 3-2-1-4     59: 3-2-4-1 \n" \
    " 60: 3-3-1-3     61: 3-3-3-1     62: 3-4-1-2     63: 3-4-2-1     64: 3-5-1-1 \n" \
    " 65: 4-1-1-4     66: 4-1-2-3     67: 4-1-3-2     68: 4-1-4-1     69: 4-2-1-3 \n" \
    " 70: 4-2-3-1     71: 4-3-1-2     72: 4-3-2-1     73: 4-4-1-1     74: 5-1-1-3 \n" \
    " 75: 5-1-2-2     76: 5-1-3-1     77: 5-2-1-2     78: 5-2-2-1     79: 5-3-1-1 \n" \
    " 80: 6-1-1-2     81: 6-1-2-1     82: 6-2-1-1                                 \n"


extern    u8               TileOffset[4];               // Offset into screen for attribute start
extern    u8               TileWidth[4];                // No of character attributes width
extern    u8               SplitData[80][4];

extern    RGBQUAD          GBView;

void hicolor_init(void);

void hicolor_set_type(uint8_t new_value);
void hicolor_set_convert_left_pattern(uint8_t new_value);
void hicolor_set_convert_right_pattern(uint8_t new_value);

void hicolor_process_image(image_data * p_decoded_image, const char * fname);

/////////////////////////////////////////////////////////////////////////////

RGBQUAD translate(uint8_t rgb[3]);
unsigned int ImageRating(u8 *src, u8 *dest, int StartX, int StartY, int Width, int Height);
void ConvertToHiColor(int ConvertType);
int ConvertRegions(int StartX, int Width, int StartY, int Height, int StartJ, int FinishJ, int ConvertType);


#endif
