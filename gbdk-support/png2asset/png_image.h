#pragma once
#include <vector>
#include "tiles.h"
using namespace std;

#define RGBA32_SZ 4 // RGBA 8:8:8:8 is 4 bytes per pixel

enum {
    SPR_NONE,
    SPR_8x8,
    SPR_8x16,
    SPR_16x16_MSX
};

// processing_mode states
enum {
    MODE_MAIN_IMAGE,
    MODE_SOURCE_TILESET,
    MODE_ENTITY_TILESET
};

struct PNGImage
{
    vector< unsigned char > data; //data in indexed format
    unsigned int w;
    unsigned int h;

    // Default tile size
    int tile_w = 8;
    int tile_h = 16;

    // TODO: embed these instead of deriving them many places in the code
    // int attribute_w_factor = 1;
    // int attribute_h_factor = 1;
    // int get_attribute_tile_w() { tile_w * attribute_w_factor; }
    // int get_attribute_tile_h() { tile_h * attribute_h_factor; }

    size_t colors_per_pal;  // Number of colors per palette (ex: CGB has 4 colors per palette x 8 palettes total)
    size_t total_color_count; // Total number of colors across all palettes (palette_count x colors_per_pal)
    unsigned char * palette = NULL; //palette colors in RGBA (1 color == 4 bytes)
    unsigned char * source_tileset_palette = NULL;  // Mostly used for ensuring source tileset and primary image palettes match sufficiently

private:
    bool zero_palette = false;

public:
    unsigned char GetGBColor(int x, int y)
    {
        return data[w * y + x] % colors_per_pal;
    }


    // This needs separate tile_w and tile_h params since
    // MSX tile extraction uses it to pull out the 4 sub-tiles
    bool ExtractGBTile(int x, int y, int extract_tile_w, int extract_tile_h, Tile& tile, int buffer_offset, int bpp)
    {
        // Set the palette to 0 when pals are not stored in tiles to allow tiles to be equal even when their palettes are different
        tile.pal = zero_palette ? 0 : data[w * y + x] >> bpp;

        bool all_zero = true;
        for(int j = 0; j < extract_tile_h; ++j)
        {
            for(int i = 0; i < extract_tile_w; ++i)
            {
                unsigned char color_idx = GetGBColor(x + i, y + j);
                tile.data[(j * extract_tile_w) + i + buffer_offset] = color_idx;
                all_zero = all_zero && (color_idx == 0);
            }
        }
        return !all_zero;
    }

    bool ExtractTile_MSX16x16(int x, int y, Tile& tile, int bpp)
    {
        // MSX 16x16 sprite tiles are composed of four 8x8 tiles in this order UL, LL, UR, LR
        bool UL_notempty, LL_notempty, UR_notempty, LR_notempty;

        // Call these separately since otherwise some get optimized out during
        // runtime if any single one before it returns false
        UL_notempty = ExtractGBTile(x, y, 8, 8, tile, 0, bpp);
        LL_notempty = ExtractGBTile(x, y + 8, 8, 8, tile, ((8 * 8) * 1), bpp);
        UR_notempty = ExtractGBTile(x + 8, y, 8, 8, tile, ((8 * 8) * 2), bpp);
        LR_notempty = ExtractGBTile(x + 8, y + 8, 8, 8, tile, ((8 * 8) * 3), bpp);
        return (UL_notempty || LL_notempty || UR_notempty || LR_notempty);
    }

    bool ExtractTile(int x, int y, Tile& tile, int sprite_mode, bool export_as_map, bool use_map_attributes, int bpp)
    {
        // Set the palette to 0 when pals are not stored in tiles to allow tiles to be equal even when their palettes are different
        zero_palette = !(export_as_map && !use_map_attributes);

        if(sprite_mode == SPR_16x16_MSX)
            return ExtractTile_MSX16x16(x, y, tile, bpp);
        else
            return ExtractGBTile(x, y, tile_w, tile_h, tile, 0, bpp); // No buffer offset for normal tile extraction
    }
    // private:
    //     bool zero_palette = false;
};
