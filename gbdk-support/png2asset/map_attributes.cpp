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

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "process_arguments.h"

unsigned char GetMapAttribute(size_t x, size_t y,PNG2AssetData* assetData)
{
    if(x < assetData->args->map_attributes_size.width && y < assetData->args->map_attributes_size.height)
        return assetData->map_attributes[y * assetData->args->map_attributes_size.width + x];
    else
        return 0;
}

void ReduceMapAttributes2x2(const vector< SetPal >& palettes,PNG2AssetData* assetData)
{
    size_t w = (assetData->args->map_attributes_size.width + 1) / 2;
    size_t h = (assetData->args->map_attributes_size.height + 1) / 2;
    vector< unsigned char > map_attributes_2x2;
    map_attributes_2x2.resize(w * h);
    for(size_t y = 0; y < h; y++)
    {
        for(size_t x = 0; x < w; x++)
        {
            // Use only Top-left attribute, ignoring the other three as they should now be identical
            map_attributes_2x2[y * w + x] = GetMapAttribute(2 * x, 2 * y,assetData);
        }
    }
    // Overwrite old attributes
    assetData->args->map_attributes_size.width = w;
    assetData->args->map_attributes_size.height = h;
    assetData->map_attributes = map_attributes_2x2;
}

//
// Aligns map attribute data to be aligned properly for NES and set_bkg_submap_attributes
// Namely:
// * Width aligned to multiples of 2 to reflect the NES's packed attribute table
// * Every 16th row is blank to reflect the unused row in the NES's packed attribute table
//
void AlignMapAttributes(PNG2AssetData* assetData)
{
    const size_t ATTRIBUTE_HEIGHT = 15;
    const size_t ATTRIBUTE_ALIGNED_HEIGHT = 16;
    vector< unsigned char > map_attributes_aligned;
    size_t map_attributes_aligned_width = 2 * ((assetData->args->map_attributes_size.width + 1) / 2);
    size_t num_vertical_nametables = (assetData->args->map_attributes_size.height + ATTRIBUTE_HEIGHT - 1) / ATTRIBUTE_HEIGHT;
    map_attributes_aligned.resize(map_attributes_aligned_width * (num_vertical_nametables * ATTRIBUTE_ALIGNED_HEIGHT));
    for(size_t i = 0; i < num_vertical_nametables; i++)
    {
        bool last_nametable = (i == num_vertical_nametables - 1);
        size_t height = last_nametable ? (assetData->args->map_attributes_size.height - i * ATTRIBUTE_HEIGHT) : ATTRIBUTE_HEIGHT;
        for(size_t y = 0; y < height; y++)
        {
            for(size_t x = 0; x < assetData->args->map_attributes_size.width; x++)
            {
                map_attributes_aligned[(i * ATTRIBUTE_ALIGNED_HEIGHT + y) * map_attributes_aligned_width + x] =
                    assetData->map_attributes[(i * ATTRIBUTE_HEIGHT + y) * assetData->args->map_attributes_size.width + x];
            }
        }
    }
    // Overwrite old attributes
    assetData->args->map_attributes_size.width = map_attributes_aligned_width;
    assetData->args->map_attributes_size.height = num_vertical_nametables * ATTRIBUTE_ALIGNED_HEIGHT;
    assetData->map_attributes = map_attributes_aligned;
}

//
// Pack map attributes
// (NES packs multiple 2-bit entries into one byte)
//
void PackMapAttributes(PNG2AssetData *assetData)
{
    vector< unsigned char > map_attributes_packed;
    assetData->args->map_attributes_packed_size.width = (assetData->args->map_attributes_size.width + 1) / 2;
    assetData->args->map_attributes_packed_size.height = (assetData->args->map_attributes_size.height + 1) / 2;
    map_attributes_packed.resize(assetData->args->map_attributes_packed_size.width * assetData->args->map_attributes_packed_size.height);
    for(size_t y = 0; y < assetData->args->map_attributes_packed_size.height; y++)
    {
        for(size_t x = 0; x < assetData->args->map_attributes_packed_size.width; x++)
        {
            unsigned char a_tl = GetMapAttribute(2 * x + 0, 2 * y + 0, assetData);
            unsigned char a_tr = GetMapAttribute(2 * x + 1, 2 * y + 0, assetData);
            unsigned char a_bl = GetMapAttribute(2 * x + 0, 2 * y + 1, assetData);
            unsigned char a_br = GetMapAttribute(2 * x + 1, 2 * y + 1, assetData);
            unsigned char packed_bits = (a_br << 6) | (a_bl << 4) | (a_tr << 2) | (a_tl << 0);
            map_attributes_packed[assetData->args->map_attributes_packed_size.width * y + x] = packed_bits;
        }
    }
    // Overwrite old attributes
    assetData->map_attributes = map_attributes_packed;
}

void HandleMapAttributes(PNG2AssetData* assetData) {
    assetData->args->map_attributes_size.width = assetData->image.w / 8;
    assetData->args->map_attributes_size.height = assetData->image.h / 8;

    // Optionally perform 2x2 reduction on attributes (NES attribute table has this format)
    if(assetData->args->use_2x2_map_attributes)
    {
        // NES attribute map dimensions are half-resolution 
        ReduceMapAttributes2x2(assetData->palettes, assetData);
    }
    // Optionally align and pack map attributes into NES PPU format
    if(assetData->args->pack_map_attributes)
    {
        AlignMapAttributes(assetData);
        PackMapAttributes(assetData);
    }
    else
    {
        // Use original attribute dimensions for packed
        assetData->args->map_attributes_packed_size.width = assetData->args->map_attributes_size.width;
        assetData->args->map_attributes_packed_size.height = assetData->args->map_attributes_size.height;
    }
}