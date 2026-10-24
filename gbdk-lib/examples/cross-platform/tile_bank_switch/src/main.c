//
// Multi-tile-bank image example for GB/GBC, SMS/GG and NES.
//
// This example for shows how an image can be split into multiple Tile Data banks when exceeding 256 tiles.
//
// A converted "upper" image is loaded to the first tile bank, and a converted "lower" image to the second.
//
// The method for display differs slightly depending on platform.
//
// On GBC and SMS/GG the attribute bit for selecting the second tile bank is set to either 0 or 1.
// This allows the graphics hardware to show up to 512 background tiles with no extra CPU processing.
//
// On NES and GB the attribute bit is not available, so a VBL and LCD ISR is used to switch tile banks mid-frame.
// On NES, this is done with mapper30 CHR-RAM bank-switching.
// On DMG, this is done by flipping the LCDCF_BG8000 bit in LCDC_REG.
//
// To keep the example similar on all platforms the image is of 160x144 resolution, and only two tile banks
// are used.
//
// However, it should be noted that the same technique can be used on NES/mapper30 to show a full-screen
// 256x240 image with multiple tile bank switches from repeated LCD interrupt events.
//

#include <gbdk/platform.h>

#include "zelda.h"

// Place image in middle of screen
const uint8_t start_x = (DEVICE_SCREEN_WIDTH - zelda_TILES_WIDTH) / 2;
const uint8_t start_y = (DEVICE_SCREEN_HEIGHT - zelda_TILES_HEIGHT) / 2;

// Height of upper image in rows.
// Target 256 total tiles, but rounded down to enforce integer height
const uint8_t tiles_height_upper = 256 / zelda_TILES_WIDTH;
const uint8_t tiles_height_lower = zelda_TILES_HEIGHT - (256 / zelda_TILES_WIDTH);

// Tile count in upper half of image
const uint8_t tile_count_upper = (256 / zelda_TILES_WIDTH) * zelda_TILES_WIDTH;

#if defined(NINTENDO)
// For GB we need to know the build (from define provided by Makefile) to know if ROM is built with attributes
#define SYSTEM_HAS_TILE_BANK_ATTRIBUTE SYSTEM_CGB
#else
// For all the other systems BKGF_BANK1 being non-zero indicates support for tile bank attribute
#define SYSTEM_HAS_TILE_BANK_ATTRIBUTE BKGF_BANK1
#endif

// if system does NOT have tile BANK0/1 bit in attributes, use VBL/LCD ISR to switch to tile bank 0/1
#if !SYSTEM_HAS_TILE_BANK_ATTRIBUTE
inline void switch_to_tile_bank(uint8_t tile_bank_number)
{
#if defined(NINTENDO_NES)
    SHOW_CHR_BANK(tile_bank_number ? VBK_BANK_1 : VBK_BANK_0);
#elif defined(NINTENDO) && !defined(SYSTEM_CGB)
    LCDC_REG = (LCDC_REG & ~LCDCF_BG8000) | (tile_bank_number ? LCDCF_BG8000 : 0);
#endif
}

void vbl_isr(void) 
{
    switch_to_tile_bank(0);
}

void lcd_isr(void) 
{
    switch_to_tile_bank(1);
}
#endif

void main(void) {
#if defined(SEGA)
    // Switch tilemap to overlap with sprite tile data,
    // to allow background tile data to use 256 tiles
    __WRITE_VDP_REG(VDP_R2, R2_MAP_0x3800);
    __WRITE_VDP_REG(VDP_R5, R5_SAT_0x3F00);
#endif

    DISPLAY_OFF;
    fill_bkg_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0);
    set_bkg_palette(0, zelda_PALETTE_COUNT, zelda_palettes);

    // Upload to tile bank0
    set_bkg_native_data(0, tile_count_upper, zelda_tiles);
    // Upload to tile bank1
#if defined(SYSTEM_CGB) || defined(SYSTEM_NES)
    // On CGB the 9th tile index bit selects a different tile bank that can be targeted with VBK_REG
    VBK_REG = VBK_BANK_1;
    set_bkg_native_data(0, zelda_TILE_COUNT - tile_count_upper, zelda_tiles + tile_count_upper * DEVICE_TILE_SIZE_BYTES);
    VBK_REG = VBK_BANK_0;
#elif defined(SYSTEM_SEGA) || defined(NINTENDO)
    // On SMS/GG and DMG the 9th tile index bit selects the sprite tile bank
    set_sprite_native_data(0, zelda_TILE_COUNT - tile_count_upper, zelda_tiles + tile_count_upper * DEVICE_TILE_SIZE_BYTES);
#endif

// if system has tile BANK0/1 bit in attributes, use them to switch between tile bank0 / tile bank1
#if SYSTEM_HAS_TILE_BANK_ATTRIBUTE
    // set upper image to source from tile bank0
    fill_bkg_rect_attributes(start_x, start_y, zelda_TILES_WIDTH, tiles_height_upper, BKGF_BANK0);
    // set lower image to source from tile bank1
    fill_bkg_rect_attributes(start_x, start_y + tiles_height_upper, zelda_TILES_WIDTH, tiles_height_lower, BKGF_BANK1);
#endif

    // Set tiles in upper half of image
    uint8_t i = 0;
    for(size_t y = 0; y < tiles_height_upper; y++)
    {
        for(size_t x = 0; x < zelda_TILES_WIDTH; x++)
        {
            set_bkg_tile_xy(start_x + x, start_y + y, i++);
        }
    }
    // Set tiles in lower half of image
    i = 0;
    for(size_t y = 0; y < tiles_height_lower; y++)
    {
        for(size_t x = 0; x < zelda_TILES_WIDTH; x++)
        {
            set_bkg_tile_xy(start_x + x, start_y + tiles_height_upper + y, i++);
        }
    }

#if !SYSTEM_HAS_TILE_BANK_ATTRIBUTE
    LYC_REG = ((start_y + tiles_height_upper)*8) - 1;
    // Enable scanline ISRs to switch tile banks on NES and GB
    CRITICAL {
        add_VBL(vbl_isr);
        add_LCD(lcd_isr);
    }
    set_interrupts(VBL_IFLAG | LCD_IFLAG);
#if defined(NINTENDO)
    STAT_REG |= STATF_LYC;
#endif
#endif

    SHOW_BKG;
    DISPLAY_ON;

    while(1)
    {
        vsync();
    }
}
