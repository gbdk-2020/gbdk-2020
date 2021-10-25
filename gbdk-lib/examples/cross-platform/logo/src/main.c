#include <gbdk/platform.h>

#include "logo.h"

void main() {
    fill_bkg_rect(0,0,DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0);
#if defined(SEGA)
    set_palette(0, 1, logo_palettes);
#endif
    set_native_tile_data(0, logo_TILE_COUNT, logo_tiles);
    set_tile_map(0, 0, logo_WIDTH >> 3, logo_HEIGHT >> 3, logo_map);
    SHOW_BKG;
}