#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include <cstdint>

#include "lodepng.h"
#include "mttile.h"
#include "export.h"
#include "map_attributes.h"
#include "palettes.h"

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "maps.h"
#include "metasprites.h"


unsigned int PaletteCountApplyMaxLimit(unsigned int max_palettes, unsigned int cur_palette_size)
{
    if(cur_palette_size > max_palettes)
    {
        printf("Warning: %d palettes found, truncating to %d (-max_palettes)\n", (unsigned int)cur_palette_size, (unsigned int)max_palettes);
        return max_palettes;
    }
    else
        return cur_palette_size;
}

int FindOrCreateSubPalette(const SetPal& pal, vector< SetPal >& palettes, size_t colors_per_pal)
{
    // Return -1 if colors can't even fit in sub-palette hardware limit
    if(pal.size() > colors_per_pal)
    {
        return -1;
    }
    //Check if it matches any palettes or create a new one
    int i;
    for(i = 0; i < (int)palettes.size(); ++i)
    {
        //Try to merge this palette with any of the palettes (checking if they are equal is not enough since the palettes can have less than 4 colors)
        SetPal merged(palettes[i]);
        merged.insert(pal.begin(), pal.end());
        if(merged.size() <= colors_per_pal)
        {
            if(palettes[i].size() <= colors_per_pal)
                palettes[i] = merged; //Increase colors with this palette (it has less than 4 colors)
            return i; //Found palette
        }
    }

    if(i == (int)palettes.size())
    {
        //Palette not found, add a new one
        palettes.push_back(pal);
    }
    return i;
}



//
// Builds palettes and palette-per-tile (attributes) for an image
//
// Inputs:
//   image32         : Input image in RGBA32 format
//   palettes        : Palettes for image. Will be added-to / merged with new palettes.
//   half_resolution : If true, attributes will follow a constraint of every 2x2 cell having the same attribute.
//
// Returns: array of attributes. This always has *per-tile* dimensions, even when half_resolution is true.
//
int* BuildPalettesAndAttributes(const PNGImage& image32, vector< SetPal >& palettes, bool half_resolution)
{
    int* palettes_per_tile = new int[(image32.w / image32.tile_w) * (image32.h / image32.tile_h)];
    int sx = half_resolution ? 2 : 1;
    int sy = half_resolution ? 2 : 1;
    for(unsigned int y = 0; y < image32.h; y += image32.tile_h * sy)
    {
        for(unsigned int x = 0; x < image32.w; x += image32.tile_w * sx)
        {
            //Get palette colors on (x, y, image32.tile_w, image32.tile_h)
            SetPal pal = GetPaletteColors(image32, (x / sx) * sx, (y / sy) * sy, sx * image32.tile_w, sy * image32.tile_h);

            int subPalIndex = FindOrCreateSubPalette(pal, palettes, image32.colors_per_pal);
            if(subPalIndex < 0)
            {
                printf("Error: more than %d colors found in tile at x:%d, y:%d of size w:%d, h:%d\n",
                    (unsigned int)image32.colors_per_pal,
                    (x / sx) * sx,
                    (y / sy) * sy,
                    sx * image32.tile_w,
                    sy * image32.tile_h);
                subPalIndex = 0; // Force to sub-palette 0, to allow getting a partially-incorrect output image
            }
            // Assign single or multiple entries in palettes_per_tile, to keep it independent of
            // of half_resolution parameter
            int dx = ((x / image32.tile_w) / sx) * sx;
            int dy = ((y / image32.tile_h) / sy) * sy;
            int w = (image32.w / image32.tile_w);
            for(int yy = 0; yy < sy; yy++)
            {
                for(int xx = 0; xx < sx; xx++)
                {
                    palettes_per_tile[(dy + yy) * w + dx + xx] = subPalIndex;
                }
            }
        }
    }
    return palettes_per_tile;
}