#include <gbdk/platform.h>
#include "common.h"
#include "NextLevel.h"


void ShowNextLevel(){

    // Hide any remainder sprites
    hide_sprites_range(0,MAX_HARDWARE_SPRITES);

    move_bkg(0,0);

    // Make sure the graphic is centered on the screen
    uint8_t titleRow = (DEVICE_SCREEN_HEIGHT-(NextLevel_HEIGHT>>3))/2;
    uint8_t titleColumn = (DEVICE_SCREEN_WIDTH-(NextLevel_WIDTH>>3))/2;

    set_bkg_data(0,NextLevel_TILE_COUNT,NextLevel_tiles);
    fill_bkg_rect(0,0,DEVICE_SCREEN_WIDTH,DEVICE_SCREEN_HEIGHT,0);
    set_bkg_tiles(titleColumn,titleRow,NextLevel_WIDTH>>3,NextLevel_HEIGHT>>3,NextLevel_map);

    WaitForStartOrA();
}