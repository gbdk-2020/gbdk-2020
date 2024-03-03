#pragma bank 255

#include <stdint.h>
#include <gbdk/platform.h>
#include "level.h"

#if defined(SEGA)
  #define WRAP_SCROLL_Y(y) ((y) % 224u)
  // For SMS, artifacts are already invisible as screen buffer size is larger than screen size
  #define SCROLL_Y_OFFSET 0
#elif defined(NINTENDO)
  #define WRAP_SCROLL_Y(y) y
  // For GB, artifacts are already invisible as screen buffer size is larger than screen size
  #define SCROLL_Y_OFFSET 0
#else
  #define WRAP_SCROLL_Y(y) ((y) % 240u)
  // For other systems assume height of 240 and adjust Y-scroll 4 pixels down to partly hide artifacts in NTSC overscan
  #define SCROLL_Y_OFFSET 4
#endif

#if defined(SEGA)
// The "compatibility" function set_bkg_submap (=set_tile_submap_compat)
// expect maps with only 1-byte-per-tile, only containing either a tile
// index or an attribute.
// But png2asset can only produce 2-byte-per-tile maps where both tile index
// and attribute are consecutive when the -map_attributes parameter is used.
// To work around this, we redefine those functions for SMS/GG only, so
// they work like this:
// * Use set_tile_submap for tile indices, to read/write both bytes of map
// * Make set_submap_attributes a no-op, as attributes were already set
#define set_submap_indices(x, y, w, h, map, map_w) set_tile_submap(x, y, w, h, map_w, map)
#define set_submap_attributes(x, y, w, h, map, map_w)
#else
#define set_submap_indices(x, y, w, h, map, map_w) set_bkg_submap(x, y, w, h, map, map_w)
#define set_submap_attributes(x, y, w, h, map, map_w) set_bkg_submap_attributes(x, y, w, h, map, map_w)
#endif

#define MIN(A,B) ((A)<(B)?(A):(B))

// current and old positions of the camera in pixels
uint16_t camera_x, old_camera_x;
// current and old position of the map in tiles
uint8_t map_pos_x, old_map_pos_x;
// redraw flag, indicates that camera position was changed
uint8_t redraw;

void SetCurrentLevelSubmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h) NONBANKED{
    
    uint8_t _previous_bank = _current_bank;

    SWITCH_ROM(currentAreaBank);

    set_submap_indices(x,y,w, h, currentLevelMap, currentLevelWidthInTiles);

    SWITCH_ROM(_previous_bank);
    
}

inline uint8_t update_column_left(uint8_t map_pos_x)
{
#if (DEVICE_SCREEN_BUFFER_WIDTH == DEVICE_SCREEN_WIDTH)
    return map_pos_x + 1;
#else
    return map_pos_x;
#endif
}

inline uint8_t update_column_right(uint8_t map_pos_x)
{
    return map_pos_x + DEVICE_SCREEN_WIDTH;
}

void UpdateCamera() BANKED {

    // update hardware scroll position
    move_bkg(camera_x, 0);
    
    // left or right
    map_pos_x = (uint8_t)(camera_x >> 3u);
    if (map_pos_x != old_map_pos_x) {
        if (camera_x < old_camera_x) {
            SetCurrentLevelSubmap(
                    update_column_right(map_pos_x), 
                    0, 
                    1, 
                    MIN(DEVICE_SCREEN_HEIGHT, currentLevelHeightInTiles ));     
        } else {
            if ((currentLevelWidthInTiles - DEVICE_SCREEN_WIDTH) > map_pos_x) {
                SetCurrentLevelSubmap(
                    update_column_right(map_pos_x), 
                    0, 
                    1, 
                    MIN(DEVICE_SCREEN_HEIGHT, currentLevelHeightInTiles));   
            }  
        }
        old_map_pos_x = map_pos_x;
    }
    // set old camera position to current camera position
    old_camera_x = camera_x;
}
