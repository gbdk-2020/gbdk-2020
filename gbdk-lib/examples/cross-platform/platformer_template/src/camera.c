#pragma bank 255

#include <stdint.h>
#include <gbdk/platform.h>
#include "level.h"

#define MIN(A,B) ((A)<(B)?(A):(B))

// current and old positions of the camera in pixels
uint16_t camera_x, camera_y, old_camera_x, old_camera_y;
// current and old position of the map in tiles
uint8_t map_pos_x, map_pos_y, old_map_pos_x, old_map_pos_y;
// redraw flag, indicates that camera position was changed
uint8_t redraw;

void SetCurrentLevelSubmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h) NONBANKED{
    
    uint8_t _previous_bank = _current_bank;

    SWITCH_ROM(currentAreaBank);

    set_bkg_submap(x,y,w, h, currentLevelMap, currentLevelWidthInTiles);

    SWITCH_ROM(_previous_bank);
    
}

void UpdateCamera() BANKED {
    // update hardware scroll position
    move_bkg(camera_x,camera_y);

    // up or down
    map_pos_y = (uint8_t)(camera_y >> 3u);
    if (map_pos_y != old_map_pos_y) { 
        if (camera_y < old_camera_y) {
            SetCurrentLevelSubmap(map_pos_x, map_pos_y, MIN(DEVICE_SCREEN_WIDTH+1, currentLevelWidthInTiles-map_pos_x), 1);
        } else {
            if ((currentLevelHeightInTiles - DEVICE_SCREEN_HEIGHT) > map_pos_y) SetCurrentLevelSubmap(map_pos_x, map_pos_y + DEVICE_SCREEN_HEIGHT, MIN(DEVICE_SCREEN_WIDTH+1, currentLevelWidthInTiles-map_pos_x), 1);     
        }
        old_map_pos_y = map_pos_y; 
    }
    // left or right
    map_pos_x = (uint8_t)(camera_x >> 3u);
    if (map_pos_x != old_map_pos_x) {
        if (camera_x < old_camera_x) {
            SetCurrentLevelSubmap(map_pos_x, map_pos_y, 1, MIN(DEVICE_SCREEN_HEIGHT+1, currentLevelHeightInTiles - map_pos_y));     
        } else {
            if ((currentLevelWidthInTiles - DEVICE_SCREEN_WIDTH) > map_pos_x) SetCurrentLevelSubmap(map_pos_x + DEVICE_SCREEN_WIDTH, map_pos_y, 1, MIN(DEVICE_SCREEN_HEIGHT+1, currentLevelHeightInTiles - map_pos_y));     
        }
        old_map_pos_x = map_pos_x;
    }
    // set old camera position to current camera position
    old_camera_x = camera_x, old_camera_y = camera_y;
}
