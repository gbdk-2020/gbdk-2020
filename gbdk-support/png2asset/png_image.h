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
    unsigned char* palette; //palette colors in RGBA (1 color == 4 bytes)

private:
    bool zero_palette = false;

public:
    unsigned char GetGBColor(int x, int y)
    {
        return data[w * y + x] % colors_per_pal;
    }

    void BuildExtraTileInfo(Tile& tile, int pixelPalette, int pixelIndex, int extract_tile_w, int extract_tile_h, int i, int j) {
        signed int index = -1;

        // Check each item for extra palettes
        for(int k = 0; k < tile.extraPaletteInfo.size(); k++) {

            // if this object has the same palette as the pixel, we'll us it's index
            if(tile.extraPaletteInfo[k].palette == pixelPalette) {

                // Stop here
                index = k;
                break;
            }
        }

        // If we didn't find an existing palette struct, create a new one and add it
        if(index == -1) {
            ExtraPalette newEp;
            newEp.palette = pixelPalette;

            // Fill with empty first pallete colors for transparency
            for(int i2 = 0; i2 < extract_tile_h * extract_tile_w; i2++) {
                newEp.pixelData.push_back(0);

            }

            tile.extraPaletteInfo.push_back(newEp);

            // Set the index for the latest addition
            index = tile.extraPaletteInfo.size() - 1;
        }


        int dataIndex = i + (j * extract_tile_w);

        tile.extraPaletteInfo[index].pixelData[dataIndex] = pixelIndex - (4 * pixelPalette);
    }


    // This needs separate tile_w and tile_h params since
    // MSX tile extraction uses it to pull out the 4 sub-tiles
    bool ExtractGBTile(int x, int y, int extract_tile_w, int extract_tile_h, Tile& tile, int buffer_offset)
    {
        // Set the palette to 0 when pals are not stored in tiles to allow tiles to be equal even when their palettes are different
        tile.pal = zero_palette ? 0 : data[w * y + x] >> 2;

        bool all_zero = true;
        for(int j = 0; j < extract_tile_h; ++j)
        {
            for(int i = 0; i < extract_tile_w; ++i)
            {
                int pixelIndex = data[w * (y + j) + (x + i)];
                int pixelPalette = pixelIndex >> 2;

                unsigned char color_idx = GetGBColor(x + i, y + j);

                // if this pixel's palette is different than the previously decided palette
                if(pixelPalette != tile.pal) {
                    BuildExtraTileInfo(tile,pixelPalette,pixelIndex,extract_tile_w, extract_tile_h,i,j);
                }

                tile.data[(j * extract_tile_w) + i + buffer_offset] = color_idx;
                all_zero = all_zero && (color_idx == 0);
            }
        }
        return !all_zero;
    }

    bool ExtractTile_MSX16x16(int x, int y, Tile& tile)
    {
        // MSX 16x16 sprite tiles are composed of four 8x8 tiles in this order UL, LL, UR, LR
        bool UL_notempty, LL_notempty, UR_notempty, LR_notempty;

        // Call these separately since otherwise some get optimized out during
        // runtime if any single one before it returns false
        UL_notempty = ExtractGBTile(x, y, 8, 8, tile, 0);
        LL_notempty = ExtractGBTile(x, y + 8, 8, 8, tile, ((8 * 8) * 1));
        UR_notempty = ExtractGBTile(x + 8, y, 8, 8, tile, ((8 * 8) * 2));
        LR_notempty = ExtractGBTile(x + 8, y + 8, 8, 8, tile, ((8 * 8) * 3));
        return (UL_notempty || LL_notempty || UR_notempty || LR_notempty);
    }

    bool ExtractTile(int x, int y, Tile& tile, int sprite_mode, bool export_as_map, bool use_map_attributes)
    {
        // Set the palette to 0 when pals are not stored in tiles to allow tiles to be equal even when their palettes are different
        zero_palette = !(export_as_map && !use_map_attributes);

        if(sprite_mode == SPR_16x16_MSX)
            return ExtractTile_MSX16x16(x, y, tile);
        else
            return ExtractGBTile(x, y, tile_w, tile_h, tile, 0); // No buffer offset for normal tile extraction
    }
    // private:
    //     bool zero_palette = false;
};
