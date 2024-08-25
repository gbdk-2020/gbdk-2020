#pragma once
#include <vector>
#include "process_arguments.h"
#include "cmp_int_color.h"
using namespace std;
unsigned char GetMapAttribute(size_t x, size_t y);
void ReduceMapAttributes2x2(PNG2AssetData* assetData);
void AlignMapAttributes(PNG2AssetData* assetData);
void PackMapAttributes(PNG2AssetData* assetData);
void HandleMapAttributes( PNG2AssetData* assetData);


 enum {
    CGB_BKGF_PRI      = 0b10000000,  /**< Background CGB BG and Window over Sprite priority Enabled */
    CGB_BKGF_YFLIP    = 0b01000000,  /**< Background CGB Y axis flip: Vertically mirrored */
    CGB_BKGF_XFLIP    = 0b00100000,  /**< Background CGB X axis flip: Horizontally mirrored */
    CGB_BKGF_BANK0    = 0b00000000,  /**< Background CGB Tile VRAM-Bank: Use Bank 0 (CGB Mode Only) */
    CGB_BKGF_BANK1    = 0b00001000,  /**< Background CGB Tile VRAM-Bank: Use Bank 1 (CGB Mode Only) */

    CGB_BKGF_CGB_PAL0 = 0b00000000,  /**< Background CGB Palette number (CGB Mode Only) */
    CGB_BKGF_CGB_PAL1 = 0b00000001,  /**< Background CGB Palette number (CGB Mode Only) */
    CGB_BKGF_CGB_PAL2 = 0b00000010,  /**< Background CGB Palette number (CGB Mode Only) */
    CGB_BKGF_CGB_PAL3 = 0b00000011,  /**< Background CGB Palette number (CGB Mode Only) */
    CGB_BKGF_CGB_PAL4 = 0b00000100,  /**< Background CGB Palette number (CGB Mode Only) */
    CGB_BKGF_CGB_PAL5 = 0b00000101,  /**< Background CGB Palette number (CGB Mode Only) */
    CGB_BKGF_CGB_PAL6 = 0b00000110,  /**< Background CGB Palette number (CGB Mode Only) */
    CGB_BKGF_CGB_PAL7 = 0b00000111,  /**< Background CGB Palette number (CGB Mode Only) */
};