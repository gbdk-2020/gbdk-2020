/** @file nes/hardware.h
    Defines that let the NES hardware registers be accessed
    from C.
*/
#ifndef _HARDWARE_H
#define _HARDWARE_H

#include <types.h>
#include <stdint.h>

#define __SHADOW_REG extern volatile uint8_t
#define __REG(addr) volatile __at (addr) uint8_t

__REG(0x2000) PPUCTRL;
#define PPUCTRL_NMI         0b10000000
#define PPUCTRL_SPR_8X8     0b00000000
#define PPUCTRL_SPR_8X16    0b00100000
#define PPUCTRL_BG_CHR      0b00010000
#define PPUCTRL_SPR_CHR     0b00001000
#define PPUCTRL_INC32       0b00000100
__SHADOW_REG shadow_PPUCTRL;

__REG(0x2001) PPUMASK;
#define PPUMASK_BLUE        0b10000000
#define PPUMASK_RED         0b01000000
#define PPUMASK_GREEN       0b00100000
#define PPUMASK_SHOW_SPR    0b00010000
#define PPUMASK_SHOW_BG     0b00001000
#define PPUMASK_SHOW_SPR_LC 0b00000100
#define PPUMASK_SHOW_BG_LC  0b00000010
#define PPUMASK_MONOCHROME  0b00000001
__SHADOW_REG shadow_PPUMASK;

__REG(0x2002) PPUSTATUS;
__REG(0x2003) OAMADDR;
__REG(0x2004) OAMDATA;
__REG(0x2005) PPUSCROLL;
__REG(0x2006) PPUADDR;
__REG(0x2007) PPUDATA;
__REG(0x4014) OAMDMA;

/* BKG attributes flags */
#define HW_HAS_BKG_ATTRIBUTES  1  /**< BKG attributes flags hardware support */
#define BKGF_PRI      0b00000000  /**< Background BG and Window over Sprite priority Enabled */
#define BKGF_YFLIP    0b00000000  /**< Background Y axis flip: Vertically mirrored */
#define BKGF_XFLIP    0b00000000  /**< Background X axis flip: Horizontally mirrored */
#define BKGF_BANK0    0b00000000  /**< Background Tile VRAM-Bank: Use Bank 0 */
#define BKGF_BANK1    0b00000000  /**< Background Tile VRAM-Bank: Use Bank 1 */

#define BKGF_CGB_PAL0 0b00000000  /**< Background Palette number (Color Mode Only) */
#define BKGF_CGB_PAL1 0b00000001  /**< Background Palette number (Color Mode Only) */
#define BKGF_CGB_PAL2 0b00000010  /**< Background Palette number (Color Mode Only) */
#define BKGF_CGB_PAL3 0b00000011  /**< Background Palette number (Color Mode Only) */
#define BKGF_CGB_PAL4 0b00000000  /**< Background Palette number (Color Mode Only) */
#define BKGF_CGB_PAL5 0b00000000  /**< Background Palette number (Color Mode Only) */
#define BKGF_CGB_PAL6 0b00000000  /**< Background Palette number (Color Mode Only) */
#define BKGF_CGB_PAL7 0b00000000  /**< Background Palette number (Color Mode Only) */
#define BKGF_PALMASK  0b00000011  /**< Mask for Background Palette number (Color Mode Only) */

/* OAM attributes flags */
#define HW_HAS_OAM_ATTRIBUTES  1  /**< OAM attributes flags hardware support */
#define OAMF_PRI      0b00100000  /**< BG and Window over Sprite Enabled */
#define OAMF_YFLIP    0b10000000  /**< Sprite Y axis flip: Vertically mirrored */
#define OAMF_XFLIP    0b01000000  /**< Sprite X axis flip: Horizontally mirrored */
#define OAMF_PAL0     0b00000000  /**< Sprite Palette number: use OBP0 (Mono Mode Only) */
#define OAMF_PAL1     0b00000000  /**< Sprite Palette number: use OBP1 (Mono Mode Only) */
#define OAMF_BANK0    0b00000000  /**< Sprite Tile VRAM-Bank: Use Bank 0 */
#define OAMF_BANK1    0b00000000  /**< Sprite Tile VRAM-Bank: Use Bank 1 */

