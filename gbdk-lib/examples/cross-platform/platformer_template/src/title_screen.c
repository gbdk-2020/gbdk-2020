#include <gbdk/platform.h>
#include "common.h"
#include "TitleScreen.h"


void ShowTitleScreen(){

    // Hide any remainder sprites
    hide_sprites_range(0,MAX_HARDWARE_SPRITES);

    move_bkg(0,0);

    // Make sure the graphic is centered on the screen
    uint8_t titleRow = (DEVICE_SCREEN_HEIGHT-(TitleScreen_HEIGHT>>3))/2;
    uint8_t titleColumn = (DEVICE_SCREEN_WIDTH-(TitleScreen_WIDTH>>3))/2;

    set_bkg_data(0,TitleScreen_TILE_COUNT,TitleScreen_tiles);
    fill_bkg_rect(0,0,DEVICE_SCREEN_WIDTH,DEVICE_SCREEN_HEIGHT,0);
    set_bkg_tiles(titleColumn,titleRow,TitleScreen_WIDTH>>3,TitleScreen_HEIGHT>>3,TitleScreen_map);

    WaitForStartOrA();
}