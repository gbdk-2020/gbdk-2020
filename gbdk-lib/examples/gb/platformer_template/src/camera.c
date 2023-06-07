
#include <stdint.h>
#include <gbdk/platform.h>
#include "level.h"

#include <gb/gb.h>
#include <stdint.h>

#define MIN(A,B) ((A)<(B)?(A):(B))

// current and old positions of the camera in pixels
uint16_t camera_x, camera_y, old_camera_x, old_camera_y;
// current and old position of the map in tiles
uint8_t map_pos_x, map_pos_y, old_map_pos_x, old_map_pos_y;
// redraw flag, indicates that camera position was changed
uint8_t redraw;

void UpdateCamera(){

    // update hardware scroll position
    SCY_REG = camera_y; SCX_REG = camera_x; 
    // up or down
    map_pos_y = (uint8_t)(camera_y >> 3u);
    if (map_pos_y != old_map_pos_y) { 
        if (camera_y < old_camera_y) {
            set_bkg_submap(map_pos_x, map_pos_y, MIN(DEVICE_SCREEN_WIDTH+1, currentLevelWidthInTiles-map_pos_x), 1, currentLevelMap, currentLevelWidthInTiles);
        } else {
            if ((currentLevelHeightInTiles - DEVICE_SCREEN_HEIGHT) > map_pos_y) set_bkg_submap(map_pos_x, map_pos_y + DEVICE_SCREEN_HEIGHT, MIN(DEVICE_SCREEN_WIDTH+1, currentLevelWidthInTiles-map_pos_x), 1, currentLevelMap, currentLevelWidthInTiles);     
        }
        old_map_pos_y = map_pos_y; 
    }
    // left or right
    map_pos_x = (uint8_t)(camera_x >> 3u);
    if (map_pos_x != old_map_pos_x) {
        if (camera_x < old_camera_x) {
            set_bkg_submap(map_pos_x, map_pos_y, 1, MIN(DEVICE_SCREEN_HEIGHT+1, currentLevelHeightInTiles - map_pos_y), currentLevelMap, currentLevelWidthInTiles);     
        } else {
            if ((currentLevelWidthInTiles - DEVICE_SCREEN_WIDTH) > map_pos_x) set_bkg_submap(map_pos_x + DEVICE_SCREEN_WIDTH, map_pos_y, 1, MIN(DEVICE_SCREEN_HEIGHT+1, currentLevelHeightInTiles - map_pos_y), currentLevelMap, currentLevelWidthInTiles);     
        }
        old_map_pos_x = map_pos_x;
    }
    // set old camera position to current camera position
    old_camera_x = camera_x, old_camera_y = camera_y;
}

void InitialCameraDraw(){

    // Draw the initial area
    // Draw one extra column to avoid a blank row when first scrolling.
    // If scrolling vertically also, you should draw one extra row as well.
    // The platformer template will only scroll horizontally
    set_bkg_submap(0,0,DEVICE_SCREEN_WIDTH+1,DEVICE_SCREEN_HEIGHT,currentLevelMap,currentLevelWidthInTiles);
}


