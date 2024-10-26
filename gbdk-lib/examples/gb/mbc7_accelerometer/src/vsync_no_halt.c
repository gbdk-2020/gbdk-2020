#include <gbdk/platform.h>

void vsync_no_halt(void) {

    // Return if the screen isn't on since
    // VBL isr to toggle vbl_done won't run
    if ((LCDC_REG & LCDCF_ON) == 0) return;

    // Wait until VBlank ISR marks the frame done
    VBL_DONE = 0;
    while (VBL_DONE == 0);
}
