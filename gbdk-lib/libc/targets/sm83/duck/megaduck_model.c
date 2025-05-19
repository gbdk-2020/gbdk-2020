#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include <duck/model.h>


#define MEGADUCK_MODEL_TILE_ADDR_CHECK 0x8D00u  // First tile at 0x8D00, Second tile at 0x8D10

// 2 consecutive Tiles:
// * Spanish model: Upside-down black Question Mark and Exclamation Point
// * German  model: 2 pixel tall black Underscore and Inverted 0 on dark grey background
//
// Note: It may be sufficient to just check 1 tile
static const uint8_t model_spanish_tiles[32] = {
    0x00u, 0x00u, 0x18u, 0x18u, 0x00u, 0x00u, 0x38u, 0x38u, 0x70u, 0x70u, 0x72u, 0x72u, 0x76u, 0x76u, 0x3Cu, 0x3Cu, // Upside-down black Question Mark
    0x00u, 0x00u, 0x18u, 0x18u, 0x00u, 0x00u, 0x18u, 0x18u, 0x3Cu, 0x3Cu, 0x3Cu, 0x3Cu, 0x3Cu, 0x3Cu, 0x18u, 0x18u, // Upside-down black Exclamation Point
};

static const uint8_t model_german_tiles[32] = {
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFFu,  // 2 pixel tall black Underscore
    0x00u, 0xFFu, 0x00u, 0xC3u, 0x00u, 0x99u, 0x00u, 0x99u, 0x00u, 0x99u, 0x00u, 0x99u, 0x00u, 0xC3u, 0x00u, 0xFFu,  // Inverted 0 on dark grey background
};

#define MODEL_SPANISH_TILES_SZ (sizeof(model_spanish_tiles) / sizeof(model_spanish_tiles[0]))
#define MODEL_GERMAN_TILES_SZ  (sizeof(model_german_tiles)  / sizeof(model_german_tiles[0]))


static bool vram_memcmp(const uint8_t * p_buf, uint8_t * p_vram, uint8_t cmp_size) {

    while (cmp_size--) {
        if (get_vram_byte(p_vram++) != *p_buf++) {
            return false;
        }
    }
    return true;
}


// This detection should be called immediately at the start of
// the program for most reliable results, since it relies on
// inspecting uncleared VRAM contents.
uint8_t duck_check_model(void) {

    if (vram_memcmp(model_spanish_tiles, (uint8_t *)MEGADUCK_MODEL_TILE_ADDR_CHECK, MODEL_SPANISH_TILES_SZ)) {
        return MEGADUCK_LAPTOP_SPANISH;
    }

    // Check German
    if (vram_memcmp(model_german_tiles, (uint8_t *)MEGADUCK_MODEL_TILE_ADDR_CHECK, MODEL_SPANISH_TILES_SZ)) {
        return MEGADUCK_LAPTOP_GERMAN;
    }

    return MEGADUCK_HANDHELD_STANDARD; // Default
}