#define OAMF_CGB_PAL0 0b00000000  /**< Sprite CGB Palette number: use OCP0 (Color Mode Only) */
#define OAMF_CGB_PAL1 0b00000001  /**< Sprite CGB Palette number: use OCP1 (Color Mode Only) */
#define OAMF_CGB_PAL2 0b00000010  /**< Sprite CGB Palette number: use OCP2 (Color Mode Only) */
#define OAMF_CGB_PAL3 0b00000011  /**< Sprite CGB Palette number: use OCP3 (Color Mode Only) */
#define OAMF_CGB_PAL4 0b00000000  /**< Sprite CGB Palette number: use OCP4 (Color Mode Only) */
#define OAMF_CGB_PAL5 0b00000000  /**< Sprite CGB Palette number: use OCP5 (Color Mode Only) */
#define OAMF_CGB_PAL6 0b00000000  /**< Sprite CGB Palette number: use OCP6 (Color Mode Only) */
#define OAMF_CGB_PAL7 0b00000000  /**< Sprite CGB Palette number: use OCP7 (Color Mode Only) */
#define OAMF_PALMASK  0b00000011  /**< Mask for Sprite CGB Palette number (Color Mode Only) */

#define DEVICE_SCREEN_X_OFFSET 0
#define DEVICE_SCREEN_Y_OFFSET 0
#define DEVICE_SCREEN_WIDTH 32
#define DEVICE_SCREEN_HEIGHT 30

#if defined(NES_TILEMAP_F)
// Full tilemap
#define DEVICE_SCREEN_BUFFER_WIDTH 64
#define DEVICE_SCREEN_BUFFER_HEIGHT 60
typedef uint16_t scroll_x_t;
typedef uint16_t scroll_y_t;
#elif defined(NES_TILEMAP_H)
// Horizontally arranged tilemap
#define DEVICE_SCREEN_BUFFER_WIDTH 64
#define DEVICE_SCREEN_BUFFER_HEIGHT 30
typedef uint16_t scroll_x_t;
typedef uint8_t scroll_y_t;
#elif defined(NES_TILEMAP_V)
// Vertically arranged tilemap
#define DEVICE_SCREEN_BUFFER_WIDTH 32
#define DEVICE_SCREEN_BUFFER_HEIGHT 60
typedef uint8_t scroll_x_t;
typedef uint16_t scroll_y_t;
#else
// Single-screen tilemap
#define DEVICE_SCREEN_BUFFER_WIDTH 32
#define DEVICE_SCREEN_BUFFER_HEIGHT 30
typedef uint8_t scroll_x_t;
typedef uint8_t scroll_y_t;
#endif

#define DEVICE_SCREEN_MAP_ENTRY_SIZE 1
#define DEVICE_SPRITE_PX_OFFSET_X 0
#define DEVICE_SPRITE_PX_OFFSET_Y -1
#define DEVICE_WINDOW_PX_OFFSET_X 0
#define DEVICE_WINDOW_PX_OFFSET_Y 0
#define DEVICE_SCREEN_PX_WIDTH (DEVICE_SCREEN_WIDTH * 8)
#define DEVICE_SCREEN_PX_HEIGHT (DEVICE_SCREEN_HEIGHT * 8)

// Scrolling coordinates (will be written to PPUSCROLL at end-of-vblank by NMI handler)
__SHADOW_REG bkg_scroll_x;
__SHADOW_REG bkg_scroll_y;
// LCD scanline - a software-driven version of GB's incrasing 'LY' scanline counter
__SHADOW_REG _lcd_scanline;

extern volatile UBYTE TIMA_REG;
extern volatile UBYTE TMA_REG;
extern volatile UBYTE TAC_REG;

// Compatibility defines for GB LY / LYC registers, to allow easier LCD ISR porting
#define SCY_REG bkg_scroll_y    /**< Scroll Y */
#define rSCY SCY_REG
#define SCX_REG bkg_scroll_x    /**< Scroll X */
#define rSCX SCX_REG
#define LY_REG _lcd_scanline    /**< LCDC Y-coordinate */
#define rLY LY_REG
#define LYC_REG _lcd_scanline   /**< LY compare */
#define rLYC LYC_REG

#if defined(NES_WINDOW_LAYER)
__SHADOW_REG win_pos_x;
__SHADOW_REG win_pos_y;
#define WX_REG win_pos_x
#define WY_REG win_pos_y
#endif

#endif
