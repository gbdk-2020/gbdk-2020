

#ifndef __defines_h__
#define __defines_h__

#include <stdint.h>

#include <common.h>

// Originates on Windows format is BGRX24
// TODO: is this right & ok for what code expects?
typedef struct tagRGBQUAD {
  uint8_t rgbBlue;
  uint8_t rgbGreen;
  uint8_t rgbRed;
  uint8_t rgbReserved;
} RGBQUAD;

#define CGB_ATTR_TILES_BANK_0   0x00u
#define CGB_ATTR_TILES_BANK_1   0x08u
#define CGB_ATTR_TILES_BANK     0x08u
#define CGB_ATTR_NO_FLIP        0x00u
#define CGB_ATTR_VFLIP          0x40u
#define CGB_ATTR_HFLIP          0x20u
#define CGB_ATTR_PALETTES_ONLY     0x07u
#define CGB_ATTR_WITHOUT_PALETTES  (~(CGB_ATTR_PALETTES_ONLY))

#define CGB_TILES_START_BANK_0  0u
#define CGB_TILES_START_BANK_1  256u

#define RGB_SZ   3    // RGB888 size in bytes
#define TILE_SZ  16u  // Tile size in bytes: (8 x 8) pixels x 2 Bits per pixel

#define u8	uint8_t
#define u16	uint16_t
#define u32 uint32_t
#define s8	int8_t
#define s16 int16_t
#define s32 int32_t

enum conversion_types {
    CONV_TYPE_MED_CUT_NO_DITHER = 1,
    CONV_TYPE_MED_CUT_YES_DITHER = 2,
    CONV_TYPE_WU = 3,

    CONV_TYPE_MIN  = CONV_TYPE_MED_CUT_NO_DITHER,
    CONV_TYPE_MAX  = CONV_TYPE_WU
};

enum pattern_types {
    HICOLOR_PATTERN_ADAPTIVE_FAST = 0,
    HICOLOR_PATTERN_ADAPTIVE_MED  = 1,
    HICOLOR_PATTERN_ADAPTIVE_BEST = 2,

    HICOLOR_PATTERN_ADAPTIVE_COUNT = 3,
    HICOLOR_PATTERN_FIXED_COUNT    = 80,
    HICOLOR_PATTERN_MIN = 0,
    HICOLOR_PATTERN_MAX = (HICOLOR_PATTERN_FIXED_COUNT - 1),

    HICOLOR_PATTERN_OPT_MIN = 0,
    HICOLOR_PATTERN_OPT_MAX = ((HICOLOR_PATTERN_FIXED_COUNT + HICOLOR_PATTERN_ADAPTIVE_COUNT) - 1),

    HICOLOR_PATTERN_NOT_FOUND           = 998,
    HICOLOR_PATTERN_NOT_FOUND_HAS_CHARS = 999
};

#define CONV_SIDE_LEFT  0
#define CONV_SIDE_RIGHT 1

#define CONV_Y_SHIFT_UP_1    1
#define CONV_Y_SHIFT_NO      0

#define SM83_OPCODE_HALT        0x76u
#define SM83_OPCODE_LD_HL_B     0x70u
#define SM83_OPCODE_LD_HL_C     0x71u
#define SM83_OPCODE_LD_HL_D     0x72u
#define SM83_OPCODE_LD_HL_E     0x73u
#define SM83_OPCODE_LD_HL_IMM8  0x36u
#define SM83_OPCODE_RET         0xC9u

#define PAL_REGION_HEIGHT_PX 2

// #define BUF_WIDTH      160      // Originally 160
// #define BUF_WIDTH_TILE_MAX      (BUF_WIDTH / TILE_WIDTH_PX)
#define BUF_HEIGHT                  256      // Originally 144
#define BUF_HEIGHT_IN_TILES         (BUF_HEIGHT / TILE_HEIGHT_PX)
#define BUF_HEIGHT_IN_TILES_RNDUP   (BUF_HEIGHT_IN_TILES+1)        // Use larger size[side] for rounded up amount
#define BUF_Y_REGION_COUNT_LR_RNDUP (((BUF_HEIGHT / PAL_REGION_HEIGHT_PX) + 1))

#define VALIDATE_WIDTH  160 // (BUF_WIDTH)
#define VALIDATE_HEIGHT (BUF_HEIGHT)

#define BYTES_PER_COLOR 2
#define COLORS_PER_PAL  4
#define PALS_PER_SIDE  4


#endif
