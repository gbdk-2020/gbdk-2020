/** @file gb/metasprites.h
    
    Metasprite support
*/

#ifndef _METASPRITES_H_INCLUDE
#define _METASPRITES_H_INCLUDE

/**
 * metasprite item description
 */
typedef struct metasprite_t {
    INT8  dy, dx;
    UINT8 dtile;
    UINT8 props;
} metasprite_t;

#define metasprite_end -128 

extern const void * __current_metasprite;
extern UBYTE __current_base_tile;
extern UBYTE __render_shadow_OAM;

static UBYTE __move_metasprite(UINT8 id, UINT8 x, UINT8 y);
static UBYTE __move_metasprite_vflip(UINT8 id, UINT8 x, UINT8 y);
static UBYTE __move_metasprite_hflip(UINT8 id, UINT8 x, UINT8 y);
static UBYTE __move_metasprite_hvflip(UINT8 id, UINT8 x, UINT8 y);
static void __hide_metasprite(UINT8 id);

/**
 * Moves metasprite to the absolute position x and y, allocating hardware sprites from base_sprite using tiles from base_tile
 * @param metasprite metasprite description
 * @param base_tile start tile where tiles for that metasprite begin
 * @param base_sprite start hardware sprite
 * @param x absolute x coordinate of the sprite
 * @param y absolute y coordinate of the sprite
 * @return number of hardware sprites used to draw this metasprite
 **/
inline UBYTE move_metasprite(const metasprite_t * metasprite, UINT8 base_tile, UINT8 base_sprite, UINT8 x, UINT8 y) {
    __current_metasprite = metasprite; 
    __current_base_tile = base_tile;
    return __move_metasprite(base_sprite, x, y); 
}

inline UBYTE move_metasprite_vflip(const metasprite_t * metasprite, UINT8 base_tile, UINT8 base_sprite, UINT8 x, UINT8 y) {
    __current_metasprite = metasprite; 
    __current_base_tile = base_tile;
    return __move_metasprite_vflip(base_sprite, x - 8, y); 
}

inline UBYTE move_metasprite_hflip(const metasprite_t * metasprite, UINT8 base_tile, UINT8 base_sprite, UINT8 x, UINT8 y) {
    __current_metasprite = metasprite; 
    __current_base_tile = base_tile;
    return __move_metasprite_hflip(base_sprite, x, y - ((LCDC_REG & 0x04U) ? 16 : 8) ); 
}

inline UBYTE move_metasprite_hvflip(const metasprite_t * metasprite, UINT8 base_tile, UINT8 base_sprite, UINT8 x, UINT8 y) {
    __current_metasprite = metasprite; 
    __current_base_tile = base_tile;
    return __move_metasprite_hvflip(base_sprite, x - 8, y - ((LCDC_REG & 0x04U) ? 16 : 8)); 
}

/**
 * Hides metasprite from screen
 * @param metasprite metasprite description
 * @param base_sprite start hardware sprite
 **/
inline void hide_metasprite(const metasprite_t * metasprite, UINT8 base_sprite) {
    __current_metasprite = metasprite; 
    __hide_metasprite(base_sprite);
}

#endif