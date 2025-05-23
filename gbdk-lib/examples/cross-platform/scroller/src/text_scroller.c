#include <gbdk/platform.h>
#include <gbdk/font.h>
#include <stdint.h>
#include <stdio.h>

// Positive 1 in valid y scroll coordinates
#define P1 1
// Negative 1 in valid y scroll coordinates
#define N1 (uint8_t)(DEVICE_SCREEN_BUFFER_HEIGHT*8 - 1)

const uint8_t shake_tbl[] = {0, P1, P1, P1, 0, N1, N1, N1};
const uint8_t scanline_offsets_tbl[] = {0, 1, 2, 3, 3, 2, 1, 0, 0, 1, 2, 3, 3, 2, 1, 0};
const uint8_t * scanline_offsets = scanline_offsets_tbl;

#define SCROLL_POS 15
#define SCROLL_POS_PIX_START ((SCROLL_POS + DEVICE_SCREEN_Y_OFFSET) * 8) - 2
#define SCROLL_POS_PIX_END ((SCROLL_POS + DEVICE_SCREEN_Y_OFFSET + 1) * 8) - 1

uint8_t scroller_x = 0;
void scanline_isr(void) {

#if defined(NINTENDO) || defined(NINTENDO_NES)
    switch (LYC_REG) {
        case 0: 
            SCX_REG = 0;
            SCY_REG = 0;
            LYC_REG = SCROLL_POS_PIX_START;
            break;
        case SCROLL_POS_PIX_START:
            SCX_REG = scroller_x;
            SCY_REG = shake_tbl[(scroller_x >> 1) & 7];
            LYC_REG = SCROLL_POS_PIX_END;
            break;
        case SCROLL_POS_PIX_END:
            SCX_REG = SCY_REG = LYC_REG = 0;
            break;
    }
#elif defined(SEGA)
    if (VCOUNTER == (SCROLL_POS_PIX_START - 8)) {
        while (VCOUNTER != SCROLL_POS_PIX_START);
        VDP_CMD = -scroller_x; VDP_CMD = VDP_RSCX;
        while (VCOUNTER != SCROLL_POS_PIX_START + 8);
    } else {
        VDP_CMD = 0; VDP_CMD = VDP_RSCX;
    }
#endif
}

const uint8_t scroller_text[] = "This is a text scroller demo for GBDK-2020. You can use ideas, that are "\
"shown in this demo, to make different parallax effects, scrolling of tilemaps which are larger than 32x32 "\
"tiles and TEXT SCROLLERS, of course! Need to write something else to make this text longer than 256 characters. "\
"The quick red fox jumps over the lazy brown dog. 0123456789.          ";

const uint8_t * scroller_next_char = scroller_text;
uint8_t * scroller_vram_addr;
uint8_t * base, * limit;

void main(void) {
    DISPLAY_OFF;
    // Init font system / clear screen
    font_init();
    font_set(font_load(font_ibm));
    // Fill the screen background with '*'
    fill_bkg_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, '*' - ' ');
    DISPLAY_ON;

    printf(" Scrolling %d chars", sizeof(scroller_text) - 1);

    CRITICAL {
        add_LCD(scanline_isr);
#if defined(NINTENDO)
        STAT_REG |= STATF_LYC; LYC_REG = 0;
#endif
    }
#if defined(SEGA)
    __WRITE_VDP_REG(VDP_R10, 0x07);
#endif
#if defined(NINTENDO) || defined(NINTENDO_NES) || defined(SEGA)
    set_interrupts(VBL_IFLAG | LCD_IFLAG);
#endif
    HIDE_LEFT_COLUMN;    
    base = (uint8_t *)((uint16_t)get_bkg_xy_addr(0, SCROLL_POS) & (DEVICE_SCREEN_MAP_ENTRY_SIZE==1?0xffe0:0xffc0));
    limit = base + (DEVICE_SCREEN_BUFFER_WIDTH * DEVICE_SCREEN_MAP_ENTRY_SIZE);

    scroller_vram_addr = base + ((DEVICE_SCREEN_X_OFFSET + DEVICE_SCREEN_WIDTH) * DEVICE_SCREEN_MAP_ENTRY_SIZE);
    if (scroller_vram_addr >= limit) scroller_vram_addr = base;
    
    set_vram_byte(scroller_vram_addr, *scroller_next_char - 0x20);
    
    while (1) {
        scroller_x++;
        if ((scroller_x & 0x07) == 0) {
            // next letter
            scroller_next_char++;
            if (*scroller_next_char == 0) scroller_next_char = scroller_text;
            
            // next vram position
            scroller_vram_addr += DEVICE_SCREEN_MAP_ENTRY_SIZE;
            if (scroller_vram_addr >= limit) scroller_vram_addr = base;
            
            // put next char
            set_vram_byte(scroller_vram_addr, *scroller_next_char - 0x20);
        }
        vsync();        
    }
}