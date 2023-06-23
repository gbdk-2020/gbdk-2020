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

unsigned char GetMapAttribute(size_t x, size_t y,PNG2AssetData* png2AssetData)
{
	if(x < png2AssetData->arguments.map_attributes_width && y < png2AssetData->arguments.map_attributes_height)
		return png2AssetData->map_attributes[y * png2AssetData->arguments.map_attributes_width + x];
	else
		return 0;
}

void ReduceMapAttributes2x2(const vector< SetPal >& palettes,PNG2AssetData* png2AssetData)
{
	size_t w = (png2AssetData->arguments.map_attributes_width + 1) / 2;
	size_t h = (png2AssetData->arguments.map_attributes_height + 1) / 2;
	vector< unsigned char > map_attributes_2x2;
	map_attributes_2x2.resize(w * h);
	for(size_t y = 0; y < h; y++)
	{
		for(size_t x = 0; x < w; x++)
		{
			// Use only Top-left attribute, ignoring the other three as they should now be identical
			map_attributes_2x2[y * w + x] = GetMapAttribute(2 * x, 2 * y,png2AssetData);
		}
	}
	// Overwrite old attributes
	png2AssetData->arguments.map_attributes_width = w;
	png2AssetData->arguments.map_attributes_height = h;
	png2AssetData->map_attributes = map_attributes_2x2;
}

//
// Aligns map attribute data to be aligned properly for NES and set_bkg_submap_attributes
// Namely:
// * Width aligned to multiples of 2 to reflect the NES's packed attribute table
// * Every 16th row is blank to reflect the unused row in the NES's packed attribute table
//
void AlignMapAttributes(PNG2AssetData* png2AssetData)
{
	const size_t ATTRIBUTE_HEIGHT = 15;
	const size_t ATTRIBUTE_ALIGNED_HEIGHT = 16;
	vector< unsigned char > map_attributes_aligned;
	size_t map_attributes_aligned_width = 2 * ((png2AssetData->arguments.map_attributes_width + 1) / 2);
	size_t num_vertical_nametables = (png2AssetData->arguments.map_attributes_height + ATTRIBUTE_HEIGHT - 1) / ATTRIBUTE_HEIGHT;
	map_attributes_aligned.resize(map_attributes_aligned_width * (num_vertical_nametables * ATTRIBUTE_ALIGNED_HEIGHT));
	for(size_t i = 0; i < num_vertical_nametables; i++)
	{
		bool last_nametable = (i == num_vertical_nametables - 1);
		size_t height = last_nametable ? (png2AssetData->arguments.map_attributes_height - i * ATTRIBUTE_HEIGHT) : ATTRIBUTE_HEIGHT;
		for(size_t y = 0; y < height; y++)
		{
			for(size_t x = 0; x < png2AssetData->arguments.map_attributes_width; x++)
			{
				map_attributes_aligned[(i * ATTRIBUTE_ALIGNED_HEIGHT + y) * map_attributes_aligned_width + x] =
					png2AssetData->map_attributes[(i * ATTRIBUTE_HEIGHT + y) * png2AssetData->arguments.map_attributes_width + x];
			}
		}
	}
	// Overwrite old attributes
	png2AssetData->arguments.map_attributes_width = map_attributes_aligned_width;
	png2AssetData->arguments.map_attributes_height = num_vertical_nametables * ATTRIBUTE_ALIGNED_HEIGHT;
	png2AssetData->map_attributes = map_attributes_aligned;
}

//
// Pack map attributes
// (NES packs multiple 2-bit entries into one byte)
//
void PackMapAttributes(PNG2AssetData *png2AssetData)
{
	vector< unsigned char > map_attributes_packed;
	png2AssetData->arguments.map_attributes_packed_width = (png2AssetData->arguments.map_attributes_width + 1) / 2;
	png2AssetData->arguments.map_attributes_packed_height = (png2AssetData->arguments.map_attributes_height + 1) / 2;
	map_attributes_packed.resize(png2AssetData->arguments.map_attributes_packed_width * png2AssetData->arguments.map_attributes_packed_height);
	for(size_t y = 0; y < png2AssetData->arguments.map_attributes_packed_height; y++)
	{
		for(size_t x = 0; x < png2AssetData->arguments.map_attributes_packed_width; x++)
		{
			unsigned char a_tl = GetMapAttribute(2 * x + 0, 2 * y + 0, png2AssetData);
			unsigned char a_tr = GetMapAttribute(2 * x + 1, 2 * y + 0, png2AssetData);
			unsigned char a_bl = GetMapAttribute(2 * x + 0, 2 * y + 1, png2AssetData);
			unsigned char a_br = GetMapAttribute(2 * x + 1, 2 * y + 1, png2AssetData);
			unsigned char packed_bits = (a_br << 6) | (a_bl << 4) | (a_tr << 2) | (a_tl << 0);
			map_attributes_packed[png2AssetData->arguments.map_attributes_packed_width * y + x] = packed_bits;
		}
	}
	// Overwrite old attributes
	png2AssetData->map_attributes = map_attributes_packed;
}

void HandleMapAttributes(PNG2AssetData* png2AssetData) {
	png2AssetData->arguments.map_attributes_width = png2AssetData->image.w / 8;
	png2AssetData->arguments.map_attributes_height = png2AssetData->image.h / 8;

	// Optionally perform 2x2 reduction on attributes (NES attribute table has this format)
	if(png2AssetData->arguments.use_2x2_map_attributes)
	{
		// NES attribute map dimensions are half-resolution 
		ReduceMapAttributes2x2(png2AssetData->palettes, png2AssetData);
	}
	// Optionally align and pack map attributes into NES PPU format
	if(png2AssetData->arguments.pack_map_attributes)
	{
		AlignMapAttributes(png2AssetData);
		PackMapAttributes(png2AssetData);
	}
	else
	{
		// Use original attribute dimensions for packed
		png2AssetData->arguments.map_attributes_packed_width = png2AssetData->arguments.map_attributes_width;
		png2AssetData->arguments.map_attributes_packed_height = png2AssetData->arguments.map_attributes_height;
	}
}